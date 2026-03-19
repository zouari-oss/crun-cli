/**
 * @file      color.h
 * @author    ZouariOmar (zouariomar20@gmail.com)
 * @brief     ANSI Color header file
 * @version   0.1
 * @date      2025-01-10
 * @copyright Copyright (c) 2025
 *
 * <a href="https://github.com/zouari-oss/cpkg">cpkg</a>
 */

#ifndef __COLOR_H__
#define __COLOR_H__

// #########################################
// ### HEADERS & MACROS DECLARATION PART ###
// #########################################

// Text colors
#define red "\x1B[1;31m"
#define green "\x1B[1;32m"
#define yellow "\x1B[1;33m"
#define bleu "\x1B[1;34m"
#define cyan "\x1B[1;36m"
#define def "\x1B[0m"
#define black "\x1B[1;30m"
#define magenta "\x1B[1;35m"
#define white "\x1B[1;37m"
#define gray "\x1B[0;37m"

// Background colors
#define bgRed "\x1B[41m"
#define bgGreen "\x1B[42m"
#define bgYellow "\x1B[43m"
#define bgBlue "\x1B[44m"
#define bgMagenta "\x1B[45m"
#define bgCyan "\x1B[46m"
#define bgWhite "\x1B[47m"

#endif // __COLOR_H__
