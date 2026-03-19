/**
 * @file      crun_json_manager.h
 * @author    ZouariOmar (zouariomar20@gmail.com)
 * @brief     JSON parsing helpers for stacks metadata.
 * @version   0.3
 * @date      2026-03-18
 * @copyright Copyright (c) 2025
 *
 * <a href="https://github.com/ZouariOmar/crun/project/inc/crun_json_manager.h">crun_json_manager.h</a>
 */

#ifndef __CRUN_JSON_MANAGER_H__
#define __CRUN_JSON_MANAGER_H__

// #########################################
// ### HEADERS & MACROS DECLARATION PART ###
// #########################################

#include <cjson/cJSON.h>
#include <stdio.h>

// ##################################
// ### FUNCTIONS DECLARATION PART ###
// ##################################

/**
 * @brief Parse JSON buffer into cJSON root object.
 *
 * @param json_buffer JSON text buffer.
 * @return cJSON* Parsed root object, or NULL on failure.
 */
cJSON *get_json_root(const char *json_buffer);

/**
 * @brief Retrieve required JSON object child by name.
 *
 * @param root Parent JSON root.
 * @param obj_name Object key name.
 * @return cJSON* Object node, or NULL on failure.
 */
cJSON *get_json_object(cJSON *root, const char *obj_name);

#endif // __CRUN_JSON_MANAGER_H__
