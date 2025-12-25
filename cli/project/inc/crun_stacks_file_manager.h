/**
 * @file      crun_stacks_file_manager.h
 * @author    @ZouariOmar (zouariomar20@gmail.com)
 * @brief     crun_stacks_file_manager header file
 * @version   0.2
 * @date      2025-11-26
 * @copyright Copyright (c) 2025
 * @link https://github.com/ZouariOmar/crun/project/src/crun_stacks_file_manager.h crun_stacks_file_manager.h @endlink
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
extern struct CrunStackProjectStruct **crun_project_stacks;

struct CrunStackProjectStruct {
  char *name, *description, *url;
}; // CrunStackProjectStruct

// #################################
// ### FUNCTION DECLARATION PART ###
// #################################
void crun_stacks_manager_init();
char *read_crun_stacks_json_file();
int is_file_exist(const char *);

// ##########################################
// ### GETTERS FUNCTIONS DECLARATION PART ###
// ##########################################
const char *get_file_path();
long get_file_size(FILE *);
char *get_language_buffer(char ***, size_t *, cJSON *);
char *get_projects_buffer(const char *, cJSON *);
cJSON *get_root_json(char *);
cJSON *get_stacks_object(cJSON *);

// ##########################################
// ### LIBCURL FUNCTIONS DECLARATION PART ###
// ##########################################
size_t write_data(void *, size_t, size_t, void *);
int download_file(const char *, const char *);

#endif // __CRUN_STACKS_FILE_MANAGER_H__
