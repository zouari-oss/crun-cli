/**
 * @file      crun_audit.c
 * @author    @ZouariOmar
 * @brief     crun audit/logging helpers
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef _WIN32
#include <direct.h>
#define __mkdir(path) _mkdir(path)
#else
#include <sys/stat.h>
#define __mkdir(path) mkdir(path, 0755)
#endif

#include "../inc/crun_audit.h"
#include "../inc/crun_file_manager.h"

static void crun_audit_log(const char *level, FILE *stream, const char *fmt, va_list args) {
  char msg_buffer[1024];
  vsnprintf(msg_buffer, sizeof(msg_buffer), fmt, args);

  time_t now = time(NULL);
  struct tm *tm_info = localtime(&now);
  char timestamp[32];
  strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);

  fprintf(stream, "[%s] [%s] %s\n", timestamp, level, msg_buffer);

  const char *audit_dir = get_file_home_path(CRUN_DEFAULT_SUFFIX_DIRECTORY);
  if (!audit_dir)
    return;

  if (!is_file_exist(audit_dir))
    __mkdir(audit_dir);

  const char *audit_file = get_file_home_path(CRUN_DEFAULT_SUFFIX_DIRECTORY "audit.log");
  if (!audit_file) {
    free((void *)audit_dir);
    return;
  }

  FILE *fp = fopen(audit_file, "a");
  if (fp) {
    fprintf(fp, "[%s] [%s] %s\n", timestamp, level, msg_buffer);
    fclose(fp);
  }

  free((void *)audit_file);
  free((void *)audit_dir);
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
