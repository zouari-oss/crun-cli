/**
 * @file      crun.h
 * @author    @ZouariOmar (zouariomar20@gmail.com)
 * @brief     crun header file
 * @version   0.2
 * @date      2025-11-26
 * @copyright Copyright (c) 2025
 * @link https://github.com/ZouariOmar/crun/project/src/crun.h crun.h @endlink
 */

#ifndef __CRUN_H__
#define __CRUN_H__

// ################################
// ### HEADERS DECLARATION PART ###
// ################################
// Include std header(s)
#include <stdio.h>

// Shared Vars
extern char **lang_map;
extern int stacks_size;
extern size_t languages_number;

// #################################
// ### FUNCTION DECLARATION PART ###
// #################################
void __init__();
void get_user_choice(int *, const char *, const int);
void free_all(void **, const size_t);
void free_language_map(size_t len);
int generate();
void notify();

#endif // __CRUN_H__
