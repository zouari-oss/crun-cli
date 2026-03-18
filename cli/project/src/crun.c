/**
 * @file      crun.c
 * @author    @ZouariOmar (zouariomar20@gmail.com)
 * @brief     crun source file
 * @version   0.2
 * @date      2025-11-26
 * @copyright Copyright (c) 2025
 * @link https://github.com/ZouariOmar/crun/project/src/crun.c crun.c @endlink
 */

// ############################
// ### HEADERS INCLUDE PART ###
// ############################

// Include std c header(s)
#include <cjson/cJSON.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#ifdef _WIN32
#include <process.h>
#define __get_pid _getpid
#else
#include <unistd.h>
#define __get_pid getpid
#endif

// Include custom header(s)
#include "../common/patterns.h"
#include "../inc/crun.h"
#include "../inc/crun_audit.h"
#include "../inc/crun_file_manager.h"
#include "../inc/crun_json_manager.h"
#include "../inc/crun_libcurl.h"
#include "../inc/crun_zip_manager.h"

// Declare and define the shared vars
struct CrunLanguage *languages_map = NULL;
size_t languages_map_length;
struct CrunPackage *packages_map = NULL;
size_t packages_map_length;

static int run_init_script(const char *init_script_path) {
  if (!init_script_path || !strlen(init_script_path))
    return EXIT_FAILURE;

  char init_directory[FILENAME_MAX] = {0};
  char init_script_name[FILENAME_MAX] = {0};
  char cmd[(FILENAME_MAX * 2) + 64];
  snprintf(init_directory, sizeof(init_directory), "%s", init_script_path);

  char *separator = strrchr(init_directory, '/');
  if (!separator)
    separator = strrchr(init_directory, '\\');
  if (!separator)
    return EXIT_FAILURE;

  snprintf(init_script_name, sizeof(init_script_name), "%s", separator + 1);
  *separator = '\0';

#ifdef _WIN32
  snprintf(cmd, sizeof(cmd), "cd /d \"%s\" && cmd /c \"%s\"", init_directory, init_script_name);
#else
  snprintf(cmd, sizeof(cmd), "cd \"%s\" && bash \"./%s\"", init_directory, init_script_name);
#endif

  const int run_res = system(cmd);
  return run_res == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

static int ensure_directory(const char *path) {
  if (is_file_exist(path))
    return EXIT_SUCCESS;

  return MKDIR(path) == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

static const char *build_hidden_extract_directory() {
  char tmp_suffix[FILENAME_MAX] = {0};
  char workspace_suffix[FILENAME_MAX] = {0};

  snprintf(tmp_suffix, sizeof(tmp_suffix), "%stmp", CRUN_DEFAULT_SUFFIX_DIRECTORY);
  snprintf(workspace_suffix,
           sizeof(workspace_suffix),
           "%stmp%cworkspace_%ld_%ld",
           CRUN_DEFAULT_SUFFIX_DIRECTORY,
           PATH_SEP,
           (long)time(NULL),
           (long)__get_pid());

  const char *crun_root_directory = get_file_home_path(CRUN_DEFAULT_SUFFIX_DIRECTORY);
  if (!crun_root_directory)
    return NULL;

  if (ensure_directory(crun_root_directory) != EXIT_SUCCESS) {
    free((void *)crun_root_directory);
    return NULL;
  }
  free((void *)crun_root_directory), crun_root_directory = NULL;

  const char *tmp_directory = get_file_home_path(tmp_suffix);
  if (!tmp_directory)
    return NULL;

  if (ensure_directory(tmp_directory) != EXIT_SUCCESS) {
    free((void *)tmp_directory);
    return NULL;
  }
  free((void *)tmp_directory), tmp_directory = NULL;

  const char *workspace_directory = get_file_home_path(workspace_suffix);
  if (!workspace_directory)
    return NULL;

  if (ensure_directory(workspace_directory) != EXIT_SUCCESS) {
    free((void *)workspace_directory);
    return NULL;
  }

  return workspace_directory;
}

static int copy_extract_output_to_directory(const char *source_directory,
                                            const char *target_directory) {
  char cmd[(FILENAME_MAX * 4) + 256];

#ifdef _WIN32
  snprintf(cmd,
           sizeof(cmd),
           "robocopy \"%s\" \"%s\" /E /XF *.zip >NUL & if %%ERRORLEVEL%% LEQ 7 (exit /b 0) else (exit /b 1)",
           source_directory,
           target_directory);
#else
  snprintf(cmd,
           sizeof(cmd),
           "tar -C \"%s\" --exclude='*.zip' -cf - . | tar -C \"%s\" -xf -",
           source_directory,
           target_directory);
#endif

  return system(cmd) == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

static void cleanup_hidden_workspace(const char *hidden_extract_directory) {
  if (!hidden_extract_directory || !strlen(hidden_extract_directory))
    return;

  char cmd[(FILENAME_MAX * 2) + 128] = {0};
#ifdef _WIN32
  snprintf(cmd, sizeof(cmd), "rmdir /S /Q \"%s\" >NUL 2>&1", hidden_extract_directory);
#else
  snprintf(cmd, sizeof(cmd), "rm -rf \"%s\"", hidden_extract_directory);
#endif
  system(cmd);

  char tmp_directory[FILENAME_MAX] = {0};
  snprintf(tmp_directory, sizeof(tmp_directory), "%s", hidden_extract_directory);
  char *separator = strrchr(tmp_directory, '/');
  if (!separator)
    separator = strrchr(tmp_directory, '\\');
  if (!separator)
    return;
  *separator = '\0';

#ifdef _WIN32
  snprintf(cmd, sizeof(cmd), "rmdir \"%s\" >NUL 2>&1", tmp_directory);
#else
  snprintf(cmd, sizeof(cmd), "rmdir \"%s\" >/dev/null 2>&1", tmp_directory);
#endif
  system(cmd);
}

static void remove_if_exist(const char *file_path) {
  if (!file_path || !is_file_exist(file_path))
    return;

  if (remove(file_path))
    fprintf(stderr, "[WARR] crun.c :: remove_if_exist :: Can't remove %s\n", file_path);
}

static void cleanup_init_scripts(const char *init_script_path) {
  if (!init_script_path || !strlen(init_script_path))
    return;

  char init_directory[FILENAME_MAX] = {0};
  snprintf(init_directory, sizeof(init_directory), "%s", init_script_path);

  char *separator = strrchr(init_directory, '/');
  if (!separator)
    separator = strrchr(init_directory, '\\');
  if (!separator)
    return;

  *separator = '\0';

  char init_sh_file_path[FILENAME_MAX] = {0};
  char init_bat_file_path[FILENAME_MAX] = {0};
  snprintf(init_sh_file_path, sizeof(init_sh_file_path), "%s%c__init__.sh", init_directory, PATH_SEP);
  snprintf(init_bat_file_path, sizeof(init_bat_file_path), "%s%c__init__.bat", init_directory, PATH_SEP);

  remove_if_exist(init_sh_file_path);
  remove_if_exist(init_bat_file_path);
}

// #########################
// ### FUNCTION DEV PART ###
// #########################

void crun() {
  CRUN_HEADER;
  CRUN_BANNER;

  const char *crun_stacks_json_file_path = get_file_home_path(CRUN_STACKS_JSON_FILE_DEFAULT_SUFFIX); //! Free this ptr
  crun_stacks_json_checker(crun_stacks_json_file_path);
  const char *json_root_buffer = get_file_data(crun_stacks_json_file_path); //! Free this ptr
  cJSON *json_root = get_json_root(json_root_buffer);                       //! Free this ptr

  char *languages_buffer = get_language_buffer(json_root); //! Free this ptr
  if (!languages_buffer || !strlen(languages_buffer)) {
    crun_audit_error("No languages available in stacks metadata.");
    free_all((void *[]){languages_buffer, (void *)crun_stacks_json_file_path}, 2);
    return;
  }

  int user_choice = -1;

  // === Get User language menu choice ===
  get_user_choice(&user_choice,
                  languages_buffer,
                  languages_map_length);

  if (!user_choice) { // EXIT_SUCCESS
    EXIT_SUCCESS_MSG;
    return;
  }

  const char *selected_language_name = languages_map[user_choice - 1].name;

  // Get language projects
  char *packages_buffer = get_packages_buffer(json_root, selected_language_name);
  if (!packages_buffer || !strlen(packages_buffer)) {
    crun_audit_error("Unable to load package templates for language '%s'.", languages_map[user_choice - 1].name);
    free_all((void *[]){languages_buffer, (void *)crun_stacks_json_file_path}, 2);
    cJSON_Delete(json_root), json_root = NULL;
    return;
  }

  // === Get user package choice ===
  get_user_choice(&user_choice,
                  packages_buffer,
                  packages_map_length);

  if (!user_choice) { // EXIT_SUCCESS
    EXIT_SUCCESS_MSG;
    return;
  }

  crun_audit_info("Template selected: language='%s', template='%s'.",
                  selected_language_name,
                  packages_map[user_choice - 1].name);

  // See if the project exist first, otherwise download the pkg
  const char *crun_package_file_path = download_crun_package(&packages_map[user_choice - 1]); // Free this ptr
  if (!crun_package_file_path) {
    crun_audit_error("Failed to retrieve package archive '%s.zip'.", packages_map[user_choice - 1].name);
    free_all((void *[]){languages_buffer, (void *)crun_stacks_json_file_path}, 2);
    cJSON_Delete(json_root), json_root = NULL;
    return;
  }

  // === Generate the project ===

  char user_current_directory[FILENAME_MAX] = {0};
  if (!__get_current_dir(user_current_directory, FILENAME_MAX)) {
    crun_audit_error("Unable to resolve current working directory.");
    free_all((void *[]){languages_buffer, (void *)crun_stacks_json_file_path}, 2);
    cJSON_Delete(json_root), json_root = NULL;
    return;
  }

  const char *hidden_extract_directory = build_hidden_extract_directory();
  if (!hidden_extract_directory || !strlen(hidden_extract_directory)) {
    crun_audit_error("Unable to prepare temporary extraction workspace.");
    free_all((void *[]){languages_buffer, (void *)crun_stacks_json_file_path, (void *)hidden_extract_directory}, 3);
    cJSON_Delete(json_root), json_root = NULL;
    return;
  }

  char init_script_path[FILENAME_MAX] = {0};
  if (extract_zip(crun_package_file_path, hidden_extract_directory, init_script_path, sizeof(init_script_path)) != EXIT_SUCCESS) {
    crun_audit_error("Failed to extract archive '%s'.", crun_package_file_path);
    cleanup_hidden_workspace(hidden_extract_directory);
    free_all((void *[]){languages_buffer, (void *)crun_stacks_json_file_path, (void *)hidden_extract_directory}, 3);
    cJSON_Delete(json_root), json_root = NULL;
    return;
  }

  if (strlen(init_script_path)) {
    const int init_res = run_init_script(init_script_path);
    cleanup_init_scripts(init_script_path);

    if (init_res != EXIT_SUCCESS) {
      crun_audit_error("Init script execution failed: %s", init_script_path);
      cleanup_hidden_workspace(hidden_extract_directory);
      free_all((void *[]){languages_buffer, (void *)crun_stacks_json_file_path, (void *)hidden_extract_directory}, 3);
      cJSON_Delete(json_root), json_root = NULL;
      return;
    }
  }

  if (copy_extract_output_to_directory(hidden_extract_directory, user_current_directory) != EXIT_SUCCESS) {
    crun_audit_error("Failed to copy extracted output into current directory.");
    cleanup_hidden_workspace(hidden_extract_directory);
    free_all((void *[]){languages_buffer, (void *)crun_stacks_json_file_path, (void *)hidden_extract_directory}, 3);
    cJSON_Delete(json_root), json_root = NULL;
    return;
  }

  cleanup_hidden_workspace(hidden_extract_directory);

  crun_audit_info("Project scaffold ready.");
  INSTALLATION_COMPLETE_MSG;

  free_all((void *[]){languages_buffer, (void *)crun_stacks_json_file_path, (void *)hidden_extract_directory}, 3);
  cJSON_Delete(json_root);
  json_root = NULL;
}

const char *download_crun_package(const struct CrunPackage *crun_package_map) {
  const char *package_name = crun_package_map->name,
             *package_url = crun_package_map->url,
             *crun_package_file_path = NULL;

  size_t crun_package_file_suffix_len =
      strlen(CRUN_DEFAULT_SUFFIX_DIRECTORY) +
      strlen(package_name) +
      strlen(".zip") + 1;

  char *crun_package_file_suffix = (char *)malloc(crun_package_file_suffix_len);

  if (!crun_package_file_suffix) {
    crun_audit_error("Memory allocation failed while preparing package cache path.");
    return NULL;
  }

  snprintf(crun_package_file_suffix,
           crun_package_file_suffix_len,
           "%s%s.zip",
           CRUN_DEFAULT_SUFFIX_DIRECTORY,
           package_name);

  crun_package_file_path = get_file_home_path(crun_package_file_suffix);
  free(crun_package_file_suffix), crun_package_file_suffix = NULL; // Free `crun_package_file_suffix`

  if (!is_file_exist(crun_package_file_path)) {
    crun_audit_warn("Template archive not found in cache: %s", crun_package_file_path);
    crun_audit_info("Downloading template archive '%s.zip'...", package_name);
    const int download_res = download_file(package_url, crun_package_file_path);
    if (download_res != EXIT_SUCCESS)
      crun_audit_error("Failed to download package '%s.zip'.", package_name);

    return download_res == EXIT_SUCCESS ? crun_package_file_path : NULL;
  }

  crun_audit_info("Using cached template archive: %s", crun_package_file_path);
  return crun_package_file_path;
}

char ask_yes_no(const char *question) {
  char choice = '\0';

  do {
    printf("%s (y/n): ", question);
    if (scanf(" %c", &choice) != 1) {
      UNACCEPTED_INPUT_MSG;
      while (getchar() != '\n')
        ;
    } else {
      choice = tolower(choice);
    }
  } while (choice != 'y' && choice != 'n');

  return choice;
}

char *get_user_project_name() {
  char buffer[256];

  while (1) {
    PROJECT_NAME;

    while (getchar() != '\n')
      ;

    if (!fgets(buffer, sizeof(buffer), stdin)) {
      UNACCEPTED_INPUT_MSG;
      continue;
    }

    char *project_name = malloc(strlen(buffer) + 1);
    if (!project_name) {
      fprintf(stderr, "[ERROR] crun.c :: Can't allocate `project_name`!\n");
      return NULL;
    }

    strcpy(project_name, buffer);

    return project_name;
  }

  return NULL;
}

void get_user_choice(int *user_choice, const char *menu, const int limiter) {
  while (1) {
    PROJECT_MENU(menu);

    if (scanf("%d", user_choice) != 1) { // invalid input
      UNACCEPTED_INPUT_MSG;
      *user_choice = -1;
      while (getchar() != '\n')
        ;

    } else if (*user_choice < 0 || *user_choice >= limiter) {
      INVALID_OPTION_MSG;

    } else
      break;
  }
}

/**
 * @brief Check/Fix the `crun_stacks.json`
 *
 * @details
 * 1. Retrieve the default `crun_stacks.json` path (where suppose to be)
 * 2. Verify the existence of the file
 * 3. [-] Download the `crun_stacks.json` in the default path
 *
 * @return void
 */
void crun_stacks_json_checker(const char *crun_stacks_json_file_path) {
  if (!is_file_exist(crun_stacks_json_file_path)) { // Download `crun_stacks_json`
    crun_audit_warn("Stacks metadata not found at '%s'.", crun_stacks_json_file_path);
    crun_audit_info("Downloading crun_stacks.json...");

    if (download_file(CRUN_STACKS_JSON_FILE_URL, crun_stacks_json_file_path) == EXIT_SUCCESS)
      crun_audit_info("crun_stacks.json downloaded successfully.");
    else
      crun_audit_error("Failed to download crun_stacks.json.");
  }
}

char *get_language_buffer(cJSON *root) {
  cJSON *stacks_object = get_json_object(root, "stacks"),
        *lang = NULL;

  languages_map_length = 1;

  char *lang_buff = NULL,
       *lang_line = NULL;

  cJSON_ArrayForEach(lang, stacks_object) {
    char *lang_name = lang->string;

    if (!cJSON_IsArray(lang))
      continue;

    // Reallocate `languages_map`
    struct CrunLanguage *tmp = (struct CrunLanguage *)realloc(languages_map,
                                                              languages_map_length * sizeof(struct CrunLanguage));
    if (!tmp)
      return fprintf(stderr, "[ERROR] crun.c :: Can't Reallocate the `languages_map`!"), NULL;

    languages_map = tmp;

    // Fill the `languages_map`
    languages_map[languages_map_length - 1].name = strdup(lang_name);

    // Allocate the `lang_line`
    lang_line = (char *)malloc(PROJECT_MENU_FIELD_LENGTH((int)languages_map_length, lang_name) * sizeof(char));

    if (!lang_line)
      return fprintf(stderr, "[ERROR] crun.c :: Can't Allocate the `lang_line`!"), NULL;

    // Fill the `lang_line`
    PROJECT_MENU_FIELD(lang_line, (int)languages_map_length++, lang_name);

    // Reallocate `lang_buff`
    size_t line_len = strlen(lang_line),
           buff_len = lang_buff ? strlen(lang_buff) : 0;

    char *buff_tmp = (char *)realloc(lang_buff, buff_len + line_len + 1);

    if (!buff_tmp) {
      fprintf(stderr, "[ERROR] crun_json_manager.c :: L209 :: Can't Reallocate the `lang_buff`!");
      return free(lang_line), lang_line = NULL, NULL;
    }

    lang_buff = buff_tmp;
    lang_buff[buff_len + line_len] = '\0';

    // Append `lang_line` into `buff`
    memcpy(lang_buff + buff_len, lang_line, line_len);

    // Cleaning
    free(lang_line), lang_line = NULL;
  }

  return lang_buff;
}

char *get_packages_buffer(cJSON *root, const char *language_name) {
  cJSON *stacks_object = get_json_object(root, "stacks"),
        *packages_array = cJSON_GetObjectItem(stacks_object, language_name);

  packages_map_length = 1;

  if (!cJSON_IsArray(packages_array)) {
    crun_audit_error("Invalid stacks schema for language '%s' (expected an array).", language_name);
    return NULL;
  }

  if (cJSON_GetArraySize(packages_array) == 0) {
    crun_audit_warn("No templates are available for language '%s'.", language_name);
    return strdup("\t\t\t(no templates available)\n");
  }

  char *packages_buffer = NULL;
  cJSON *item = NULL;

  cJSON_ArrayForEach(item, packages_array) {
    cJSON *name = cJSON_GetObjectItem(item, "name"),
          *description = cJSON_GetObjectItem(item, "description"),
          *url = cJSON_GetObjectItem(item, "url");

    if (!cJSON_IsString(name) || !cJSON_IsString(description) || !cJSON_IsString(url))
      continue;

    const char *name_str = name->valuestring,
               *description_str = description->valuestring,
               *url_str = url->valuestring;

    // Reallocate `languages_map`
    struct CrunPackage *tmp = (struct CrunPackage *)realloc(packages_map,
                                                            packages_map_length * sizeof(struct CrunPackage));
    if (!tmp)
      return fprintf(stderr, "[ERROR] crun.c :: Can't Reallocate the `packages_map`!"), NULL;

    packages_map = tmp;

    // Fill the `packages_map`
    packages_map[packages_map_length - 1].name = strdup(name_str);
    packages_map[packages_map_length - 1].description = strdup(description_str);
    packages_map[packages_map_length - 1].url = strdup(url_str);

    // Allocate the `package_line`
    char *package_line = (char *)malloc(PROJECT_MENU_FIELD_LENGTH((int)packages_map_length, name_str) * sizeof(char));

    if (!package_line) {
      fprintf(stderr, "[ERROR] crun_json_manager.c :: Can't Allocate the `package_line`!");
      return NULL;
    }

    // Fill the `package_line`
    PROJECT_MENU_FIELD(package_line, (int)packages_map_length++, name_str);

    // Reallocate `packages_buffer`
    size_t line_len = strlen(package_line),
           buff_len = packages_buffer ? strlen(packages_buffer) : 0;

    char *buff_tmp = (char *)realloc(packages_buffer, buff_len + line_len + 1);

    if (!buff_tmp) {
      fprintf(stderr, "[ERROR] crun_json_manager.c :: Can't Reallocate the `lang_buff`!");
      return free(package_line), package_line = NULL, NULL;
    }

    packages_buffer = buff_tmp;
    packages_buffer[buff_len + line_len] = '\0';

    // Append `package_line` into `packages_buffer`
    memcpy(packages_buffer + buff_len, package_line, line_len);

    // Cleaning
    free(package_line), package_line = NULL;
  }

  return packages_buffer;
}

/**
 * @brief Free all the allocated pointers
 *
 * @details We free:
 * - languages_map
 * - language_buffer
 * - crun_stacks_json_file_path
 * - more..
 *
 * @return description
 */
void free_all(void **ptrs, size_t len) {
  // Free `languages_map` items
  for (size_t i = 0; i < languages_map_length - 1; ++i)
    if (languages_map[i].name)
      free(languages_map[i].name), languages_map[i].name = NULL;

  // Free `languages_map`
  if (languages_map)
    free(languages_map), languages_map = NULL;

  // Free `packages_map` items
  for (size_t i = 0; i < packages_map_length - 1; ++i) {
    if (packages_map[i].name)
      free(packages_map[i].name), packages_map[i].name = NULL;
    if (packages_map[i].description)
      free(packages_map[i].description), packages_map[i].description = NULL;
    if (packages_map[i].url)
      free(packages_map[i].url), packages_map[i].url = NULL;
  }

  // Free `packages_map`
  if (packages_map)
    free(packages_map), packages_map = NULL;

  // Free Other ptr(s)
  for (size_t i = 0; i < len; ++i)
    if (ptrs[i])
      free(ptrs[i]), ptrs[i] = NULL;
}
