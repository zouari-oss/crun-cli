/**
 * @file      crun.c
 * @author    @ZouariOmar (zouariomar20@gmail.com)
 * @brief     crun source file
 * @version   0.2
 * @date      2025-11-26
 * @copyright Copyright (c) 2025
 * @link https://github.com/ZouariOmar/crun/project/src/crun.c crun.c @endlink
 */

// ########################
// ### INC HEADERS PART ###
// ########################

// Include std c header(s)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include json header(s)
#include <cjson/cJSON.h>
#include <unistd.h>

// Include custom header(s)
#include "../common/patterns.h"
#include "../inc/crun.h"
#include "../inc/crun_stacks_file_manager.h"

char **lang_map = NULL;
size_t languages_number;
struct CrunStackProjectStruct **crun_project_stacks = NULL;

// #########################
// ### FUNCTION DEV PART ###
// #########################

void free_all(void **ptrs, size_t len) {
  if (!lang_map)
    return;

  for (size_t i = 0; i < len; ++i)
    free(lang_map[i]);
  free(lang_map);

  for (size_t i = 0; i < len; ++i) {
    free(ptrs[i]);
    ptrs[i] = NULL;
  }
}

void __init__() {
  CRUN_HEADER;
  CRUN_BANNER;

  crun_stacks_manager_init();
  cJSON *root = get_root_json(read_crun_stacks_json_file());

  char *language_buffer = get_language_buffer(&lang_map, &languages_number, root);
  if (!language_buffer || !strlen(language_buffer)) {
    fprintf(stderr, "[ERROR] `language_buffer` is null/empty!");
    free_all((void *[]){language_buffer, (void *)crun_stacks_json_file_path}, 2);
    return;
  }

  int user_choice = -1;
  get_user_choice(&user_choice,
                  language_buffer,
                  languages_number); // Get User language menu choice

  char *projects_buffer = get_projects_buffer(lang_map[user_choice - 1], root);
  if (!projects_buffer || !strlen(projects_buffer)) {
    fprintf(stderr, "[ERROR] `language_buffer` is null/empty!");
    free_all((void *[]){language_buffer, (void *)crun_stacks_json_file_path}, 2);
    cJSON_Delete(root);
    root = NULL;
    return;
  }

  get_user_choice(&user_choice,
                  projects_buffer,
                  6); // Get User project menu choice

  free_all((void *[]){language_buffer, (void *)crun_stacks_json_file_path}, 2);
  cJSON_Delete(root);
  root = NULL;
}

void get_user_choice(int *user_choice, const char *menu, const int limiter) {
  do {
    PROJECT_MENU(menu); // Display language menu

    if (scanf("%d", user_choice) != 1) { // invalid input
      INVALID_OPTION_MSG;
      *user_choice = -1;
      while (getchar() != '\n') // clear input buffer
        ;
    }
  } while (user_choice < 0 || *user_choice >= limiter);
}

int generate() {
  return EXIT_SUCCESS;
}

void notify() {
}
