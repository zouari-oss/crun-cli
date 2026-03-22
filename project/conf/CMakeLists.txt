# === Cmake info ===
cmake_minimum_required(VERSION 4.2.3)
# === !Cmake info ===

# === Project info ===
project(
  crun
  VERSION 1.4.0.3
  DESCRIPTION "C-based project scaffolding CLI"
  HOMEPAGE_URL https://github.com/zouari-oss/crun-cli
  LANGUAGES C
)
# === !Project info ===

# === Setting section ===
set(CMAKE_C_STANDARD 23)
set(CMAKE_BUILD_TYPE Release)
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/crun.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/crun_libcurl.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/crun_audit.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/crun_json_manager.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/crun_file_manager.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/crun_zip_manager.c)
set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc/crun.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/patterns.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/color.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc/crun_libcurl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc/crun_audit.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc/crun_json_manager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc/crun_file_manager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc/crun_zip_manager.h)
# === !Setting section ===

# === Find packages part ===
find_package(CURL REQUIRED)
find_package(cJSON REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(MINIZIP REQUIRED minizip)
# === !Find packages part ===

# === Exec inline command ===
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
# === !Exec inline command ===

# === Linkers part ===
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CURL_INCLUDE_DIRS}
    ${CJSON_INCLUDE_DIRS}
    ${MINIZIP_INCLUDE_DIRS}
)
target_link_libraries(${PROJECT_NAME} PRIVATE
                      CURL::libcurl
                      ${CJSON_LIBRARIES}
                      ${MINIZIP_LIBRARIES})
# === !Linkers part ===

# === Include(s) part
include(Packaging.cmake)
# === !Include(s) part
