# === Include(s) part ===
include(InstallRequiredSystemLibraries)
include(GNUInstallDirs)
# === !Include(s) part ===

# === Package info ===
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VENDOR "@ZouariOmar")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_CONTACT ${HOMEPAGE_URL})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${DESCRIPTION})
set(CPACK_PACKAGE_CONTACT "@ZouariOmar <zouariomar20gmail.com>")
set(CPACK_PACKAGE_ICON "logo.png")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "crun-pkg")
# === !Package info ===

# === Packaging info ===
set(CPACK_PACKAGE_FILE_NAME
    "${PROJECT_NAME}_${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}-${CMAKE_PROJECT_VERSION_TWEAK}_${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_PROCESSOR}"
)
set(CPACK_GENERATOR "STGZ;ZIP;TGZ;DEB;RPM;AppImage")
set(CPACK_PACKAGING_INSTALL_PREFIX "/usr/local")
set(CPACK_VERBOSE ON)
# === !Packaging info ===

# === Options part ===
option(CRUN_BUNDLE_RUNTIME_DEPS "Bundle resolved runtime shared libraries into install tree" ON)
# === !Options part ===

# === Deb conf ===
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Zouari Omar")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libcjson1, libcurl4, libminizip1")
# === !Deb conf ===

# === Rpm conf ===
set(CPACK_RPM_PACKAGE_LICENSE "GPL-3.0")
set(CPACK_RPM_PACKAGE_GROUP "Applications/Development")
set(CPACK_RPM_PACKAGE_AUTOREQ ON)
set(CPACK_RPM_PACKAGE_AUTOPROV ON)
set(CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")
# === !Rpm conf ===

# === AppImage conf ===
set(CPACK_APPIMAGE_DESKTOP_FILE "crun.desktop")
set(CPACK_APPIMAGE_ICON ${CPACK_PACKAGE_ICON})
# === !AppImage conf ===

# === Installation part ===
if(CRUN_BUNDLE_RUNTIME_DEPS)
  install(TARGETS ${PROJECT_NAME}
          RUNTIME_DEPENDENCY_SET crun_runtime_deps
          LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}
          RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/${PROJECT_NAME}
  )

  install(RUNTIME_DEPENDENCY_SET crun_runtime_deps
          DESTINATION ${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}
          PRE_EXCLUDE_REGEXES "^linux-vdso\\.so.*"
          POST_EXCLUDE_REGEXES ".*/(ld-linux[^/]*|libc|libm|libpthread|libdl|librt)\\.so(\\..*)?$"
  )
else()
  install(TARGETS ${PROJECT_NAME}
          LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}
          RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/${PROJECT_NAME}
  )
endif()

install(FILES ${HEADERS}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
        DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}
        FILES_MATCHING
        PATTERN "*.cmake"
        PATTERN "CMakeLists.txt"
)

install(FILES ${CMAKE_SOURCE_DIR}/../res/app/${CPACK_APPIMAGE_DESKTOP_FILE}
              ${CMAKE_SOURCE_DIR}/../res/img/${CPACK_APPIMAGE_ICON}
        DESTINATION share)
# === !Installation part ===

# === Include(s) part ===
include(CPack)
# === !Include(s) part ===

