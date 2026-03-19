/**
 * @file      crun_json_manager.h
 * @author    @ZouariOmar (zouariomar20@gmail.com)
 * @brief     JSON parsing helpers for stacks metadata.
 * @version   0.3
 * @date      2026-03-18
 * @copyright Copyright (c) 2025
 * @link https://github.com/ZouariOmar/crun/project/inc/crun_json_manager.h crun_json_manager.h @endlink
 */

#ifndef __CRUN_JSON_MANAGER_H__
#define __CRUN_JSON_MANAGER_H__

// ################################
// ### HEADERS DECLARATION PART ###
// ################################
// Include stander C header(s)
#include <stdio.h>

// Include json header(s)
#include <cjson/cJSON.h>

/**
 * @brief Parse JSON buffer into cJSON root object.
 *
 * @param json_buffer JSON text buffer.
 * @return cJSON* Parsed root object, or NULL on failure.
 */
cJSON *get_json_root(const char *);

/**
 * @brief Retrieve required JSON object child by name.
 *
 * @param root Parent JSON root.
 * @param obj Object key name.
 * @return cJSON* Object node, or NULL on failure.
 */
cJSON *get_json_object(cJSON *, const char *);

#endif // __CRUN_JSON_MANAGER_H__
