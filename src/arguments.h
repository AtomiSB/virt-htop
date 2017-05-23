/* This file contains command line arguments routines
 * Copyright (C) 2017 Atomi
 * Author: Sebastian Bialobrzecki <sbb@openmailbox.org>
 *
 * This file is part of virt-htop.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
/** @file arguments.h 
 * This file contains command line arguments routines 
 */
#ifndef ARGUMENTS_H
#define ARGUMENTS_H
/**
 * Number of possible argument choices, 
 * size of the options_value and options_count arrays. 
 */
#define OPTIONS_SIZE (4)

/**
 * Used for indexing the options_value and options_count arrays 
 */
typedef enum {
    CONNECT_SHORT, CONNECT_LONG,
    HELP_SHORT, HELP_LONG
} options_enum;

/**
 * Available command line argument options as strings 
 * @see options_enum 
 */
const char  *options_value[OPTIONS_SIZE];
/**
 * Number of possible values for options_value[options_enum] array 
 * @see options_enum 
 */
int options_count[OPTIONS_SIZE];

/**
 * Print usage information.
 */
void print_usage();

/**
 * Print helpful information.
 */
void print_help();

/**
 * Parse command line argument values for given option, return valid argument values.
 * @param begin     - pointer to the first string of command line argument list
 * @param end       - pointer to the last string of command line argument list
 * @param option    - enum index of options_enum type
 * @return char** of parsed values for option, NULL otherwise 
 */
char **parser_find_option(const char **begin, const char **end, options_enum option);

#endif /* ARGUMENTS_H */
