/**
 * @file      crun.h
 * @author    @ZouariOmar (zouariomar20@gmail.com)
 * @brief     crun header file
 * @version   0.2
 * @date      2025-11-26
 * @copyright Copyright (c) 2025
 * @link https://github.com/ZouariOmar/crun/project/inc/crun.h crun.h @endlink
 */

#ifndef __CRUN_H__
#define __CRUN_H__

// ################################
// ### HEADERS DECLARATION PART ###
// ################################
// Include std header(s)
#include <cjson/cJSON.h>
#include <stdio.h>

struct CrunLanguage {
  char *name;
};

struct CrunPackage {
  char *name, *description, *url;
};

// Shared Vars
extern struct CrunLanguage *languages_map;
extern size_t languages_map_length;
extern struct CrunPackage *packages_map;
extern size_t packages_map_length;

// #################################
// ### FUNCTION DECLARATION PART ###
// #################################
void crun();
void crun_stacks_json_checker(const char *);
void get_user_choice(int *, const char *, const int);
char *get_language_buffer(cJSON *);
char *get_packages_buffer(cJSON *, const char *);
void free_all(void **, const size_t);
// int generate();
// void notify();

#endif // __CRUN_H__
