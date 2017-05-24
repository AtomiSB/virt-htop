/* This file contains routines to draw terminal output using ncurses
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
#include "tui_node.h"

const char *tui_node_info_type[TUI_NODE_INFO_SIZE] = {
    "Hostname:",
    "     URI:",
    " Version:",
    "  Memory:",
    "  Memory:"
};

void tui_init_default_node_data(tui_node_data *tui)
{
    tui->node_memory_size    = NULL;
    for (int i = 0; i != TUI_NODE_INFO_SIZE; ++i) {
        tui->node_data[i] = NULL;
        tui->node_type[i] = i;
    }
}

void tui_deinit_node_data(tui_node_data *tui)
{
    for (int i = 0; i != TUI_NODE_INFO_SIZE; ++i)
        free(tui->node_data[i]);
}

void tui_create_node_panel(tui_node_data *tui, virt_node_data *data)
{
    /* copy the pointers */
    tui->node_data[TUI_NODE_INFO_HOSTNAME] = 
        data->node_data[data->node_type[VIRT_NODE_DATA_TYPE_HOSTNAME]];
    tui->node_data[TUI_NODE_INFO_URI] = 
        data->node_data[data->node_type[VIRT_NODE_DATA_TYPE_URI]];
    tui->node_data[TUI_NODE_INFO_LIB_VERSION] = 
        data->node_data[data->node_type[VIRT_NODE_DATA_TYPE_LIB_VERSION]];
    tui->node_data[TUI_NODE_INFO_TOTAL_MEMORY] = 
        data->node_data[data->node_type[VIRT_NODE_DATA_TYPE_TOTAL_MEMORY]];

    /* set up default order */
    tui->node_type[0] = TUI_NODE_INFO_HOSTNAME;
    tui->node_type[1] = TUI_NODE_INFO_URI;
    tui->node_type[2] = TUI_NODE_INFO_LIB_VERSION;
    tui->node_type[3] = TUI_NODE_INFO_TOTAL_MEMORY;
    tui->node_type[4] = TUI_NODE_INFO_DOMAINS_MEMORY;
}

void tui_node_update_memory_data(tui_node_data *tui, virt_domain_data *data)
{
    tui->node_data[TUI_NODE_INFO_DOMAINS_MEMORY] = data->domain_memory_size;
}

void tui_draw_node_panel(tui_node_data *tui)
{
    int x = 0, y = 0;
    /*for (int i = 0; i != TUI_NODE_INFO_SIZE; ++i, ++y) {*/
        /*mvwaddstr(stdscr, y, x, tui_node_info_type[tui->domain_type[i]]);*/
        /*attron(A_BOLD | COLOR_PAIR(TUI_COLOR_HELP_KEY));*/
        /*waddch(stdscr, ' ');*/
        /*waddstr(stdscr, tui->node_data[tui->domain_type[i]]);*/
        /*attroff(A_BOLD | COLOR_PAIR(TUI_COLOR_HELP_KEY));*/
    /*}*/
    mvwaddstr(stdscr, y++, x, tui_node_info_type[TUI_NODE_INFO_HOSTNAME]);
    attron(A_BOLD | COLOR_PAIR(TUI_COLOR_HELP_KEY));
    waddch(stdscr, ' ');
    waddstr(stdscr, tui->node_data[TUI_NODE_INFO_HOSTNAME]);
    attroff(A_BOLD | COLOR_PAIR(TUI_COLOR_HELP_KEY));

    mvwaddstr(stdscr, y++, x, tui_node_info_type[TUI_NODE_INFO_URI]);
    attron(A_BOLD | COLOR_PAIR(TUI_COLOR_HELP_KEY));
    waddch(stdscr, ' ');
    waddstr(stdscr, tui->node_data[TUI_NODE_INFO_URI]);
    attroff(A_BOLD | COLOR_PAIR(TUI_COLOR_HELP_KEY));

    mvwaddstr(stdscr, y++, x, tui_node_info_type[TUI_NODE_INFO_LIB_VERSION]);
    attron(A_BOLD | COLOR_PAIR(TUI_COLOR_HELP_KEY));
    waddch(stdscr, ' ');
    waddstr(stdscr, tui->node_data[TUI_NODE_INFO_LIB_VERSION]);
    attroff(A_BOLD | COLOR_PAIR(TUI_COLOR_HELP_KEY));

    mvwaddstr(stdscr, y++, x, tui_node_info_type[TUI_NODE_INFO_TOTAL_MEMORY]);
    attron(A_BOLD | COLOR_PAIR(TUI_COLOR_HELP_KEY));
    waddch(stdscr, ' ');
    waddstr(stdscr, tui->node_data[TUI_NODE_INFO_DOMAINS_MEMORY]);
    waddstr(stdscr, "/");
    waddstr(stdscr, tui->node_data[TUI_NODE_INFO_TOTAL_MEMORY]);
    waddstr(stdscr, "MB");
    attroff(A_BOLD | COLOR_PAIR(TUI_COLOR_HELP_KEY));
}
