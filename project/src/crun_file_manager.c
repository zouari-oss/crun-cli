/**
 * @file      crun_file_manager.c
 * @author    ZouariOmar <zouariomar20@gmail.com>
 * @brief     crun_file_manager source file
 * @version   0.1
 * @date      2025-12-26
 * @copyright Copyright (c) 2025
 *
 * <a href="https://github.com/zouari-oss/crun-cli/project/src/crun_file_manager.c">crun_file_manager.c</a>
 */

#include "../inc/crun_file_manager.h"
#include "../inc/crun_audit.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *get_file_data(const char *filename) {
  if (!filename || !strlen(filename))
    return NULL;

  FILE *fp = fopen(filename, "rb");
  if (!fp) {
    crun_audit_error("Unable to open file: %s", filename);
    return NULL;
  }

  const long file_size = get_file_size(fp);
  if (file_size < 0) {
    fclose(fp), fp = NULL;
    crun_audit_error("Unable to determine size for file: %s", filename);
    return NULL;
  }

  char *buffer = (char *)malloc((file_size * sizeof(char)) + 1);
  if (!buffer) {
    crun_audit_error("Memory allocation failed while reading file: %s", filename);
    return fclose(fp), fp = NULL, NULL;
  }

  size_t len = fread(buffer, 1, file_size, fp);
  if (ferror(fp)) {
    free(buffer), buffer = NULL;
    fclose(fp), fp = NULL;
    crun_audit_error("I/O error while reading file: %s", filename);
    return NULL;
  }

  buffer[len] = '\0';

  return fclose(fp), fp = NULL, buffer;
}

int is_file_exist(const char *filename) {
  return !access(filename, F_OK);
}

const char *get_file_home_path(const char *suffix) {
  const char *home = OS_HOME;
  if (!home)
    return NULL;

  size_t len = strlen(home) + strlen(suffix) + 1;
  char *path = (char *)malloc(len * sizeof(char));
  if (!path)
    return NULL;

  snprintf(path, len, "%s%s", home, suffix);
  return path;
}

const long get_file_size(FILE *fp) {
  if (!fp)
    return -1;

  fseek(fp, 0, SEEK_END);
  long file_size = ftell(fp);
  rewind(fp);
  return file_size;
}
