/**
 * @file      crun_file_manager.h
 * @author    @ZouariOmar (zouariomar20@gmail.com)
 * @brief     crun_file_manager header file
 * @version   0.2
 * @date      2025-11-26
 * @copyright Copyright (c) 2025
 * @link https://github.com/ZouariOmar/crun/project/inc/crun_file_manager.h crun_file_manager.h @endlink
 */

#ifndef __CRUN_FILE_MANAGER_H__
#define __CRUN_FILE_MANAGER_H__

// ################################
// ### HEADERS DECLARATION PART ###
// ################################

// Include std header(s)
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

// #################################
// ### FUNCTION DECLARATION PART ###
// #################################
char *get_file_data(const char *);
int is_file_exist(const char *);
const long get_file_size(FILE *);
const char *get_file_home_path(const char *suffix);

#endif // __CRUN_FILE_MANAGER_H__
