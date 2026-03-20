/**
 * @file      crun_libcurl.c
 * @author    ZouariOmar <zouariomar20@gmail.com>
 * @brief     crun_libcurl source file
 * @version   0.1
 * @date      2025-12-26
 * @copyright Copyright (c) 2025
 *
 * <a href="https://github.com/zouari-oss/crun-cli/project/src/crun_libcurl.c">crun_libcurl.c</a>
 */

#include "../inc/crun_libcurl.h"
#include "../inc/crun_audit.h"
#include "../inc/crun_file_manager.h"
#include <curl/curl.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

struct DownloadProgress {
  int last_percent;
  int started;
};

static const char *ensure_crun_ca_certificate(void) {
  const char *ca_file_path = get_file_home_path(CRUN_DEFAULT_SUFFIX_DIRECTORY CURL_CA_CERT_NAME);
  if (!ca_file_path) {
    crun_audit_error("Unable to resolve CA certificate path.");
    return NULL;
  }

  if (is_file_exist(ca_file_path))
    return ca_file_path;

  const char *crun_root_directory = get_file_home_path(CRUN_DEFAULT_SUFFIX_DIRECTORY);
  if (!crun_root_directory) {
    crun_audit_error("Unable to resolve crun root directory for CA certificate.");
    free((void *)ca_file_path);
    return NULL;
  }

  if (__mkdir(crun_root_directory) != 0 && !is_file_exist(crun_root_directory)) {
    crun_audit_error("Unable to create crun root directory for CA certificate: %s", crun_root_directory);
    free((void *)crun_root_directory);
    free((void *)ca_file_path);
    return NULL;
  }

  free((void *)crun_root_directory);
  crun_root_directory = NULL;

  CURL *bootstrap_curl = curl_easy_init();
  if (!bootstrap_curl) {
    crun_audit_error("Unable to initialize curl for CA certificate bootstrap.");
    free((void *)ca_file_path);
    return NULL;
  }

  FILE *ca_file = fopen(ca_file_path, "wb");
  if (!ca_file) {
    crun_audit_error("Unable to open CA certificate destination path: %s", ca_file_path);
    curl_easy_cleanup(bootstrap_curl);
    free((void *)ca_file_path);
    return NULL;
  }

  curl_easy_setopt(bootstrap_curl, CURLOPT_URL, "https://curl.se/ca/cacert-2026-03-19.pem");
  curl_easy_setopt(bootstrap_curl, CURLOPT_WRITEFUNCTION, write_data);
  curl_easy_setopt(bootstrap_curl, CURLOPT_WRITEDATA, ca_file);
  curl_easy_setopt(bootstrap_curl, CURLOPT_FOLLOWLOCATION, 1L);

  crun_audit_info("Downloading CA certificate bundle...");
  const CURLcode bootstrap_res = curl_easy_perform(bootstrap_curl);
  fclose(ca_file);
  ca_file = NULL;
  curl_easy_cleanup(bootstrap_curl);
  bootstrap_curl = NULL;

  if (bootstrap_res != CURLE_OK) {
    remove(ca_file_path);
    crun_audit_error("Failed to bootstrap CA certificate (%s).", curl_easy_strerror(bootstrap_res));
    free((void *)ca_file_path);
    return NULL;
  }

  crun_audit_info("CA certificate bundle installed: %s", ca_file_path);
  return ca_file_path;
}

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
  const char *ca_certificate_path = ensure_crun_ca_certificate();
  if (!ca_certificate_path)
    return EXIT_FAILURE;

  CURL *curl = curl_easy_init();
  if (!curl) {
    free((void *)ca_certificate_path);
    return EXIT_FAILURE;
  }

  FILE *fp = fopen(out, "wb");
  if (!fp) {
    crun_audit_error("Cannot open destination file for download: %s", out);
    curl_easy_cleanup(curl);
    free((void *)ca_certificate_path);
    return EXIT_FAILURE;
  }

  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // NOTE: packages follow other routes ==> I NEED THIS LINE
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
  curl_easy_setopt(curl, CURLOPT_CAINFO, ca_certificate_path);
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
  free((void *)ca_certificate_path);
  ca_certificate_path = NULL;

  if (res != CURLE_OK) {
    crun_audit_error("Download failed (%s): %s", curl_easy_strerror(res), url);
    return EXIT_FAILURE;
  }

  crun_audit_info("Download completed successfully: %s", out);
  return EXIT_SUCCESS;
}
