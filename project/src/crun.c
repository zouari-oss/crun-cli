/**
 * @file      crun.c
 * @author    ZouariOmar (zouariomar20@gmail.com)
 * @brief     crun source file
 * @version   0.2
 * @date      2025-11-26
 * @copyright Copyright (c) 2025
 *
 * <a href="https://github.com/zouari-oss/crun-cli/project/src/crun.c">crun.c</a>
 */

#include "../inc/crun.h"
#include "../common/patterns.h"
#include "../inc/crun_audit.h"
#include "../inc/crun_file_manager.h"
#include "../inc/crun_json_manager.h"
#include "../inc/crun_libcurl.h"
#include "../inc/crun_zip_manager.h"
#include <cjson/cJSON.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

struct CrunLanguage *languages_map = NULL;
size_t languages_map_length = 1;
struct CrunPackage *packages_map = NULL;
size_t packages_map_length = 1;

static char *get_last_path_separator(char *path) {
  if (!path)
    return NULL;

  char *separator = strrchr(path, '/');
  if (!separator)
    separator = strrchr(path, '\\');
  return separator;
}

static const char *get_path_basename(const char *path) {
  if (!path || !strlen(path))
    return NULL;

  const char *slash = strrchr(path, '/');
  const char *backslash = strrchr(path, '\\');
  const char *separator = slash > backslash ? slash : backslash;
  return separator ? separator + 1 : path;
}

static int append_url_extension(const char *url, char *buffer, size_t buffer_size) {
  if (!url || !buffer || !buffer_size)
    return EXIT_FAILURE;

  const char *basename = strrchr(url, '/');
  basename = basename ? basename + 1 : url;
  if (!strlen(basename))
    return EXIT_FAILURE;

  const char *fragment = strchr(basename, '#');
  const char *query = strchr(basename, '?');
  const char *end = basename + strlen(basename);
  if (fragment && fragment < end)
    end = fragment;
  if (query && query < end)
    end = query;

  const char *dot = NULL;
  for (const char *cursor = end; cursor > basename; --cursor) {
    if (*(cursor - 1) == '.') {
      dot = cursor - 1;
      break;
    }
  }

  if (!dot || dot == end)
    return EXIT_FAILURE;

  const size_t ext_len = (size_t)(end - dot);
  if (ext_len + 1 > buffer_size)
    return EXIT_FAILURE;

  memcpy(buffer, dot, ext_len);
  buffer[ext_len] = '\0';
  return EXIT_SUCCESS;
}

static int ends_with_ignore_case(const char *value, const char *suffix) {
  if (!value || !suffix)
    return 0;

  const size_t value_len = strlen(value);
  const size_t suffix_len = strlen(suffix);
  if (suffix_len == 0 || value_len < suffix_len)
    return 0;

  const char *value_end = value + (value_len - suffix_len);
  for (size_t i = 0; i < suffix_len; ++i) {
    if (tolower((unsigned char)value_end[i]) != tolower((unsigned char)suffix[i]))
      return 0;
  }

  return 1;
}

static int install_single_file_template(const char *source_path, const char *target_directory) {
  if (!source_path || !target_directory || !strlen(source_path) || !strlen(target_directory))
    return EXIT_FAILURE;

  const char *source_basename = get_path_basename(source_path);
  if (!source_basename || !strlen(source_basename))
    return EXIT_FAILURE;

  char target_path[FILENAME_MAX] = {0};
  snprintf(target_path, sizeof(target_path), "%s%c%s", target_directory, PATH_SEP, source_basename);

  if (copy_file_to_path(source_path, target_path) != EXIT_SUCCESS) {
    crun_audit_error("Failed to install single-file template '%s'.", source_basename);
    return EXIT_FAILURE;
  }

  crun_audit_info("Single-file template installed: %s", target_path);
  return EXIT_SUCCESS;
}

static int append_menu_line(char **buffer, const char *line) {
  if (!buffer || !line)
    return EXIT_FAILURE;

  const size_t line_len = strlen(line);
  const size_t buffer_len = *buffer ? strlen(*buffer) : 0;
  char *tmp = (char *)realloc(*buffer, buffer_len + line_len + 1);
  if (!tmp)
    return EXIT_FAILURE;

  *buffer = tmp;
  memcpy(*buffer + buffer_len, line, line_len);
  (*buffer)[buffer_len + line_len] = '\0';
  return EXIT_SUCCESS;
}

static int run_init_script(const char *init_script_path) {
  if (!init_script_path || !strlen(init_script_path))
    return EXIT_FAILURE;

  char init_directory[FILENAME_MAX] = {0};
  char init_script_name[FILENAME_MAX] = {0};
  char cmd[(FILENAME_MAX * 2) + 64];
  snprintf(init_directory, sizeof(init_directory), "%s", init_script_path);

  char *separator = get_last_path_separator(init_directory);
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

  snprintf(tmp_suffix, sizeof(tmp_suffix), "%s", CRUN_TMP_DIRECTORY_SUFFIX);
  snprintf(workspace_suffix,
           sizeof(workspace_suffix),
           "%s%cworkspace_%ld_%ld",
           CRUN_TMP_DIRECTORY_SUFFIX,
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
  char *separator = get_last_path_separator(tmp_directory);
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
    crun_audit_warn("Unable to remove temporary file '%s'.", file_path);
}

static void cleanup_init_scripts(const char *init_script_path) {
  if (!init_script_path || !strlen(init_script_path))
    return;

  char init_directory[FILENAME_MAX] = {0};
  snprintf(init_directory, sizeof(init_directory), "%s", init_script_path);

  char *separator = get_last_path_separator(init_directory);
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

static void cleanup_runtime_context(struct CrunRuntimeContext *ctx) {
  if (!ctx)
    return;

  cleanup_hidden_workspace(ctx->workspace_path);
  free_all((void *[]){
               ctx->languages_buffer,
               ctx->packages_buffer,
               (void *)ctx->json_root_buffer,
               (void *)ctx->stacks_path,
               (void *)ctx->workspace_path,
           },
           5);
  cJSON_Delete(ctx->json_root);
}

void crun_version() {
  VERSION_MSG;
}

void crun_help() {
  HELP_MSG;
}

int crun_update() {
  const char *stacks_path = get_file_home_path(CRUN_STACKS_JSON_FILE_DEFAULT_SUFFIX);
  if (!stacks_path) {
    crun_audit_error("Unable to resolve stacks metadata path.");
    return EXIT_FAILURE;
  }

  const char *crun_root_directory = get_file_home_path(CRUN_DEFAULT_SUFFIX_DIRECTORY);
  if (!crun_root_directory) {
    free((void *)stacks_path);
    crun_audit_error("Unable to resolve crun data directory.");
    return EXIT_FAILURE;
  }

  if (ensure_directory(crun_root_directory) != EXIT_SUCCESS) {
    free((void *)stacks_path);
    free((void *)crun_root_directory);
    crun_audit_error("Unable to prepare crun data directory.");
    return EXIT_FAILURE;
  }
  free((void *)crun_root_directory);

  crun_audit_info("Updating stacks metadata...");
  const int update_res = download_file(CRUN_STACKS_JSON_FILE_URL, stacks_path);
  if (update_res == EXIT_SUCCESS)
    crun_audit_info("Stacks metadata updated successfully.");
  else
    crun_audit_error("Stacks metadata update failed.");

  free((void *)stacks_path);
  return update_res;
}

void crun() {
  CRUN_HEADER;
  CRUN_BANNER;

  struct CrunRuntimeContext ctx = {0};

  ctx.stacks_path = get_file_home_path(CRUN_STACKS_JSON_FILE_DEFAULT_SUFFIX);
  if (!ctx.stacks_path) {
    crun_audit_error("Unable to resolve stacks metadata path.");
    return;
  }

  crun_stacks_json_checker(ctx.stacks_path);
  ctx.json_root_buffer = get_file_data(ctx.stacks_path);
  if (!ctx.json_root_buffer) {
    crun_audit_error("Unable to read stacks metadata file.");
    free((void *)ctx.stacks_path);
    return;
  }

  ctx.json_root = get_json_root(ctx.json_root_buffer);
  if (!ctx.json_root) {
    cleanup_runtime_context(&ctx);
    return;
  }

  ctx.languages_buffer = get_language_buffer(ctx.json_root);
  if (!ctx.languages_buffer || !strlen(ctx.languages_buffer)) {
    crun_audit_error("No languages available in stacks metadata.");
    cleanup_runtime_context(&ctx);
    return;
  }

  int user_choice = -1;

  // === Get User language menu choice ===
  get_user_choice(&user_choice,
                  ctx.languages_buffer,
                  languages_map_length);

  if (!user_choice) { // EXIT_SUCCESS
    crun_audit_info("User exited language selection.");
    cleanup_runtime_context(&ctx);
    return;
  }

  const char *selected_language_name = languages_map[user_choice - 1].name;

  // Get language projects
  ctx.packages_buffer = get_packages_buffer(ctx.json_root, selected_language_name);
  if (!ctx.packages_buffer || !strlen(ctx.packages_buffer)) {
    crun_audit_error("Unable to load package templates for language '%s'.", selected_language_name);
    cleanup_runtime_context(&ctx);
    return;
  }

  // === Get user package choice ===
  get_user_choice(&user_choice,
                  ctx.packages_buffer,
                  packages_map_length);

  if (!user_choice) { // EXIT_SUCCESS
    crun_audit_info("User exited template selection.");
    cleanup_runtime_context(&ctx);
    return;
  }

  crun_audit_info("Template selected: language='%s', template='%s'.",
                  selected_language_name,
                  packages_map[user_choice - 1].name);

  // See if the project exist first, otherwise download the pkg
  const char *crun_package_file_path = download_crun_package(&packages_map[user_choice - 1]);
  if (!crun_package_file_path) {
    crun_audit_error("Failed to retrieve package archive '%s.zip'.", packages_map[user_choice - 1].name);
    cleanup_runtime_context(&ctx);
    return;
  }

  // === Generate the project ===

  char user_current_directory[FILENAME_MAX] = {0};
  if (!__get_current_dir(user_current_directory, FILENAME_MAX)) {
    crun_audit_error("Unable to resolve current working directory.");
    cleanup_runtime_context(&ctx);
    return;
  }

  if (is_zip_archive(crun_package_file_path)) {
    ctx.workspace_path = build_hidden_extract_directory();
    if (!ctx.workspace_path || !strlen(ctx.workspace_path)) {
      crun_audit_error("Unable to prepare temporary extraction workspace.");
      cleanup_runtime_context(&ctx);
      return;
    }

    char init_script_path[FILENAME_MAX] = {0};
    if (extract_zip(crun_package_file_path, ctx.workspace_path, init_script_path, sizeof(init_script_path)) != EXIT_SUCCESS) {
      crun_audit_error("Failed to extract archive '%s'.", crun_package_file_path);
      cleanup_runtime_context(&ctx);
      return;
    }

    if (strlen(init_script_path)) {
      const int init_res = run_init_script(init_script_path);
      cleanup_init_scripts(init_script_path);

      if (init_res != EXIT_SUCCESS) {
        crun_audit_error("Init script execution failed: %s", init_script_path);
        cleanup_runtime_context(&ctx);
        return;
      }
    } else {
      crun_audit_info("No init script detected. Continuing with extracted output.");
    }

    if (copy_extract_output_to_directory(ctx.workspace_path, user_current_directory) != EXIT_SUCCESS) {
      crun_audit_error("Failed to copy extracted output into current directory.");
      cleanup_runtime_context(&ctx);
      return;
    }
  } else if (install_single_file_template(crun_package_file_path, user_current_directory) != EXIT_SUCCESS) {
    cleanup_runtime_context(&ctx);
    return;
  }

  crun_audit_info("Project scaffold ready.");
  INSTALLATION_COMPLETE_MSG;

  cleanup_runtime_context(&ctx);
}

const char *download_crun_package(const struct CrunPackage *crun_package_map) {
  if (!crun_package_map || !crun_package_map->name || !crun_package_map->url) {
    crun_audit_error("Invalid package metadata.");
    return NULL;
  }

  const char *package_name = crun_package_map->name,
             *package_url = crun_package_map->url,
             *crun_package_file_path = NULL;

  char package_extension[32] = ".zip";
  if (append_url_extension(package_url, package_extension, sizeof(package_extension)) != EXIT_SUCCESS)
    crun_audit_warn("Package URL has no detectable extension. Falling back to '.zip' cache suffix.");

  const int should_append_extension = !ends_with_ignore_case(package_name, package_extension);
  const char *effective_extension = should_append_extension ? package_extension : "";

  size_t crun_package_file_suffix_len =
      strlen(CRUN_DEFAULT_SUFFIX_DIRECTORY) +
      strlen(package_name) +
      strlen(effective_extension) + 1;

  char *crun_package_file_suffix = (char *)malloc(crun_package_file_suffix_len);

  if (!crun_package_file_suffix) {
    crun_audit_error("Memory allocation failed while preparing package cache path.");
    return NULL;
  }

  snprintf(crun_package_file_suffix,
           crun_package_file_suffix_len,
           "%s%s%s",
           CRUN_DEFAULT_SUFFIX_DIRECTORY,
           package_name,
           effective_extension);

  crun_package_file_path = get_file_home_path(crun_package_file_suffix);
  free(crun_package_file_suffix), crun_package_file_suffix = NULL; // Free `crun_package_file_suffix`
  if (!crun_package_file_path) {
    crun_audit_error("Unable to resolve package cache file path.");
    return NULL;
  }

  if (!is_file_exist(crun_package_file_path)) {
    crun_audit_warn("Template package not found in cache: %s", crun_package_file_path);
    crun_audit_info("Downloading template package '%s%s'...", package_name, effective_extension);
    const int download_res = download_file(package_url, crun_package_file_path);
    if (download_res != EXIT_SUCCESS) {
      crun_audit_error("Failed to download package '%s%s'.", package_name, effective_extension);
      free((void *)crun_package_file_path);
    }

    return download_res == EXIT_SUCCESS ? crun_package_file_path : NULL;
  }

  crun_audit_info("Using cached template package: %s", crun_package_file_path);
  return crun_package_file_path;
}

void get_user_choice(int *user_choice, const char *menu, const int limiter) {
  while (1) {
    PROJECT_MENU(menu);

    if (scanf("%d", user_choice) != 1) { // invalid input
      crun_audit_error("Invalid input. Please enter a numeric option.");
      *user_choice = -1;
      while (getchar() != '\n')
        ;

    } else if (*user_choice < 0 || *user_choice >= limiter) {
      crun_audit_warn("Invalid option. Select a number between 0 and %d.", limiter - 1);

    } else
      break;
  }
}

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
      return crun_audit_error("Memory allocation failed while growing language map."), NULL;

    languages_map = tmp;

    // Fill the `languages_map`
    languages_map[languages_map_length - 1].name = strdup(lang_name);
    if (!languages_map[languages_map_length - 1].name)
      return crun_audit_error("Memory allocation failed while storing language name."), NULL;

    // Allocate the `lang_line`
    lang_line = (char *)malloc((PROJECT_MENU_FIELD_LENGTH((int)languages_map_length, lang_name) + 1) * sizeof(char));

    if (!lang_line)
      return crun_audit_error("Memory allocation failed while building language menu."), NULL;

    // Fill the `lang_line`
    PROJECT_MENU_FIELD(lang_line, (int)languages_map_length++, lang_name);
    if (append_menu_line(&lang_buff, lang_line) != EXIT_SUCCESS) {
      free(lang_line), lang_line = NULL;
      crun_audit_error("Memory allocation failed while appending language menu line.");
      free(lang_buff), lang_buff = NULL;
      return NULL;
    }

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
      return crun_audit_error("Memory allocation failed while growing package map."), NULL;

    packages_map = tmp;

    // Fill the `packages_map`
    packages_map[packages_map_length - 1].name = strdup(name_str);
    packages_map[packages_map_length - 1].description = strdup(description_str);
    packages_map[packages_map_length - 1].url = strdup(url_str);
    if (!packages_map[packages_map_length - 1].name ||
        !packages_map[packages_map_length - 1].description ||
        !packages_map[packages_map_length - 1].url)
      return crun_audit_error("Memory allocation failed while storing package metadata."), NULL;

    // Allocate the `package_line`
    char *package_line = (char *)malloc((PROJECT_MENU_FIELD_LENGTH((int)packages_map_length, name_str) + 1) * sizeof(char));

    if (!package_line) {
      crun_audit_error("Memory allocation failed while building package menu.");
      return NULL;
    }

    // Fill the `package_line`
    PROJECT_MENU_FIELD(package_line, (int)packages_map_length++, name_str);
    if (append_menu_line(&packages_buffer, package_line) != EXIT_SUCCESS) {
      free(package_line), package_line = NULL;
      crun_audit_error("Memory allocation failed while appending package menu line.");
      free(packages_buffer), packages_buffer = NULL;
      return NULL;
    }

    // Cleaning
    free(package_line), package_line = NULL;
  }

  return packages_buffer;
}

static void free_languages_map_data() {
  if (languages_map && languages_map_length > 0) {
    for (size_t i = 0; i < languages_map_length - 1; ++i) {
      if (languages_map[i].name)
        free(languages_map[i].name), languages_map[i].name = NULL;
    }
  }

  if (languages_map)
    free(languages_map), languages_map = NULL;
  languages_map_length = 1;
}

static void free_packages_map_data() {
  if (packages_map && packages_map_length > 0) {
    for (size_t i = 0; i < packages_map_length - 1; ++i) {
      if (packages_map[i].name)
        free(packages_map[i].name), packages_map[i].name = NULL;
      if (packages_map[i].description)
        free(packages_map[i].description), packages_map[i].description = NULL;
      if (packages_map[i].url)
        free(packages_map[i].url), packages_map[i].url = NULL;
    }
  }

  // Free `packages_map`
  if (packages_map)
    free(packages_map), packages_map = NULL;
  packages_map_length = 1;
}

void free_all(void **ptrs, size_t len) {
  free_languages_map_data();
  free_packages_map_data();

  // Free Other ptr(s)
  for (size_t i = 0; i < len; ++i)
    if (ptrs[i])
      free(ptrs[i]), ptrs[i] = NULL;
}
