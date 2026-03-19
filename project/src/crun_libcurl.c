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
#include "../inc/crun_audit.h"
#include "../inc/crun_libcurl.h"

// ##################################
// ### LIBCURL FUNCTIONS DEV PART ###
// ##################################

struct DownloadProgress {
  int last_percent;
  int started;
};

size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
  return fwrite(ptr, size, nmemb, (FILE *)stream);
}

static int download_progress_cb(void *clientp,
                                curl_off_t dltotal,
                                curl_off_t dlnow,
                                curl_off_t ultotal,
                                curl_off_t ulnow) {
  struct DownloadProgress *progress = (struct DownloadProgress *)clientp;
  (void)ultotal;
  (void)ulnow;

  if (dltotal <= 0)
    return 0;

  int percent = (int)((dlnow * 100) / dltotal);
  if (progress && progress->last_percent == percent)
    return 0;

  if (progress) {
    progress->last_percent = percent;
    progress->started = 1;
  }

  fprintf(stdout, "\r[INFO] Download progress: %3d%%", percent);
  fflush(stdout);
  return 0;
}

int download_file(const char *url, const char *out) {
  struct DownloadProgress progress = {.last_percent = -1, .started = 0};

  CURL *curl = curl_easy_init();
  if (!curl)
    return EXIT_FAILURE;

  FILE *fp = fopen(out, "wb");
  if (!fp) {
    crun_audit_error("Cannot open destination file for download: %s", out);
    curl_easy_cleanup(curl);
    return EXIT_FAILURE;
  }

  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // NOTE: packages follow other routes ==> I NEED THIS LINE
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
  curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
  curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, download_progress_cb);
  curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &progress);

  crun_audit_info("Download started: %s", url);

  CURLcode res = curl_easy_perform(curl);
  if (progress.started)
    fprintf(stdout, "\n");

  // Cleaning
  fclose(fp);
  fp = NULL;
  curl_easy_cleanup(curl);
  curl = NULL;

  if (res != CURLE_OK) {
    crun_audit_error("Download failed (%s): %s", curl_easy_strerror(res), url);
    return EXIT_FAILURE;
  }

  crun_audit_info("Download completed successfully: %s", out);
  return EXIT_SUCCESS;
}
