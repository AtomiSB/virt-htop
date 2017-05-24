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
#ifndef TUI_H
#define TUI_H
/** @file tui.h 
 * This file contains routines to draw terminal output using ncurses */
#include <ncurses.h>
#include <menu.h>
#include "virt.h"
#include "tui_node.h"
#include "tui_domain.h"
/** Input delay between keystrokes in milliseconds */
#define TUI_INPUT_DELAY (50)
/** Time between screen refresh in seconds */
#define TUI_REFRESH_TIME (1.0)
/** Number of defined color pairs */
#define COLORS_SIZE (4)
/** Command panel's number of elements */
#define TUI_COMMAND_PANEL_SIZE (10)
/** Size of array containing pairs (key, desc) used in printing helpful information */
#define TUI_HELP_KEYS_SIZE (8)

/** Represents F(N) keys used for calling command panel's buttons */
typedef enum {
    TUI_COMMAND_KEY_HELP = 1,
    TUI_COMMAND_KEY_SETUP,
    TUI_COMMAND_KEY_SEARCH,
    TUI_COMMAND_KEY_DEFINE,
    TUI_COMMAND_KEY_AUTO,
    TUI_COMMAND_KEY_START,
    TUI_COMMAND_KEY_PAUSE,
    TUI_COMMAND_KEY_REBOOT,
    TUI_COMMAND_KEY_DESTROY,
    TUI_COMMAND_KEY_QUIT
} tui_command_key_enum;

/** Represents keys used for manipulating TUI */
typedef enum {
    TUI_KEY_LIST_DOWN         = 'j',
    TUI_KEY_LIST_UP           = 'k',
    TUI_KEY_COMMAND_HELP      = '?',
    TUI_KEY_COMMAND_AUTOSTART = 'a',
    TUI_KEY_COMMAND_START     = 's',
    TUI_KEY_COMMAND_PAUSE     = 'p',
    TUI_KEY_COMMAND_REBOOT    = 'r',
    TUI_KEY_COMMAND_DESTROY   = 'd',
    TUI_KEY_QUIT              = 'q'
} tui_keyboard_key_enum;

/** List of color pairs used in TUI. */
typedef enum {
    TUI_COLOR_COMMAND_PANEL_KEY = 1,    /** Command panel keys coloring */
    TUI_COLOR_COMMAND_PANEL_TEXT,       /** Command panel desc coloring */
    TUI_COLOR_COLUMN_HEADER_TEXT,       /** Column header text color */
    TUI_COLOR_HELP_KEY                  /** Helpful information coloring */
} tui_color_enum;

/**
 * Key shortcuts of the command panel buttons
 * used in the main loop.
 */
typedef enum {
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, 
} tui_command_panel_enum;

/** Pair of (key, description) used in printing helpful information. */
typedef struct {
    const char *key;    /** Key shortcut */
    const char *desc;   /** Key description */
} tui_help_keys_pair;

/** Help screen pairs containing (key, description). */
tui_help_keys_pair tui_help_keys[TUI_HELP_KEYS_SIZE];

/** Keys of command panel buttons. */
const char *tui_command_panel_keys[TUI_COMMAND_PANEL_SIZE];
/** Key description of command panel buttons. */
const char *tui_command_panel_text[TUI_COMMAND_PANEL_SIZE];

/**
 * Initialize TUI, this function needs to be called before 
 * any other tui functions.
 */
void tui_init();

/** Forward declaration of tui_node_data */
typedef struct tui_node_data tui_node_data;
/** Forward declaration of tui_domain_data */
typedef struct tui_domain_data tui_domain_data;

/** Represents domain columns */
typedef struct tui_data {
    tui_node_data   *node_data;
    tui_domain_data *domain_data;
} tui_data;

/**
 * Set tui object to default state.
 * @param tui - pointer to the tui_domain_data that draws on the screen
 */
void tui_init_default(tui_data *tui);

/**
 * Deinitialize the tui object.
 * @param tui - pointer to the tui_data that draws on the screen
 */
void tui_deinit(tui_data *tui);

/**
 * Deinitialize and set tui object to default state.
 * @see tui_init_default
 * @see tui_deinit
 * @param tui - pointer to the tui_data that draws on the screen
 */
void tui_reset(tui_data *tui);

/**
 * Draw command panel at the bottom of the screen.
 */
void tui_draw_command_panel();

/**
 * Draw all TUI, header, domain list and command panel.
 * @param tui - pointer to the tui_data that draws on the screen
 */
void tui_draw_all(tui_data *tui);

/**
 * Draw the help screen
 */
void tui_draw_help();

#endif // TUI_H
