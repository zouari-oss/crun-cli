/**
 * @file      crun_stacks_file_manager.h
 * @author    @ZouariOmar (zouariomar20@gmail.com)
 * @brief     crun_stacks_file_manager header file
 * @version   0.2
 * @date      2025-11-26
 * @copyright Copyright (c) 2025
 * @link https://github.com/ZouariOmar/crun/project/inc/crun_stacks_file_manager.h crun_stacks_file_manager.h @endlink
 */

#ifndef __CRUN_STACKS_FILE_MANAGER_H__
#define __CRUN_STACKS_FILE_MANAGER_H__

// ################################
// ### HEADERS DECLARATION PART ###
// ################################
// Include stander C header(s)
#include <stdio.h>

// Include json header(s)
#include <cjson/cJSON.h>

// Shared Vars
extern const char *crun_stacks_json_file_path;

// ##################################
// ### FUNCTIONS DECLARATION PART ###
// ##################################
cJSON *get_json_root(const char *);
cJSON *get_json_object(cJSON *root, const char *obj);

#endif // __CRUN_STACKS_FILE_MANAGER_H__
