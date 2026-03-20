/**
 * @file      main.c
 * @author    ZouariOmar (zouariomar20@gmail.com)
 * @brief     Main source file
 * @version   0.2
 * @date      2025-11-26
 * @copyright Copyright (c) 2025
 *
 * <a href="https://github.com/zouari-oss/crun-cli/project/src/main.c">main.c</a>
 */

#include "../inc/crun.h"
#include <stdlib.h>
#include <string.h>

/**
 * @fn         main(int, const char **)
 * @brief      CLI entry point with flag dispatch.
 * @param argc Argument count
 * @param argv Argument vector
 * @return     int EXIT_SUCCESS/EXIT_FAILURE.
 */
int main(int argc, const char **argv) {
  if (argc == 1)
    return crun(), EXIT_SUCCESS;

  if (!strcmp(argv[1], "--version"))
    return crun_version(), EXIT_SUCCESS;

  if (!strcmp(argv[1], "--help") || !strcmp(argv[1], "-h"))
    return crun_help(), EXIT_SUCCESS;

  if (!strcmp(argv[1], "--update"))
    return crun_update();

  fprintf(stderr, "[ERROR] Unknown flag: %s\n", argv[1]);
  crun_help();
  return EXIT_FAILURE;
}
