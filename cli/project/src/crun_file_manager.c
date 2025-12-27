/**
 * @file      crun_file_manager.c
 * @author    @ZouariOmar <zouariomar20@gmail.com>
 * @brief     crun_file_manager source file
 * @version   0.1
 * @date      2025-12-26
 * @copyright Copyright (c) 2025
 * @link      https://github.com/ZouariOmar/crun/project/src/crun_file_manager.c crun_file_manager.c @endlink
 */

// ########################
// ### INC HEADERS PART ###
// ########################

// Include std header(s)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include custom header(s)
#include "../inc/crun_file_manager.h"

// ##########################
// ### FUNCTIONS DEV PART ###
// ##########################

char *get_file_data(const char *filename) {
  FILE *fp = fopen(filename, "r");
  if (!fp) {
    fprintf(stderr, "[ERROR] crun_file_manager.c :: Unable to open '%s' file!\n", filename);
    return NULL;
  }

  const long file_size = get_file_size(fp);

  char *buffer = (char *)malloc((file_size * sizeof(char)) + 1);
  if (!buffer) {
    fprintf(stderr, "[ERROR] crun_file_manager.c :: Can't Allocate the `buffer`!");
    return fclose(fp), fp = NULL, NULL;
  }

  size_t len = fread(buffer, 1, file_size, fp);
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
  fseek(fp, 0, SEEK_END);
  long file_size = ftell(fp);
  rewind(fp);
  return file_size;
}
