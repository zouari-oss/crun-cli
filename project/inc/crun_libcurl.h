/**
 * @file      crun_libcurl.h
 * @author    @ZouariOmar (zouariomar20@gmail.com)
 * @brief     Network download helpers built on top of libcurl.
 * @version   0.3
 * @date      2026-03-18
 * @copyright Copyright (c) 2025
 * @link https://github.com/ZouariOmar/crun/project/inc/crun_libcurl.h crun_libcurl.h @endlink
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

/**
 * @brief libcurl write callback used when storing response data to a file.
 *
 * @param ptr Pointer to downloaded bytes.
 * @param size Size of each element.
 * @param nmemb Number of elements.
 * @param stream Destination stream.
 * @return size_t Number of bytes written.
 */
size_t write_data(void *, size_t, size_t, void *);

/**
 * @brief Download a file from URL to a destination path.
 *
 * @param url Remote URL.
 * @param out Destination file path.
 * @return int EXIT_SUCCESS on success, EXIT_FAILURE otherwise.
 */
int download_file(const char *, const char *);

#endif // __CRUN_LIBCURL_H__
