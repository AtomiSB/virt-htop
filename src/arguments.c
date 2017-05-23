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
#include "arguments.h"
#include "utils.h"

const char *options_value[OPTIONS_SIZE] = {
    "-c", "--connect",
    "-h", "--help"
};

int options_count[OPTIONS_SIZE] = {
    1, 1,
    0, 0
};

void print_usage()
{
    printf("Usage: virt-htop [option] -c|--connect <URL>\n");
    printf("--help -h:              Print this information\n");
    printf("--connect -c <URL>:     Connect to the <URL> node\n");
    printf("\n");
}

void print_help()
{
    printf("virt-htop Copyright (C) 2017 Atomi Sebastian Bialobrzecki\n");
    printf("This is free software; see the source for copying conditions.\n"\
            "There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
    printf("\n");
}

char **parser_find_option(const char **begin, const char **end, options_enum option)
{
    if (!(end - begin)) return NULL;

    const char **iter = begin;
    /* find option first */
    while (iter != end) {
        int pos = strcmp(*begin, options_value[option]);

        if (pos == 0) {
            /* If option doesn't have arguments */
            if (options_count[option] == 0)
                return (char **)(options_value + option);

            ++iter;

            if (!*iter) return NULL;

            char **arguments = calloc(options_count[option], sizeof(char *));
            /* find arguments */
            for (int i = 0; i != options_count[option]; ++i, ++iter)
                if (*iter)
                    arguments[i] = copy_str(*iter);
            return arguments;
        }
        ++iter;
    }

    return NULL;
}
