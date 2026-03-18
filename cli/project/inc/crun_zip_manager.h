/**
 * @file      crun_zip_manager.h
 * @author    @ZouariOmar <zouariomar20@gmail.com>
 * @brief     crun_zip_manager header file
 * @version   0.1
 * @date      2025-12-31
 * @copyright Copyright (c) 2025
 * @link      https://github.com/ZouariOmar/crun/project/inc/crun_zip_manager.h crun_zip_manager.h @endlink
 */

#ifndef __CRUN_ZIP_MANAGER_H__
#define __CRUN_ZIP_MANAGER_H__

// ################################
// ### HEADERS DECLARATION PART ###
// ################################
// Include stander header(s)
#ifdef _WIN32
#include <direct.h>
#define MKDIR(path) _mkdir(path)
#define PATH_SEP '\\'
#else
#include <unistd.h>
#define MKDIR(path) mkdir(path, 0755)
#define PATH_SEP '/'
#endif

// ##################################
// ### FUNCTIONS DECLARATION PART ###
// ##################################
static void ensure_dir(const char *);
static int is_safe_path(const char *);
int extract_zip(const char *, const char *);

#endif // __CRUN_ZIP_MANAGER_H__
