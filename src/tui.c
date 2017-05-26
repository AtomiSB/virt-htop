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

tui_help_keys_pair tui_help_keys[TUI_HELP_KEYS_SIZE] = {
    {"Arrows j  k:", " Scroll list"},
    {"      F1  ?:", " Show this help screen"},
    {"      F5  a:", " Toggle autostart option"},
    {"      F6  s:", " Start, Resume"},
    {"      F7  p:", " Suspend"},
    {"      F8  r:", " Reboot"},
    {"      F9  d:", " Destroy"},
    {"      F10 q:", " Quit"}
};

void tui_init_global()
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

void tui_init_all(tui_data *tui)
{
    for (int i = 0; i != TUI_INIT_FUNCTION_SIZE; ++i) 
        tui_init[i](tui);
    tui_init_node(tui);
}

void tui_init_domain(void *tdata)
{
    tui_data *tui = (tui_data *)tdata;

    tui->domain_data    = malloc(sizeof(tui_domain_data));
    tui_init_all_domain_columns(tui->domain_data);
}

void tui_init_node(void *tdata)
{
    tui_data *tui = (tui_data *)tdata;

    tui->node_data      = malloc(sizeof(tui_node_data));
    tui_init_all_node_data(tui->node_data);
}

void tui_deinit_all(tui_data *tui)
{
    for (int i = 0; i != TUI_DEINIT_FUNCTION_SIZE; ++i) 
        tui_deinit[i](tui);
    tui_deinit_node(tui);
}

void tui_deinit_node(void *tdata)
{
    tui_data *tui = (tui_data *)tdata;

    tui_deinit_node_data(tui->node_data);
    free(tui->node_data);
}

void tui_deinit_domain(void *tdata)
{
    tui_data *tui = (tui_data *)tdata;

    tui_deinit_domain_columns(tui->domain_data);
    free(tui->domain_data);
}

void tui_reset_all(tui_data *tui)
{
    tui_deinit_all(tui);
    tui_init_all(tui);
}

void tui_reset_domain(void *tdata)
{
    tui_deinit_domain(tdata);
    tui_init_domain(tdata);
}

void tui_reset_node(void *tdata)
{
    tui_deinit_node(tdata);
    tui_init_node(tdata);
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

void tui_draw_domains(void *tdata)
{
    tui_data *tui = (tui_data *)tdata;

    tui_draw_node_panel(tui->node_data);
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

void tui_menu_driver_domain(void *tdata, int type)
{
    tui_data *tui = (tui_data*)tdata;
    for (int i = 0; i != TUI_DOMAIN_COLUMN_SIZE; ++i)
        menu_driver(tui->domain_data->domain_column[i], type);
}

int tui_menu_index_domain(void *tdata)
{
    tui_data *tui = (tui_data*)tdata;
    return item_index(current_item(tui->domain_data->domain_column[0]));
}

void tui_menu_set_index_domain(void *tdata, int index)
{
    tui_data *tui = (tui_data*)tdata;
    if (tui->domain_data->domain_size > 1)
        for (int i = 0; i != TUI_DOMAIN_COLUMN_SIZE; ++i)
            set_current_item(   tui->domain_data->domain_column[i], 
                                tui->domain_data->domain_column[i]->items[index]);
}

tui_init_function tui_init[TUI_INIT_FUNCTION_SIZE] = {
    tui_init_domain,
};

tui_deinit_function tui_deinit[TUI_DEINIT_FUNCTION_SIZE] = {
    tui_deinit_domain,
};

tui_reset_function tui_reset[TUI_RESET_FUNCTION_SIZE] = {
    tui_reset_domain,
};

tui_create_function tui_create[TUI_CREATE_FUNCTION_SIZE] = {
    tui_create_domain
};

tui_draw_function tui_draw[TUI_DRAW_FUNCTION_SIZE] = {
    tui_draw_domains
};

tui_menu_driver_function tui_menu_driver[TUI_MENU_DRIVER_FUNCTION_SIZE] = {
    tui_menu_driver_domain
};

tui_menu_index_function tui_menu_index[TUI_MENU_INDEX_FUNCTION_SIZE] = {
    tui_menu_index_domain
};

tui_menu_set_index_function tui_menu_set_index[TUI_MENU_SET_INDEX_FUNCTION_SIZE] = {
    tui_menu_set_index_domain
};
