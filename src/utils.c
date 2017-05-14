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
#include "utils.h"
#define NUMBER_BUFFER_SIZE 30

char *number_to_str(void *x, const char *format, type_tag tag)
{
    char buffer[NUMBER_BUFFER_SIZE];

    size_t size = 0;
    switch (tag) {
        case TYPE_INT:
            size = sprintf(buffer, format, *((int *)x)); break;
        case TYPE_UL:
            size = sprintf(buffer, format, *((unsigned long *)x)); break;
        case TYPE_ULL:
            size = sprintf(buffer, format, *((unsigned long long *)x)); break;
        case TYPE_DOUBLE:
            size = sprintf(buffer, format, *((double *)x)); break;
    }

    char *copy = malloc(size + 1);
    copy[size] = '\0';

    return copy ? memcpy(copy, buffer, size) : NULL;
}

char *int_to_str(int x)
{
    return number_to_str((void *)&x, "%d", TYPE_INT);
}

char *ul_to_str(unsigned long x)
{
    return number_to_str((void *)&x, "%lu", TYPE_UL);
}

char *ull_to_str(unsigned long long x)
{
    return number_to_str((void *)&x, "%llu", TYPE_ULL);
}

char *double_to_str(double x)
{
    return number_to_str((void *)&x, "%.1f", TYPE_DOUBLE);
}

char *copy_str_n(const char *str, size_t n)
{
    if (!str)
        return NULL;

    char *copy = malloc(n + 1);

    return copy ? memcpy(copy, str, n + 1) : NULL;
}

char *copy_str(const char *str)
{
    if (!str) 
        return NULL;

    size_t len = strlen(str);

    return copy_str_n(str, len);
}

void free_pointer_char(char **begin, char **end)
{
    char **iter = begin;
    while (iter != end) {
        free(*iter);
        ++iter;
    }
    free(begin);
}
