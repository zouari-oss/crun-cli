/**
 * @file      crun_zip_manager.c
 * @author    @ZouariOmar <zouariomar20@gmail.com>
 * @brief     crun_zip_manager source file
 * @version   0.1
 * @date      2026-01-01
 * @copyright Copyright (c) 2025
 * @link      https://github.com/ZouariOmar/crun/project/src/crun_zip_manager.c crun_zip_manager.c @endlink
 * @link      https://chromium.googlesource.com/external/github.com/nmoinvaz/minizip/+/2.7.1/README.md README.md @endlink
 */

// ############################
// ### HEADERS INCLUDE PART ###
// ############################

// Inlcude stander header(s)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// Include zip header(s)
#include <minizip/unzip.h>

// Include custom header(s)
#include "../inc/crun_audit.h"
#include "../inc/crun_zip_manager.h"

// ##############################
// ### ZIP FUNCTIONS DEV PART ###
// ##############################

static void ensure_dir(const char *path) {
  char tmp[1024];
  snprintf(tmp, sizeof(tmp), "%s", path);

  for (char *p = tmp + 1; *p; p++) {
    if (*p == '/' || *p == '\\') {
      char old = *p;
      *p = '\0';
      MKDIR(tmp);
      *p = old;
    }
  }
}

static int is_safe_path(const char *path) {
  return strstr(path, "..") == NULL;
}

static const char *get_expected_init_script_name() {
#ifdef _WIN32
  return "__init__.bat";
#else
  return "__init__.sh";
#endif
}

static void maybe_capture_init_script(const char *zip_entry_name,
                                      const char *out_path,
                                      char *init_script_path,
                                      size_t init_script_path_size) {
  if (!init_script_path || !init_script_path_size || init_script_path[0] != '\0')
    return;

  const char *entry_basename = strrchr(zip_entry_name, '/');
  if (!entry_basename)
    entry_basename = strrchr(zip_entry_name, '\\');

  entry_basename = entry_basename ? entry_basename + 1 : zip_entry_name;
  if (!strcmp(entry_basename, get_expected_init_script_name()))
    snprintf(init_script_path, init_script_path_size, "%s", out_path);
}

int extract_zip(const char *zip_path,
                const char *out_dir,
                char *init_script_path,
                size_t init_script_path_size) {
  if (init_script_path && init_script_path_size)
    init_script_path[0] = '\0';

  unzFile zip = unzOpen(zip_path);
  if (!zip) {
    crun_audit_error("Unable to open zip archive: %s", zip_path);
    return EXIT_FAILURE;
  }

  unz_global_info info;
  if (unzGetGlobalInfo(zip, &info) != UNZ_OK) {
    crun_audit_error("Unable to read zip metadata: %s", zip_path);
    unzClose(zip);
    return EXIT_FAILURE;
  }

  int extract_res = EXIT_SUCCESS;
  for (uLong i = 0; i < info.number_entry; i++) {
    unz_file_info file_info;
    char name[512];

    if (unzGetCurrentFileInfo(zip, &file_info,
                              name, sizeof(name),
                              NULL, 0, NULL, 0) != UNZ_OK) {
      extract_res = EXIT_FAILURE;
      break;
    }

    if (!strlen(name))
      continue;

    if (!is_safe_path(name)) {
      crun_audit_error("Unsafe zip entry detected: %s", name);
      unzClose(zip);
      return EXIT_FAILURE;
    }

    char out_path[1024];
    snprintf(out_path, sizeof(out_path),
             "%s%c%s", out_dir, PATH_SEP, name);

    if (name[strlen(name) - 1] == '/') {
      MKDIR(out_path);
    } else {
      maybe_capture_init_script(name, out_path, init_script_path, init_script_path_size);
      ensure_dir(out_path);

      if (unzOpenCurrentFile(zip) != UNZ_OK) {
        extract_res = EXIT_FAILURE;
        break;
      }

      FILE *fp = fopen(out_path, "wb");
      if (!fp) {
        unzCloseCurrentFile(zip);
        extract_res = EXIT_FAILURE;
        break;
      }

      char buf[8192];
      int read;
      while ((read = unzReadCurrentFile(zip, buf, sizeof(buf))) > 0) {
        fwrite(buf, 1, read, fp);
      }

      if (read < 0)
        extract_res = EXIT_FAILURE;

      fclose(fp);
      unzCloseCurrentFile(zip);

      if (extract_res != EXIT_SUCCESS)
        break;
    }

    if (i + 1 < info.number_entry)
      unzGoToNextFile(zip);
  }

  unzClose(zip);
  return extract_res;
}
