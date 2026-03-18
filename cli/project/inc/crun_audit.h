/**
 * @file      crun_audit.h
 * @author    @ZouariOmar
 * @brief     crun audit/logging helpers
 */

#ifndef __CRUN_AUDIT_H__
#define __CRUN_AUDIT_H__

void crun_audit_info(const char *fmt, ...);
void crun_audit_warn(const char *fmt, ...);
void crun_audit_error(const char *fmt, ...);

#endif // __CRUN_AUDIT_H__
