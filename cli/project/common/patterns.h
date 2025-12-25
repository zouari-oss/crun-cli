/**
 * @file      patterns.h
 * @author    ZouariOmar (zouariomar20@gmail.com)
 * @brief     patterns header file
 * @version   0.1
 * @date      2024-10-19
 * @copyright Copyright (c) 2024
 * @link https://github.com/ZouariOmar/crun/project/import/patterns.hpp patterns.hpp @endlink
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
         "https://github.com/ZouariOmar ====%s\n\n",                  \
         bleu, def)

#define PROJECT_MENU_FIELD_LENGTH(i, field) snprintf(NULL, 0, "\t\t\t[%d] %s\n", i, field)

#define PROJECT_MENU_FIELD(buff, i, field) sprintf(buff, "\t\t\t[%d] %s\n", i, field)

#define PROJECT_MENU(fileds)                                                                                  \
  printf("%s\t\t------------------------------------------------------------------------------------------\n" \
         "%s\t\tProject Language:\n"                                                                          \
         "%s%s"                                                                                               \
         "\t\t\t[0] Exit\n"                                                                                   \
         "%s\t\tVisit https://github.com/ZouariOmar/crun for more info\n"                                     \
         "%s\t\t------------------------------------------------------------------------------------------\n" \
         "...%s",                                                                                             \
         yellow, red, yellow, fileds, red, yellow, def)

#define PROJECT_NAME printf("%s\n\nProject Name...\n%s", yellow, def)

//? Messages patterns
#define HELP_MSG                                                                            \
  printf("%sUsage:\n%s"                                                                     \
         "  --language <c|cpp|java> Set the language (c = 1, cpp = 2, java = 3)\n"          \
         "  --project <N>           Set the project ID (N must be a number from 1 to 9+)\n" \
         "  --name <NAME>           Set the user name (only letters A-Z, a-z)\n"            \
         "  --help                  Display this help message\n",                           \
         yellow, def)
#define INVALID_OPTION_MSG printf("%s\n\n[WARR] Invalid Option !\n\n%s", red, def)

#define EXIT_SUCCESS_MSG printf("%s\n\n[SUCCESS] See You Next Time !\n\n%s", green, def)

#define INSTALLATION_COMPLETE_MSG printf("%sInstallation Complete!\nHAPPY CODING ⚡️\n%s", green, def)

//? REGEX `crun` patterns
#define CRUN_LANGUAGE_REGEX_PATTERN "--language([1-9]+)"
#define CRUN_PROJECT_REGEX_PATTERN "--project([1-9]+)"
#define CRUN_NAME_REGEX_PATTERN "--name([A-Za-z]+)"
#define CRUN_HELP_REGEX_PATTERN "--help"

//? `crun` files
#define CRUN_STACKS_JSON_URL "https://raw.githubusercontent.com/ZouariOmar/cpkg/refs/heads/main/crun/crun_stacks.json"

//? OS Info
#ifdef _WIN32
#define OS_HOME getenv("APPDATA")
#else
#define OS_HOME getenv("HOME")
#endif
