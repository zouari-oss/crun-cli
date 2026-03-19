/**
 * @file      crun_audit.h
 * @author    @ZouariOmar (zouariomar20@gmail.com)
 * @brief     Audit logging API for CLI/file outputs.
 * @version   0.2
 * @date      2026-03-18
 * @copyright Copyright (c) 2025
 */

#ifndef __CRUN_AUDIT_H__
#define __CRUN_AUDIT_H__

/**
 * @brief Emit an informational audit event.
 *
 * @param fmt printf-style message format.
 * @param ... Format arguments.
 */
void crun_audit_info(const char *fmt, ...);

/**
 * @brief Emit a warning audit event.
 *
 * @param fmt printf-style message format.
 * @param ... Format arguments.
 */
void crun_audit_warn(const char *fmt, ...);

/**
 * @brief Emit an error audit event.
 *
 * @param fmt printf-style message format.
 * @param ... Format arguments.
 */
void crun_audit_error(const char *fmt, ...);

#endif // __CRUN_AUDIT_H__
