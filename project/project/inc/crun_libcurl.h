/**
 * @file      crun_libcrul.h
 * @author    @ZouariOmar (zouariomar20@gmail.com)
 * @brief     crun_libcrul header file
 * @version   0.2
 * @date      2025-11-26
 * @copyright Copyright (c) 2025
 * @link https://github.com/ZouariOmar/crun/project/inc/crun_libcrul.h crun_libcrul.h @endlink
 */

#ifndef __CRUN_LIBCURL_H__
#define __CRUN_LIBCURL_H__
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

// ################################
// ### HEADERS DECLARATION PART ###
// ################################
// Include stander C header(s)
#include <stdio.h>

// ##########################################
// ### LIBCURL FUNCTIONS DECLARATION PART ###
// ##########################################
size_t write_data(void *, size_t, size_t, void *);
int download_file(const char *, const char *);

#endif // __CRUN_LIBCURL_H__
