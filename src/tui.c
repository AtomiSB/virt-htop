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
#include "tui.h"
#include "virt.h"
#include <stdlib.h>
#include <string.h>

int tui_column_width[TUI_DOMAIN_COLUMN_SIZE] = {
    4, 25, 12, 10, 7, 70
};

const char *tui_node_info_type[TUI_NODE_INFO_SIZE] = {
    "Hostname:",
    "     URI:",
    " Version:",
    "  Memory:",
    "  Memory:"
};

const char *tui_command_panel_keys[TUI_COMMAND_PANEL_SIZE] = {
    "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10"
};

const char *tui_command_panel_text[TUI_COMMAND_PANEL_SIZE] = {
    "HELP   ",
    "CONFIG ",
    "SEARCH ",
    "DEFINE ",
    "AUTO   ",
    "START  ",
    "PAUSE  ",
    "REBOOT ",
    "DESTROY",
    "QUIT   "
};

tui_help_keys_pair tui_help_keys[TUI_HELP_KEYS_SIZE] =
{
    {"Arrows j  k:", " Scroll list"},
    {"      F1  ?:", " Show this help screen"},
    {"      F5  a:", " Toggle autostart option"},
    {"      F6  s:", " Start, Resume"},
    {"      F7  p:", " Suspend"},
    {"      F8  r:", " Reboot"},
    {"      F9  d:", " Destroy"},
    {"      F10 q:", " Quit"}
};

void tui_init()
{
    initscr();              /* initialize screen */
    start_color();          /* enable colors */
    cbreak();               /* immediately return user input */
    noecho();               /* no user input echoing */
    curs_set(0);            /* hide cursor */
    keypad(stdscr, TRUE);   /* allow special key input */

    /* color pairs init */
    init_pair(TUI_COLOR_COMMAND_PANEL_KEY, COLOR_WHITE, COLOR_BLACK);
    init_pair(TUI_COLOR_COMMAND_PANEL_TEXT, COLOR_BLACK, COLOR_CYAN);
    init_pair(TUI_COLOR_COLUMN_HEADER_TEXT, COLOR_BLACK, COLOR_GREEN);
    init_pair(TUI_COLOR_HELP_KEY, COLOR_CYAN, COLOR_BLACK);
}

void tui_init_default(tui_data *tui)
{
    tui->node_memory_size    = NULL;
    for (int i = 0; i != TUI_NODE_INFO_SIZE; ++i) {
        tui->node_data[i] = NULL;
        tui->node_type[i] = i;
    }

    tui->domain_data = malloc(sizeof(tui_domain_data));
    tui_init_default_domain_columns(tui->domain_data);
}

void tui_deinit_node_data(tui_data *tui)
{
    for (int i = 0; i != TUI_NODE_INFO_SIZE; ++i)
        free(tui->node_data[i]);
    free(tui->domain_data->domain_memory_size);
}

void tui_deinit(tui_data *tui)
{
    /* unpost and free all the memory taken up */
    for (int i = 0; i != TUI_NODE_INFO_SIZE; ++i)
        free(tui->node_data[i]);

    tui_deinit_domain_columns(tui->domain_data);
    free(tui->domain_data);
}

void tui_reset(tui_data *tui)
{
    tui_deinit(tui);
    tui_init_default(tui);
}

void tui_draw_node_panel(tui_data *tui)
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

void tui_draw_command_panel()
{
    /* get current terminal size */
    int max_width = 0, max_height = 0;
    getmaxyx(stdscr, max_width, max_height);
    max_height = 0;

    /* draw the first key 'F1' */
    attron(COLOR_PAIR(TUI_COLOR_COMMAND_PANEL_KEY));
    mvprintw(max_width - 1, max_height, tui_command_panel_keys[F1]);
    attroff(COLOR_PAIR(TUI_COLOR_COMMAND_PANEL_KEY));

    attron(COLOR_PAIR(TUI_COLOR_COMMAND_PANEL_TEXT));
    printw(tui_command_panel_text[F1]);
    attroff(COLOR_PAIR(TUI_COLOR_COMMAND_PANEL_TEXT));

    /* draw the rest of the keys */
    for (int i = 1; i != TUI_COMMAND_PANEL_SIZE; ++i) {
        attron(COLOR_PAIR(TUI_COLOR_COMMAND_PANEL_KEY));
        printw(tui_command_panel_keys[i]);
        attroff(COLOR_PAIR(TUI_COLOR_COMMAND_PANEL_KEY));

        attron(COLOR_PAIR(TUI_COLOR_COMMAND_PANEL_TEXT));
        printw(tui_command_panel_text[i]);
        attroff(COLOR_PAIR(TUI_COLOR_COMMAND_PANEL_TEXT));
    }

    /* fill up the rest of the screen */
    int width = 0;
    getmaxyx(stdscr, max_height, max_width);
    getyx(stdscr, max_height, width);
    attron(COLOR_PAIR(TUI_COLOR_COMMAND_PANEL_TEXT));
    for (int i = 0; i != max_width - width; ++i)
        printw(" ");
    attroff(COLOR_PAIR(TUI_COLOR_COMMAND_PANEL_TEXT));
}

void tui_create_node_panel(tui_data *tui, virt_node_data *data)
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
    tui->node_data[TUI_NODE_INFO_DOMAINS_MEMORY] = tui->domain_data->domain_memory_size;

    /* set up default order */
    tui->node_type[0] = TUI_NODE_INFO_HOSTNAME;
    tui->node_type[1] = TUI_NODE_INFO_URI;
    tui->node_type[2] = TUI_NODE_INFO_LIB_VERSION;
    tui->node_type[3] = TUI_NODE_INFO_TOTAL_MEMORY;
    tui->node_type[4] = TUI_NODE_INFO_DOMAINS_MEMORY;
}

void tui_draw_all(tui_data *tui)
{
    tui_draw_node_panel(tui);
    tui_draw_column_header(tui);
    tui_draw_domain_columns(tui->domain_data);
    tui_draw_command_panel();
}

void tui_draw_help()
{
    timeout(-1); /* make input blocking */

    int y = 0;
    clear();
    attron(A_BOLD | COLOR_PAIR(TUI_COLOR_HELP_KEY));
    mvwaddstr(stdscr, y++, 0, "virt-htop ");
    waddstr(stdscr, VIRT_HTOP_VERSION);
    waddstr(stdscr, " Copyright (C) 2017 Atomi Sebastian Bialobrzecki");
    mvwaddstr(stdscr, y++, 0, "Released under GNU GPL v3 or later");
    attroff(A_BOLD | COLOR_PAIR(TUI_COLOR_HELP_KEY));
    /* draw pairs of key: desc */
    for (int i = 0; i != TUI_HELP_KEYS_SIZE; ++i, ++y) {
        attron(A_BOLD | COLOR_PAIR(TUI_COLOR_HELP_KEY));
        mvwaddstr(stdscr, y, 0, tui_help_keys[i].key);
        attroff(A_BOLD | COLOR_PAIR(TUI_COLOR_HELP_KEY));
        waddstr(stdscr, tui_help_keys[i].desc);
    }
    attron(A_BOLD | COLOR_PAIR(TUI_COLOR_HELP_KEY));
    mvwaddstr(stdscr, y, 0, "Press any key to return");
    attroff(A_BOLD | COLOR_PAIR(TUI_COLOR_HELP_KEY));

    getch();
    clear();
    timeout(TUI_INPUT_DELAY); /* make input non-blocking with expected timeout */
}
