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

#define CRUN_IO_BUFFER_SIZE 8192

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

int is_zip_archive(const char *filename) {
  if (!filename || !strlen(filename))
    return 0;

  FILE *fp = fopen(filename, "rb");
  if (!fp) {
    crun_audit_error("Unable to open file for archive detection: %s", filename);
    return 0;
  }

  unsigned char signature[4] = {0};
  const size_t read_len = fread(signature, 1, sizeof(signature), fp);
  fclose(fp), fp = NULL;

  if (read_len < 4)
    return 0;

  const int is_local_header = signature[0] == 'P' && signature[1] == 'K' &&
                              signature[2] == 0x03 && signature[3] == 0x04;
  const int is_empty_archive = signature[0] == 'P' && signature[1] == 'K' &&
                               signature[2] == 0x05 && signature[3] == 0x06;
  const int is_spanned_archive = signature[0] == 'P' && signature[1] == 'K' &&
                                 signature[2] == 0x07 && signature[3] == 0x08;

  return is_local_header || is_empty_archive || is_spanned_archive;
}

int copy_file_to_path(const char *source_path, const char *target_path) {
  if (!source_path || !target_path || !strlen(source_path) || !strlen(target_path)) {
    crun_audit_error("Invalid source/target path for file copy.");
    return EXIT_FAILURE;
  }

  FILE *source_fp = fopen(source_path, "rb");
  if (!source_fp) {
    crun_audit_error("Unable to open source file for copy: %s", source_path);
    return EXIT_FAILURE;
  }

  FILE *target_fp = fopen(target_path, "wb");
  if (!target_fp) {
    fclose(source_fp), source_fp = NULL;
    crun_audit_error("Unable to open destination file for copy: %s", target_path);
    return EXIT_FAILURE;
  }

  char buffer[CRUN_IO_BUFFER_SIZE];
  size_t read_len = 0;
  int copy_res = EXIT_SUCCESS;

  while ((read_len = fread(buffer, 1, sizeof(buffer), source_fp)) > 0) {
    if (fwrite(buffer, 1, read_len, target_fp) != read_len) {
      crun_audit_error("I/O error while writing file: %s", target_path);
      copy_res = EXIT_FAILURE;
      break;
    }
  }

  if (ferror(source_fp)) {
    crun_audit_error("I/O error while reading file: %s", source_path);
    copy_res = EXIT_FAILURE;
  }

  fclose(source_fp), source_fp = NULL;
  fclose(target_fp), target_fp = NULL;
  return copy_res;
}
