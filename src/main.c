/* 
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
#include <time.h>
#include "utils.h"
#include "tui.h"
#include "arguments.h"
#define LOG_FILE ("virt-htop.log")

int main_loop(virt_data *virt, tui_data *tui)
{
    tui_mode current_mode = TUI_MODE_DOMAIN;

    /* generate tui */
    tui_create[current_mode](tui, virt_get[current_mode](virt));
    /* get data from node */
    virt_node_data node_data = virt_get_node_data(virt);
    /* generate node panel */
    tui_create_node_panel(tui->node_data, &node_data);

    tui_draw[current_mode](tui);

    /* this index always points to the current selected item */
    size_t index = 0;
    /* prepare counter variable for drawing tui */
    time_t refresh_counter = 0;
    time(&refresh_counter);
    int user_input = 0;

    /* make input non-blocking
       with expected timeout*/
    timeout(TUI_INPUT_DELAY);

    keypad(tui->domain_data->domain_columns_win, TRUE);   /* allow special key input */
    int quit    = FALSE;
    int command = FALSE;
    while (quit != TRUE) {
        /* if user pushed button */
        if ((user_input = getch()) != ERR) {
            switch (user_input) {
                case KEY_F(TUI_COMMAND_KEY_QUIT): case TUI_KEY_QUIT: {
                    quit = TRUE;
                    break;
                }
                case TUI_KEY_MODE_ONE: {
                    current_mode = TUI_MODE_DOMAIN;
                    break;
                }
                case KEY_DOWN: case TUI_KEY_LIST_DOWN: {
                    command = TRUE;
                    tui_menu_driver[current_mode](tui, REQ_DOWN_ITEM);
                    index = tui_menu_index[current_mode](tui);
                    break;
                }
                case KEY_UP: case TUI_KEY_LIST_UP: {
                    command = TRUE;
                    tui_menu_driver[current_mode](tui, REQ_UP_ITEM);
                    index = tui_menu_index[current_mode](tui);
                    break;
                }
                case KEY_NPAGE: {
                    command = TRUE;
                    tui_menu_driver[current_mode](tui, REQ_SCR_DLINE);
                    index = tui_menu_index[current_mode](tui);
                    break;
                }
                case KEY_PPAGE: {
                    command = TRUE;
                    tui_menu_driver[current_mode](tui, REQ_SCR_ULINE);
                    index = tui_menu_index[current_mode](tui);
                    break;
                }
                case KEY_F(TUI_COMMAND_KEY_HELP): 
                case TUI_KEY_COMMAND_HELP: {
                    command = TRUE;
                    tui_draw_help();
                    break;
                }
                case KEY_F(TUI_COMMAND_KEY_AUTO): 
                case TUI_KEY_COMMAND_AUTOSTART: {
                    command = TRUE;
                    index = tui_menu_index[current_mode](tui);
                    virt_autostart[current_mode](virt, index);
                    break;
                }
                case KEY_F(TUI_COMMAND_KEY_START): 
                case TUI_KEY_COMMAND_START: {
                    command = TRUE;
                    index = tui_menu_index[current_mode](tui);
                    virt_create[current_mode](virt, index);
                    break;
                }
                case KEY_F(TUI_COMMAND_KEY_PAUSE): 
                case TUI_KEY_COMMAND_PAUSE: {
                    command = TRUE;
                    index = tui_menu_index[current_mode](tui);
                    virt_pause[current_mode](virt, index);
                    break;
                }
                case KEY_F(TUI_COMMAND_KEY_REBOOT): 
                case TUI_KEY_COMMAND_REBOOT: {
                    command = TRUE;
                    index = tui_menu_index[current_mode](tui);
                    virt_reboot[current_mode](virt, index);
                    break;
                }
                case KEY_F(TUI_COMMAND_KEY_DESTROY): 
                case TUI_KEY_COMMAND_DESTROY: {
                    command = TRUE;
                    index = tui_menu_index[current_mode](tui);
                    virt_destroy[current_mode](virt, index);
                    break;
                }
            }
        }
        /* check if its time to refresh the screen */
        if (command == TRUE || difftime(time(NULL), refresh_counter) >= TUI_REFRESH_TIME) {
            /* clear the screen*/
            clear();

            /* reset tui and virt data*/
            tui_reset[current_mode](tui);
            tui_reset_node(tui);
            virt_reset_all(virt);

            /* generate tui */
            tui_create[current_mode](tui, virt_get[current_mode](virt));

            /* get data from node*/
            node_data = virt_get_node_data(virt);
            tui_create_node_panel(tui->node_data, &node_data);

            tui_draw[current_mode](tui);

            /* set index for each column */
            tui_menu_set_index[current_mode](tui, index);

            refresh();

            /* restart the counter */
            time(&refresh_counter);
            command = FALSE;
        }
        wrefresh(tui->domain_data->domain_columns_win);
    }
    return 0;
}

int main(int argc, const char **argv)
{
    openlog(LOG_FILE, LOG_PID, LOG_USER);

    if (argc < 2) {
        print_usage();
        return 0;
    }

    /* help argument */
    char **help = parser_find_option(argv+1, argv+argc, HELP_SHORT);
    if (!help)
        help = parser_find_option(argv+1, argv+argc, HELP_LONG);
    if (help) {
        print_help();
        return 0;
    }

    /* get connection arguments */
    char **conn_args = parser_find_option(argv+1, argv+argc, CONNECT_SHORT);
    if (!conn_args)
        conn_args = parser_find_option(argv+1, argv+argc, CONNECT_LONG);

    if (!conn_args) {
        print_usage();
        return 1;
    }
    
    /* initialize libvirt */
    virt_setup();
    
    /* data associated with libvirt */
    virt_data virt;
    virt_init_all(&virt);

    /* connect to the node */
    virt.conn = virt_connect_node(conn_args);

    if (virt.conn == NULL) {
        fprintf(stderr, "Failed to open connection\n");
        return 1;
    }

    /* initialize ncurses library routines */
    tui_init_global();

    /* data associated with TUI */
    tui_data tui;
    tui_init_all(&tui);

    int res = main_loop(&virt, &tui);

    /* deinit data */
    endwin();
    tui_deinit_all(&tui);
    virt_deinit_all(&virt);
    free_pointer_char(conn_args, conn_args + options_count[CONNECT_SHORT]);

    closelog();

    return res;
}
