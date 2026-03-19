# Set the cmake min version (current - 4.1.2)
cmake_minimum_required(VERSION 4.1.2)

# Set the project name
project(
  crun
  DESCRIPTION
    A-simple-program-to-automate-the-setup-of-new-projects-using-templates-from-cpkg-repo.
  VERSION 1.4.0
  HOMEPAGE_URL https://github.com/ZouariOmar/crun
  LANGUAGES C CXX)

# Set the CMake version
set(CMAKE_CX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set compiler flags -- add_compile_options(-Wall -Wextra)

# Generate release product
set(CMAKE_BUILD_TYPE Release)

# Find packages
find_package(CURL REQUIRED)
find_package(cJSON REQUIRED)

# Find modules
pkg_check_modules(MINIZIP REQUIRED minizip)

# Include packages
include_directories(${CURL_INCLUDE_DIR} ${CJSON_INCLUDE_DIRS}
                    ${MINIZIP_INCLUDE_DIRS})

# Set sources files
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/crun.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/crun_libcurl.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/crun_audit.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/crun_json_manager.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/crun_file_manager.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/crun_zip_manager.c)

# Set headers files
set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc/crun.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/patterns.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/color.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc/crun_libcurl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc/crun_audit.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc/crun_json_manager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc/crun_file_manager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc/crun_zip_manager.h)

# Generate Crun
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link packages after creating the executable
target_link_libraries(${PROJECT_NAME} PRIVATE CURL::libcurl ${CJSON_LIBRARIES}
                                              ${MINIZIP_LIBRARIES})
