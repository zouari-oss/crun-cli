/**
 * @file      crun_file_manager.h
 * @author    ZouariOmar (zouariomar20@gmail.com)
 * @brief     File/path utilities for crun runtime data.
 * @version   0.3
 * @date      2026-03-18
 * @copyright Copyright (c) 2025
 *
 * <a href="https://github.com/zouari-oss/crun-cli/project/inc/crun_file_manager.h">crun_file_manager.h</a>
 */

#ifndef __CRUN_FILE_MANAGER_H__
#define __CRUN_FILE_MANAGER_H__

// #########################################
// ### HEADERS & MACROS DECLARATION PART ###
// #########################################

#include <stdio.h>

#ifdef _WIN32
#include <io.h>
#define OS_HOME getenv("APPDATA")
#define F_OK 0
#define access _access
#define CRUN_DEFAULT_SUFFIX_DIRECTORY "\\crun\\"

#else
#include <unistd.h>
#define OS_HOME getenv("HOME")
#define CRUN_DEFAULT_SUFFIX_DIRECTORY "/.local/share/crun/"
#endif

#define CRUN_STACKS_JSON_FILE_URL "https://raw.githubusercontent.com/ZouariOmar/cpkg/refs/heads/main/crun/crun_stacks.json"
#define CRUN_STACKS_JSON_FILE_DEFAULT_SUFFIX CRUN_DEFAULT_SUFFIX_DIRECTORY "crun_stacks.json"
#define CRUN_TMP_DIRECTORY_SUFFIX CRUN_DEFAULT_SUFFIX_DIRECTORY "tmp"

// ##################################
// ### FUNCTIONS DECLARATION PART ###
// ##################################

/**
 * @brief Read full file content into a null-terminated buffer.
 *
 * @param filename Absolute/relative file path.
 * @return char* Allocated buffer (caller frees), or NULL on failure.
 */
char *get_file_data(const char *);

/**
 * @brief Check if file or directory exists.
 *
 * @param filename Path to check.
 * @return int Non-zero if path exists, zero otherwise.
 */
int is_file_exist(const char *);

/**
 * @brief Compute total file size for opened stream.
 *
 * @param fp Opened file pointer.
 * @return long File size in bytes, -1 on failure.
 */
const long get_file_size(FILE *);

/**
 * @brief Build home-based absolute path using suffix.
 *
 * @param suffix Path suffix appended to OS home location.
 * @return const char* Allocated path buffer (caller frees), or NULL on failure.
 */
const char *get_file_home_path(const char *);

/**
 * @brief Detect whether a file is a ZIP archive using its signature.
 *
 * @param filename Path to the file to inspect.
 * @return int Non-zero if ZIP signature is detected, zero otherwise.
 */
int is_zip_archive(const char *);

/**
 * @brief Copy a file from source path to destination path.
 *
 * @param source_path Existing file path.
 * @param target_path Destination file path.
 * @return int EXIT_SUCCESS on success, EXIT_FAILURE otherwise.
 */
int copy_file_to_path(const char *, const char *);

#endif // __CRUN_FILE_MANAGER_H__
