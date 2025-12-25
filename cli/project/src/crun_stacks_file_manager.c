/**
 * @file      crun_stacks_file_manager.c
 * @author    @ZouariOmar <zouariomar20@gmail.com>
 * @brief     crun_stacks_file_manager source file
 * @version   0.1
 * @date      2025-12-25
 * @copyright Copyright (c) 2025
 * @link https://github.com/ZouariOmar/crun/project/src/crun_stacks_file_manager.c crun_stacks_file_manager.c @endlink
 */

// ########################
// ### INC HEADERS PART ###
// ########################

// Include curl header(s)
#include <cjson/cJSON.h>
#include <curl/curl.h>

// Include std header(s)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
#include <io.h>
#define F_OK 0
#define access _access
#else
#include <unistd.h>
#endif

// Include custom header(s)
#include "../common/patterns.h"
#include "../inc/crun_stacks_file_manager.h"

const char *crun_stacks_json_file_path = NULL;

/**
 * @brief Initialize the crun stacks manager
 *
 * @details
 * 1. Retrieve the default `crun_stacks.json` path (where suppose to be)
 * 2. Verify the existence of the file
 * 3. [-] Download the `crun_stacks.json` in the default path
 *
 * @return void
 */
void crun_stacks_manager_init() {
  // get `crun_stacks.json` path
  if (!crun_stacks_json_file_path)
    crun_stacks_json_file_path = get_file_path();

  if (!is_file_exist(crun_stacks_json_file_path)) { // Download `crun_stacks_json`
    printf("[WARR] %s not exist!\n", crun_stacks_json_file_path);
    printf("[INFO] Downloading crun_stacks.json...\n");

    if (download_file(CRUN_STACKS_JSON_URL, crun_stacks_json_file_path))
      fprintf(stderr, "[ERROR] crun_stacks_file_manager.c :: L57 :: crun_stacks.json Download failed!\n");
    else
      fprintf(stdout, "[INFO] crun_stacks.json Downloaded successfully!\n");
  }
}

char *read_crun_stacks_json_file() {
  if (!crun_stacks_json_file_path)
    crun_stacks_json_file_path = get_file_path();

  // Open `crun_stacks.json`
  FILE *fp = fopen(crun_stacks_json_file_path, "r");
  if (!fp) {
    perror("[ERROR] crun_stacks_file_manager.c :: L70 :: Unable to open 'crun_stacks.json' file!\n");
    fclose(fp);
    return NULL;
  }

  // Get file size
  long file_size = get_file_size(fp);

  // Allocate `buffer`
  char *buffer = malloc((file_size * sizeof(char)));
  if (!buffer) {
    fprintf(stderr, "[ERROR] crun_stacks_file_manager.c :: L81 :: Can't Allocate the `buffer`!");
    fclose(fp);
    return NULL;
  }

  // Read `crun_stacks.json`
  size_t len = fread(buffer, 1, file_size, fp);
  buffer[len] = '\0';

  // Cleaning
  fclose(fp);

  return buffer;
}

int is_file_exist(const char *file_path) { // out: 0 ==> exist
  return !access(file_path, F_OK);
}

// ##################################
// ### GETTERS FUNCTIONS DEV PART ###
// ##################################

const char *get_file_path() {
  const char *home = OS_HOME;
  if (!home)
    return NULL;

#ifdef _WIN32
  const char *suffix = "\\crun\\crun_stacks.json";
#else
  const char *suffix = "/.local/share/crun/crun_stacks.json";
#endif

  size_t len = strlen(home) + strlen(suffix) + 1;
  char *path = (char *)malloc(len * sizeof(char));
  if (!path)
    return NULL;

  snprintf(path, len, "%s%s", home, suffix);
  return path;
}

long get_file_size(FILE *fp) {
  fseek(fp, 0, SEEK_END);
  long file_size = ftell(fp);
  rewind(fp);
  return file_size;
}

cJSON *get_root_json(char *crun_stacks_buffer) {
  // Parse the JSON data
  cJSON *json = cJSON_Parse(crun_stacks_buffer);

  if (!json) {
    fprintf(stderr, "[ERROR] crun_stacks_file_manager.c :: L138 :: Can't Parse `crun_stacks.json` file!\n");
    cJSON_Delete(json);
    free(crun_stacks_buffer);
    return NULL;
  }

  const char *error_ptr = cJSON_GetErrorPtr();
  if (error_ptr) {
    fprintf(stderr, "[ERROR] crun_stacks_file_manager.c :: L146 :: CJSON ERROR PTR: %s\n", error_ptr);
    cJSON_Delete(json);
    free(crun_stacks_buffer);
    return NULL;
  }

  return json;
}

cJSON *get_stacks_object(cJSON *root) {
  // Access `stacks` object
  cJSON *stacks = cJSON_GetObjectItemCaseSensitive(root, "stacks");
  if (!cJSON_IsObject(stacks)) {
    fprintf(stderr, "[ERROR] crun_stacks_file_manager.c :: L159 :: Invalid or missing stacks\n");
    cJSON_Delete(root);
    return NULL;
  }

  return stacks;
}

char *get_language_buffer(char ***language_map, size_t *languages_number, cJSON *root) {
  char *crun_stacks_buff = read_crun_stacks_json_file(),
       *lang_buff = NULL,
       *lang_line = NULL;

  cJSON *stacks = get_stacks_object(root),
        *lang = NULL;

  *languages_number = 1;

  cJSON_ArrayForEach(lang, stacks) {
    char *lang_name = lang->string;

    if (!cJSON_IsArray(lang))
      continue;

    // Reallocate `language_map`
    char **tmp = (char **)realloc(*language_map, (*languages_number) * sizeof(char *));
    if (!tmp) {
      fprintf(stderr, "[ERROR] crun_stacks_file_manager.c :: L187 :: Can't Reallocate the `language_map`!");
      return NULL;
    }
    *language_map = tmp;

    // Fill the `language_map`
    (*language_map)[*languages_number - 1] = strdup(lang_name);

    // Allocate the `lang_line`
    lang_line = (char *)malloc(PROJECT_MENU_FIELD_LENGTH((int)*languages_number, lang_name) * sizeof(char));
    if (!lang_line) {
      fprintf(stderr, "[ERROR] crun_stacks_file_manager.c :: L198 :: Can't Allocate the `lang_line`!");
      return NULL;
    }

    // Fill the `lang_line`
    PROJECT_MENU_FIELD(lang_line, (int)(*languages_number)++, lang_name);

    // Reallocate `lang_buff`
    size_t line_len = strlen(lang_line),
           buff_len = lang_buff ? strlen(lang_buff) : 0;
    char *buff_tmp = (char *)realloc(lang_buff, buff_len + line_len + 1);
    if (!buff_tmp) {
      fprintf(stderr, "[ERROR] crun_stacks_file_manager.c :: L209 :: Can't Reallocate the `lang_buff`!");
      return NULL;
    }
    lang_buff = buff_tmp;
    lang_buff[buff_len + line_len] = '\0';

    // Append `lang_line` into `buff`
    memcpy(lang_buff + buff_len, lang_line, line_len);
  }

  // Cleaning
  free(lang_line);
  lang_line = NULL;
  free(crun_stacks_buff);
  crun_stacks_buff = NULL;

  return lang_buff;
}

char *get_projects_buffer(const char *language, cJSON *root) {
  cJSON *stacks = cJSON_GetObjectItem(root, "stacks");
  cJSON *lang_array = cJSON_GetObjectItem(stacks, language);
  if (!cJSON_IsArray(lang_array))
    return NULL;

  char *projects_buffer = NULL;
  size_t projects_buffer_len = 0;
  int index = 1;
  cJSON *item;

  cJSON_ArrayForEach(item, lang_array) {
    cJSON *name = cJSON_GetObjectItem(item, "name");
    if (cJSON_IsString(name)) {
      const char *nameStr = name->valuestring;

      // Allocate the `lang_line`
      char *project_line = (char *)malloc(PROJECT_MENU_FIELD_LENGTH(index, nameStr) * sizeof(char));
      if (!project_line) {
        fprintf(stderr, "[ERROR] crun_stacks_file_manager.c :: L198 :: Can't Allocate the `lang_line`!");
        return NULL;
      }

      // Fill the `lang_line`
      PROJECT_MENU_FIELD(project_line, index++, nameStr);

      // Reallocate `lang_buff`
      size_t line_len = strlen(project_line),
             buff_len = projects_buffer ? strlen(projects_buffer) : 0;
      char *buff_tmp = (char *)realloc(projects_buffer, buff_len + line_len + 1);
      if (!buff_tmp) {
        fprintf(stderr, "[ERROR] crun_stacks_file_manager.c :: L209 :: Can't Reallocate the `lang_buff`!");
        return NULL;
      }
      projects_buffer = buff_tmp;
      projects_buffer[buff_len + line_len] = '\0';

      // Append `lang_line` into `buff`
      memcpy(projects_buffer + buff_len, project_line, line_len);

      // Cleaning
      free(project_line);
    }
  }

  return projects_buffer;
}

// ##################################
// ### LIBCURL FUNCTIONS DEV PART ###
// ##################################

size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
  return fwrite(ptr, size, nmemb, (FILE *)stream);
}

int download_file(const char *url, const char *out) {
  CURL *curl = curl_easy_init();
  if (!curl)
    return EXIT_FAILURE;

  FILE *fp = fopen(out, "wb");
  if (!fp) {
    fprintf(stdout, "[WARR] crun_stacks_file_manager.c :: L236 :: Can't open/write %s\n", out);
    curl_easy_cleanup(curl);
    return EXIT_FAILURE;
  }

  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

  CURLcode res = curl_easy_perform(curl);

  // Cleaning
  fclose(fp);
  fp = NULL;
  curl_easy_cleanup(curl);
  curl = NULL;

  return res != CURLE_OK;
}
