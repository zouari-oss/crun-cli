/**
 * @file      crun_audit.c
 * @author    ZouariOmar (zouariomar20@gmail.com)
 * @brief     crun audit/logging helpers implementation
 * @version   0.2
 * @date      2025-11-26
 * @copyright Copyright (c) 2025
 *
 * <a href="https://github.com/zouari-oss/crun-cli/project/src/crun_audit.c">crun_audit.c</a>
 */

#include "../inc/crun_audit.h"
#include "../inc/crun_file_manager.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static const char *get_audit_file_path() {
  static char *cached_audit_file_path = NULL;
  static int initialized = 0;

  if (initialized)
    return cached_audit_file_path;

  initialized = 1;

  const char *audit_dir = get_file_home_path(CRUN_DEFAULT_SUFFIX_DIRECTORY);
  if (!audit_dir)
    return NULL;

  if (!is_file_exist(audit_dir))
    __mkdir(audit_dir);

  const char *audit_file = get_file_home_path(CRUN_DEFAULT_SUFFIX_DIRECTORY "audit.log");
  free((void *)audit_dir);
  if (!audit_file)
    return NULL;

  cached_audit_file_path = strdup(audit_file);
  free((void *)audit_file);
  return cached_audit_file_path;
}

static void crun_audit_log(const char *level, FILE *stream, const char *fmt, va_list args) {
  char msg_buffer[1024];
  vsnprintf(msg_buffer, sizeof(msg_buffer), fmt, args);

  time_t now = time(NULL);
  struct tm *tm_info = localtime(&now);
  char timestamp[32];
  strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);

  fprintf(stream, "[%s] [%s] %s\n", timestamp, level, msg_buffer);

  const char *audit_file = get_audit_file_path();
  if (!audit_file)
    return;

  FILE *fp = fopen(audit_file, "a");
  if (fp) {
    fprintf(fp, "[%s] [%s] %s\n", timestamp, level, msg_buffer);
    fclose(fp);
  }
}

void crun_audit_info(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  crun_audit_log("INFO", stdout, fmt, args);
  va_end(args);
}

void crun_audit_warn(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  crun_audit_log("WARN", stderr, fmt, args);
  va_end(args);
}

void crun_audit_error(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  crun_audit_log("ERROR", stderr, fmt, args);
  va_end(args);
}
