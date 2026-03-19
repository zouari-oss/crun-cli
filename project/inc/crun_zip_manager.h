/**
 * @file      crun_zip_manager.h
 * @author    ZouariOmar <zouariomar20@gmail.com>
 * @brief     ZIP extraction helpers
 * @version   0.2
 * @date      2026-03-18
 * @copyright Copyright (c) 2025
 *
 * <a href="https://github.com/zouari-oss/crun-cli/project/inc/crun_zip_manager.h">crun_zip_manager.h</a>
 */

#ifndef __CRUN_ZIP_MANAGER_H__
#define __CRUN_ZIP_MANAGER_H__

// #########################################
// ### HEADERS & MACROS DECLARATION PART ###
// #########################################

#include <stddef.h>

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

/**
 * @brief Extract ZIP content safely into output directory.
 *
 * Detects init script path (`__init__.sh` or `__init__.bat`) while extracting.
 *
 * @param zip_path Source zip file path.
 * @param out_dir Destination extraction directory.
 * @param init_script_path Optional output buffer to receive detected init script path.
 * @param init_script_path_size Size of @p init_script_path buffer.
 * @return int EXIT_SUCCESS on success, EXIT_FAILURE otherwise.
 */
int extract_zip(const char *, const char *, char *, size_t);

#endif // __CRUN_ZIP_MANAGER_H__
