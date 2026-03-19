/**
 * @file      patterns.h
 * @author    ZouariOmar (zouariomar20@gmail.com)
 * @brief     patterns header file
 * @version   0.1
 * @date      2024-10-19
 * @copyright Copyright (c) 2024
 * @link      https://github.com/zouari-oss/crun-cli/projec/common/patterns.h @endlink
 */

// Custom include files
#include "color.h"

//? Menus patterns
#ifdef _WIN32
#define CRUN_HEADER \
  system("cmd /c echo [Crun - V1.4]");
#else
#define CRUN_HEADER                                                            \
  system("echo \"\033[0;32m$(figlet -w $(tput cols) '                        " \
         "        Crun - V1.4')\033[0m\"")
#endif

#define CRUN_BANNER                                                   \
  printf("%s\t\t\t\t=================== Crun Project "                \
         "===================\n\t\t\t\t================== By "        \
         "@ZouariOmar ==================\n\t\t\t\t==== Visit Us In: " \
         "https://github.com/zouari-oss ====%s\n\n",                  \
         bleu, def)

#define PROJECT_MENU_FIELD_LENGTH(i, field) snprintf(NULL, 0, "\t\t\t[%d] %s\n", i, field)

#define PROJECT_MENU_FIELD(buff, i, field) sprintf(buff, "\t\t\t[%d] %s\n", i, field)

#define PROJECT_MENU(fileds)                                                                                  \
  printf("%s\t\t------------------------------------------------------------------------------------------\n" \
         "%s\t\tProject Language:\n"                                                                          \
         "%s%s"                                                                                               \
         "\t\t\t[0] Exit\n"                                                                                   \
         "%s\t\tVisit https://github.com/zouari-oss/crun-cli for more info\n"                                 \
         "%s\t\t------------------------------------------------------------------------------------------\n" \
         "...%s",                                                                                             \
         yellow, red, yellow, fileds, red, yellow, def)

//? Messages patterns
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
