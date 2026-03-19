/**
 * @file      crun.h
 * @author    ZouariOmar (zouariomar20@gmail.com)
 * @brief     Core crun CLI interfaces.
 * @version   0.3
 * @date      2026-03-18
 * @copyright Copyright (c) 2025
 *
 * <a href="https://github.com/zouari-oss/crun-cli/project/inc/crun.h">crun.h</a>
 */

#ifndef __CRUN_H__
#define __CRUN_H__

// #########################################
// ### HEADERS & MACROS DECLARATION PART ###
// #########################################

#include <cjson/cJSON.h>
#include <stdio.h>

#ifdef _WIN32
#include <direct.h>
#include <process.h>
#define __get_current_dir _getcwd
#define __get_pid _getpid

#else
#include <unistd.h>
#define __get_current_dir getcwd
#define __get_pid getpid
#endif

// ################################
// ### STRUCTS DECLARATION PART ###
// ################################

struct CrunLanguage {
  char *name;
}; // CrunLanguage Structure

struct CrunPackage {
  char *name, *description, *url;
}; // CrunPackage Structure

struct CrunRuntimeContext {
  char *languages_buffer;
  char *packages_buffer;
  const char *json_root_buffer;
  const char *stacks_path;
  const char *workspace_path;
  cJSON *json_root;
}; // CrunRuntimeContext Structure

// ##################################
// ### FUNCTIONS DECLARATION PART ###
// ##################################

/**
 * @brief Main CLI flow entrypoint.
 */
void crun();

/**
 * @brief Print command-line usage/help.
 */
void crun_help();

/**
 * @brief Refresh local stacks metadata from remote source.
 *
 * @return int EXIT_SUCCESS on success, EXIT_FAILURE otherwise.
 */
int crun_update();

/**
 * @brief Ensure stacks metadata file exists locally.
 *
 * @param crun_stacks_json_file_path Absolute path to local metadata file.
 */
void crun_stacks_json_checker(const char *);

/**
 * @brief Prompt for menu choice with validation.
 *
 * @param user_choice Output selected value.
 * @param menu Rendered menu text.
 * @param limiter Upper bound (exclusive), usually map length.
 */
void get_user_choice(int *, const char *, const int);

/**
 * @brief Resolve package archive from cache or download it.
 *
 * @param crun_package_map Selected package metadata.
 * @return const char* Allocated absolute path to package archive, or NULL on failure.
 */
const char *download_crun_package(const struct CrunPackage *);

/**
 * @brief Build selectable language menu buffer from JSON root.
 *
 * @param root Parsed JSON root.
 * @return char* Allocated menu text buffer, or NULL on failure.
 */
char *get_language_buffer(cJSON *);

/**
 * @brief Build selectable package menu buffer for one language.
 *
 * @param root Parsed JSON root.
 * @param language_name Selected language key.
 * @return char* Allocated menu text buffer, or NULL on failure.
 */
char *get_packages_buffer(cJSON *, const char *);

/**
 * @brief Release language/package maps and additional owned pointers.
 *
 * @param ptrs Extra pointers to free.
 * @param len Number of pointers in ptrs.
 */
void free_all(void **, const size_t);

#endif // __CRUN_H__
