/**
 * @file      crun_audit.h
 * @author    ZouariOmar (zouariomar20@gmail.com)
 * @brief     Audit logging API for CLI/file outputs.
 * @version   0.2
 * @date      2026-03-18
 * @copyright Copyright (c) 2025
 *
 * <a href="https://github.com/zouari-oss/crun-cli/project/inc/crun_audit.h">crun_audit.h</a>
 */

#ifndef __CRUN_AUDIT_H__
#define __CRUN_AUDIT_H__

// #########################################
// ### HEADERS & MACROS DECLARATION PART ###
// #########################################

#ifdef _WIN32
#include <direct.h>
#define __mkdir(path) _mkdir(path)

#else
#include <sys/stat.h>
#define __mkdir(path) mkdir(path, 0755)
#endif

// ##################################
// ### FUNCTIONS DECLARATION PART ###
// ##################################

/**
 * @brief Emit an informational audit event.
 *
 * @param fmt printf-style message format.
 * @param ... Format arguments.
 */
void crun_audit_info(const char *, ...);

/**
 * @brief Emit a warning audit event.
 *
 * @param fmt printf-style message format.
 * @param ... Format arguments.
 */
void crun_audit_warn(const char *, ...);

/**
 * @brief Emit an error audit event.
 *
 * @param fmt printf-style message format.
 * @param ... Format arguments.
 */
void crun_audit_error(const char *, ...);

#endif // __CRUN_AUDIT_H__
