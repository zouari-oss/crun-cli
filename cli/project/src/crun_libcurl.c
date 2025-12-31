/**
 * @file      crun_libcurl.c
 * @author    @ZouariOmar <zouariomar20@gmail.com>
 * @brief     crun_libcurl source file
 * @version   0.1
 * @date      2025-12-26
 * @copyright Copyright (c) 2025
 * @link      https://github.com/ZouariOmar/crun/project/src/crun_libcurl.c crun_libcurl.c @endlink
 */

// ########################
// ### INC HEADERS PART ###
// ########################

// Include curl header(s)
#include <curl/curl.h>

// Include custom header(s)
#include "../inc/crun_libcurl.h"

// ##################################
// ### LIBCURL FUNCTIONS DEV PART ###
// ##################################

size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
  return fwrite(ptr, size, nmemb, (FILE *)stream);
}

int download_file(const char *url, const char *out) {
  CURL *curl = curl_easy_init();
  if (!curl)
    return EXIT_FAILURE;

  FILE *fp = fopen(out, "wb");
  if (!fp) {
    fprintf(stderr, "[WARR] crun_libcurl.c :: Can't open/write %s\n", out);
    curl_easy_cleanup(curl);
    return EXIT_FAILURE;
  }

  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // NOTE: packages follow other routes ==> I NEED THIS LINE
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

  CURLcode res = curl_easy_perform(curl);

  // Cleaning
  fclose(fp);
  fp = NULL;
  curl_easy_cleanup(curl);
  curl = NULL;

  return EXIT_FAILURE;
}
