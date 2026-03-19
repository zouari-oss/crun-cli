/**
 * @file      crun_json_manager.c
 * @author    ZouariOmar <zouariomar20@gmail.com>
 * @brief     crun_json_manager source file
 * @version   0.1
 * @date      2025-12-25
 * @copyright Copyright (c) 2025
 *
 * <a href="https://github.com/zouari-oss/crun-cli/project/src/crun_json_manager.c">crun_json_manager.c</a>
 */

#include "../inc/crun_json_manager.h"
#include "../inc/crun_audit.h"
#include <cjson/cJSON.h>
#include <string.h>

cJSON *get_json_root(const char *json_bufferer) {
  if (!json_bufferer || !strlen(json_bufferer)) {
    crun_audit_error("JSON input buffer is empty.");
    return NULL;
  }

  cJSON *json = cJSON_Parse(json_bufferer);

  if (!json) {
    crun_audit_error("Unable to parse JSON metadata.");
    return NULL;
  }

  const char *error_ptr = cJSON_GetErrorPtr();
  if (error_ptr) {
    crun_audit_error("JSON parse error near: %s", error_ptr);
    return cJSON_Delete(json), json = NULL, NULL;
  }

  return json;
}

cJSON *get_json_object(cJSON *root, const char *obj_name) {
  if (!root || !obj_name || !strlen(obj_name)) {
    crun_audit_error("Invalid JSON object request.");
    return NULL;
  }

  cJSON *object = cJSON_GetObjectItemCaseSensitive(root, obj_name);

  if (!cJSON_IsObject(object)) {
    crun_audit_error("Invalid or missing JSON object: %s", obj_name);
    return NULL;
  }

  return object;
}
