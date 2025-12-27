/**
 * @file      crun_json_manager.c
 * @author    @ZouariOmar <zouariomar20@gmail.com>
 * @brief     crun_json_manager source file
 * @version   0.1
 * @date      2025-12-25
 * @copyright Copyright (c) 2025
 * @link https://github.com/ZouariOmar/crun/project/src/crun_json_manager.c crun_json_manager.c @endlink
 */

// ########################
// ### INC HEADERS PART ###
// ########################

// Include curl header(s)
#include <cjson/cJSON.h>
#include <curl/curl.h>

// Include std header(s)
#include <stdio.h>
#include <string.h>

// Include custom header(s)
#include "../common/patterns.h"
#include "../inc/crun_json_manager.h"

// ##########################
// ### FUNCTIONS DEV PART ###
// ##########################

cJSON *get_json_root(const char *json_bufferer) {
  cJSON *json = cJSON_Parse(json_bufferer);

  if (!json) {
    fprintf(stderr, "[ERROR] crun_json_manager.c :: Can't Parse the json file!\n");
    return cJSON_Delete(json), json = NULL, NULL;
  }

  const char *error_ptr = cJSON_GetErrorPtr();
  if (error_ptr) {
    fprintf(stderr, "[ERROR] crun_json_manager.c :: CJSON ERROR PTR: %s\n", error_ptr);
    return cJSON_Delete(json), json = NULL, NULL;
  }

  return json;
}

cJSON *get_json_object(cJSON *root, const char *obj_name) {
  cJSON *object = cJSON_GetObjectItemCaseSensitive(root, obj_name);

  if (!cJSON_IsObject(object)) {
    fprintf(stderr, "[ERROR] crun_json_manager.c :: Invalid or missing object (%s)\n", obj_name);
    return NULL;
  }

  return object;
}
