/* This file contains utility routines such as copying and freeing data
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
#ifndef UTILS_H
#define UTILS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Tag used by number_to_str function.
 * @see number_to_str
 */
typedef enum {
    TYPE_INT,       /** Convert int */
    TYPE_UL,        /** Convert unsigned long */
    TYPE_ULL,       /** Convert unsigned long long */
    TYPE_DOUBLE     /** Convert double */
} type_tag;
/**
 * Generic function for copying x value into string.
 * Returned value must be freed by the user.
 * @param x         - value to be converted
 * @param format    - format to be used by sprintf
 * @param tag       - what is the type of x value
 * @return value converted to string, NULL otherwise
 */
char *number_to_str(void *x, const char *format, type_tag tag);
/**
 * Converts int x to string
 * @param x - value to be converted
 * @see number_to_str
 */
char *int_to_str(int x);
/**
 * Converts unsigned long x to string
 * @param x - value to be converted
 * @see number_to_str
 */
char *ul_to_str(unsigned long x);
/**
 * Converts unsigned long long x to string
 * @param x - value to be converted
 * @see number_to_str
 */
char *ull_to_str(unsigned long long x);
/**
 * Converts double x to string
 * @param x - value to be converted
 * @see number_to_str
 */
char *double_to_str(double x);

/**
 * Custom version of POSIX's strdup function.
 * Copy up to n elements.
 * @param str   - string to copy
 * @param n     - number of elements to copy
 * @see copy_str
 */
char *copy_str_n(const char *str, size_t n);
/**
 * Custom version of POSIX's strdup function.
 * @param str - string to copy
 * @return copy allocated string of str
 * @see copy_str_n
 */
char *copy_str(const char *str);

/**
 * Deallocates memory pointed by char** string.
 * @param begin - string to the beginning of string's list
 * @param end   - string to the beginning of string's list
 */
void free_pointer_char(char **begin, char **end);

#endif /* UTILS_H */
