/**
 * @file      patterns.h
 * @author    ZouariOmar (zouariomar20@gmail.com)
 * @brief     patterns header file
 * @version   0.1
 * @date      2024-10-19
 * @copyright Copyright (c) 2024
 *
 * <a href="https://github.com/zouari-oss/crun-cli/projec/common/patterns.h">patterns.h</a>
 */

// #########################################
// ### HEADERS & MACROS DECLARATION PART ###
// #########################################

#define CRUN_VERSION "v1.4.0-3"
#define CRUN_AUTHOR "ZouariOmar"
#define CRUN_REPOSITORY "https://github.com/zouari-oss"

#include "color.h"

#ifdef _WIN32
#define CRUN_HEADER system("cmd /c echo [Crun - " CRUN_VERSION "]");
#else
#define CRUN_HEADER \
  system("echo \"" green "$(figlet -w $(tput cols) '                         Crun - " CRUN_VERSION "')\"" def)
#endif

#define CRUN_BANNER                                                                                          \
  printf("%s\t\t\t\t=================== Crun Project ===================\n\t\t\t\t================== "       \
         "By @" CRUN_AUTHOR " ==================\n\t\t\t\t==== Visit Us In: " CRUN_REPOSITORY " ====%s\n\n", \
         bleu, def)

#define PROJECT_MENU_FIELD_LENGTH(i, field) snprintf(NULL, 0, "\t\t\t[%d] %s\n", i, field)

#define PROJECT_MENU_FIELD(buff, i, field) sprintf(buff, "\t\t\t[%d] %s\n", i, field)

#define PROJECT_MENU(fileds)                                                                                  \
  printf("%s\t\t------------------------------------------------------------------------------------------\n" \
         "%s\t\tStacks:\n"                                                                                    \
         "%s%s"                                                                                               \
         "\t\t\t[0] Exit\n"                                                                                   \
         "%s\t\tVisit https://github.com/zouari-oss/crun-cli for more info\n"                                 \
         "%s\t\t------------------------------------------------------------------------------------------\n" \
         "...%s",                                                                                             \
         yellow, red, yellow, fileds, red, yellow, def)

#define VERSION_MSG                                 \
  printf("%scrun %s\n"                              \
         "Copyright (C) 2026 zouari-oss, Inc.%s\n", \
         yellow, CRUN_VERSION, def)

#define HELP_MSG                                                                       \
  printf("%scrun usage:%s\n"                                                           \
         "  crun                 Run interactive template selection flow\n"            \
         "  crun --help | -h     Show this help message\n"                             \
         "  crun --update        Refresh local stacks metadata (crun_stacks.json)\n\n" \
         "%sNotes:%s\n"                                                                \
         "  - Templates are cached under ~/.local/share/crun/ (Linux).\n"              \
         "  - Audit logs are written to ~/.local/share/crun/audit.log.\n",             \
         yellow, def, yellow, def)

#define INSTALLATION_COMPLETE_MSG printf("%sInstallation Complete!\nHAPPY CODING :)\n%s", green, def)
