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

const char *tui_node_info_summary[TUI_NODE_INFO_SUMMARY_SIZE] = {
    "  domains:",
    "   active:",
    "  running:",
    "suspended:",
    "   paused:",
    " inactive:"
};

const char *tui_column_header[TUI_DOMAIN_COLUMN_SIZE] = {
    "ID  ",
    "DOMAIN",
    "STATE",
    "AUTOSTART",
    "MEM(%)",
    "REASON"
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
    {"Arrows j  k:", " Scroll domain list"},
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

void tui_init_default_domain_columns(tui_data *tui)
{
    for (int i = 0; i != TUI_DOMAIN_COLUMN_SIZE; ++i) {
        tui->domain_data[i] = NULL;
        tui->domain_data_item[i] = NULL;
        tui->domain_type[i] = i;
    }
    
    tui->domain_column       = NULL;

    tui->domain_columns_win  = NULL;
    tui->domain_columns_sub_win  = NULL;

    tui->domain_memory_size  = NULL;
    tui->node_memory_size    = NULL;

    tui->domain_size = 0;
}

void tui_init_default(tui_data *tui)
{
    for (int i = 0; i != TUI_NODE_INFO_SIZE; ++i) {
        tui->node_data[i] = NULL;
        tui->node_type[i] = i;
    }

    tui_init_default_domain_columns(tui);
}

void tui_deinit_node_data(tui_data *tui)
{
    for (int i = 0; i != TUI_NODE_INFO_SIZE; ++i)
        free(tui->node_data[i]);
    free(tui->domain_memory_size);
}

void tui_deinit_domain_columns(tui_data *tui)
{
    /* free columns */
    if (tui->domain_column) {
        for (int i = 0; i != TUI_DOMAIN_COLUMN_SIZE; ++i) {
            if (tui->domain_column[i]) {
                unpost_menu(tui->domain_column[i]);
                free_menu(tui->domain_column[i]);
            }
        }
    }
    free(tui->domain_column);

    /* free items */
    for (int i = 0; i != TUI_DOMAIN_COLUMN_SIZE; ++i) {
        for (int j = 0; j != tui->domain_size; ++j)
            if (tui->domain_data_item[i][j])
                free_item(tui->domain_data_item[i][j]);
        free(tui->domain_data_item[i]);
    }

    /* free strings */
    for (int i = 0; i != TUI_DOMAIN_COLUMN_SIZE; ++i) {
        for (int j = 0; j != tui->domain_size; ++j)
            free(tui->domain_data[i][j]);
        free(tui->domain_data[i]);
    }
    if (tui->domain_columns_win) {
        for (int i = 0; i != TUI_DOMAIN_COLUMN_SIZE; ++i) 
            delwin(tui->domain_columns_sub_win[i]);
        free(tui->domain_columns_sub_win);
        delwin(tui->domain_columns_win);
    }
}

void tui_deinit(tui_data *tui)
{
    /* unpost and free all the memory taken up */
    for (int i = 0; i != TUI_NODE_INFO_SIZE; ++i)
        free(tui->node_data[i]);

    tui_deinit_domain_columns(tui);
}

void tui_reset(tui_data *tui)
{
    tui_deinit(tui);
    tui_init_default(tui);
}

ITEM **tui_create_items(char **begin, char **end)
{
    size_t dist = end - begin;

    ITEM **items = (ITEM **)calloc(dist, sizeof(ITEM *));

    for (int i = 0; i != dist; ++i)
        items[i] = new_item(begin[i], TUI_DOMAIN_COLUMN_SELECTOR);

    return items;
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

void tui_draw_column_header(tui_data *tui)
{
    int x   = 0;
    int y   = TUI_HEADER_HEIGHT-1;
    move(y, x);

    attron(COLOR_PAIR(TUI_COLOR_COLUMN_HEADER_TEXT));

    /* first print n empty spaces where n is the width of i column,
     * then print i column header at (y, x) and move over the y*/
    for (int i = 0; i != TUI_DOMAIN_COLUMN_SIZE; ++i) {
        for (int j = 0; j != tui_column_width[tui->domain_type[i]]; ++j)
            printw(" ");
        mvprintw(y, x, tui_column_header[tui->domain_type[i]]);
        x += tui_column_width[tui->domain_type[i]];
    }

    /* fill up the rest of the screen */
    int max_x = 0;
    getmaxyx(stdscr, y, max_x);
    getyx(stdscr, y, x);
    for (int i = 0; i != max_x - x; ++i)
        printw(" ");
    attroff(COLOR_PAIR(TUI_COLOR_COLUMN_HEADER_TEXT));
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

void tui_draw_domain_columns(tui_data *tui)
{
    int x = 0, y = 0;
    /* length between previous and next printable column */
    size_t total_column_width = 0;
    getmaxyx(stdscr, x, y); y = 0;
    for (int i = 0; i != TUI_DOMAIN_COLUMN_SIZE; ++i) {
        set_menu_win(tui->domain_column[tui->domain_type[i]], tui->domain_columns_win);

        /* create subwindow that shows columns */
        tui->domain_columns_sub_win[tui->domain_type[i]] = 
            derwin( tui->domain_columns_win, 
                    x-TUI_HEADER_HEIGHT-1, 
                    tui_column_width[tui->domain_type[i]], 
                    y, 
                    total_column_width);

        /* update main window data before printing it out */
        touchwin(tui->domain_columns_win);
        set_menu_sub(   tui->domain_column[tui->domain_type[i]], 
                        tui->domain_columns_sub_win[tui->domain_type[i]]);
        /* set scrollable column up to x-TUI_HEADER_HEIGHT-1 elements in height 
           and 1 in width.*/
        set_menu_format(tui->domain_column[tui->domain_type[i]], x-TUI_HEADER_HEIGHT-1, 1);
        /* no column mark */
        set_menu_mark(tui->domain_column[tui->domain_type[i]], NULL);

        post_menu(tui->domain_column[tui->domain_type[i]]);

        total_column_width += tui_column_width[tui->domain_type[i]];
    }
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
    tui->node_data[TUI_NODE_INFO_DOMAINS_MEMORY] = tui->domain_memory_size;

    /* set up default order */
    tui->node_type[0] = TUI_NODE_INFO_HOSTNAME;
    tui->node_type[1] = TUI_NODE_INFO_URI;
    tui->node_type[2] = TUI_NODE_INFO_LIB_VERSION;
    tui->node_type[3] = TUI_NODE_INFO_TOTAL_MEMORY;
    tui->node_type[4] = TUI_NODE_INFO_DOMAINS_MEMORY;
}

void tui_create_columns(tui_data *tui, virt_domain_data *data)
{
    /* last item counts as NULL */
    tui->domain_size = data->domain_size;

    /* create pointer to menus 'domain columns' */
    tui->domain_column = calloc(TUI_DOMAIN_COLUMN_SIZE, sizeof(MENU *));

    /* copy domain pointers */
    for (int i = 0; i != TUI_DOMAIN_COLUMN_SIZE; ++i) {
        tui->domain_data[i]         = data->domain_data[i];
        tui->domain_data_item[i]    = tui_create_items(data->domain_data[i], data->domain_data[i] + tui->domain_size);
    }

    /* create columns */
    tui->domain_column[TUI_DOMAIN_COLUMN_ID]            = 
        new_menu((ITEM **)tui->domain_data_item[tui->domain_type[VIRT_DOMAIN_DATA_TYPE_ID]]);
    tui->domain_column[TUI_DOMAIN_COLUMN_NAME]          = 
        new_menu((ITEM **)tui->domain_data_item[tui->domain_type[VIRT_DOMAIN_DATA_TYPE_NAME]]);
    tui->domain_column[TUI_DOMAIN_COLUMN_STATE]         =
        new_menu((ITEM **)tui->domain_data_item[tui->domain_type[VIRT_DOMAIN_DATA_TYPE_STATE]]);
    tui->domain_column[TUI_DOMAIN_COLUMN_AUTOSTART]     =
        new_menu((ITEM **)tui->domain_data_item[tui->domain_type[VIRT_DOMAIN_DATA_TYPE_AUTOSTART]]);
    tui->domain_column[TUI_DOMAIN_COLUMN_MEMORY_PRC]    =
        new_menu((ITEM **)tui->domain_data_item[tui->domain_type[VIRT_DOMAIN_DATA_TYPE_MEMORY_PRC]]);
    tui->domain_column[TUI_DOMAIN_COLUMN_REASON]        =
        new_menu((ITEM **)tui->domain_data_item[tui->domain_type[VIRT_DOMAIN_DATA_TYPE_REASON]]);

    /* set up default order */
    tui->domain_type[0] = TUI_DOMAIN_COLUMN_ID;
    tui->domain_type[1] = TUI_DOMAIN_COLUMN_NAME;
    tui->domain_type[2] = TUI_DOMAIN_COLUMN_STATE;
    tui->domain_type[3] = TUI_DOMAIN_COLUMN_AUTOSTART;
    tui->domain_type[4] = TUI_DOMAIN_COLUMN_MEMORY_PRC;
    tui->domain_type[5] = TUI_DOMAIN_COLUMN_REASON;

    /* copy memory size */
    tui->domain_memory_size = data->domain_memory_size;

    int x = 0, y = 0;
    getmaxyx(stdscr, x, y);

    /* create the window to be associated with the menu */
    tui->domain_columns_win = newwin(x-TUI_HEADER_HEIGHT-1, y, TUI_HEADER_HEIGHT, 0);
    tui->domain_columns_sub_win = malloc(sizeof(WINDOW*) * TUI_DOMAIN_COLUMN_SIZE);
    keypad(tui->domain_columns_win, TRUE);
}

void tui_draw_all(tui_data *tui)
{
    tui_draw_node_panel(tui);
    tui_draw_column_header(tui);
    tui_draw_domain_columns(tui);
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
