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
#ifndef TUI_DOMAIN_H
#define TUI_DOMAIN_H
/** @file tui_domain.h 
 * This file contains routines to draw domain columns */
#include "tui.h"
/** Number of columns displayed in the middle of the screen */
#define TUI_DOMAIN_COLUMN_SIZE (6)
/** This is used as a column item description for filling up the selection color */
#define TUI_DOMAIN_COLUMN_SELECTOR ("                                ")
/** Size of the upper side of the screen (header) */
#define TUI_HEADER_HEIGHT (10)

/** Columns width */
int tui_column_width[TUI_DOMAIN_COLUMN_SIZE];
/** Pointer containing column header strings printed right above the columns. */
const char *tui_column_header[TUI_DOMAIN_COLUMN_SIZE];

/** Represents column type */
typedef enum {
    TUI_DOMAIN_COLUMN_ID,
    TUI_DOMAIN_COLUMN_NAME,
    TUI_DOMAIN_COLUMN_STATE,
    TUI_DOMAIN_COLUMN_AUTOSTART,
    TUI_DOMAIN_COLUMN_MEMORY_PRC,
    TUI_DOMAIN_COLUMN_REASON
} tui_domain_column_enum;

/**
 * Struct that holds domain data.
 */
typedef tui_domain_column_enum tui_domain_type;
typedef struct tui_domain_data {
    char    **domain_data[TUI_DOMAIN_COLUMN_SIZE];          /** Domain data strings */
    ITEM    **domain_data_item[TUI_DOMAIN_COLUMN_SIZE];     /** Domain column items */
    MENU    **domain_column;                                /** Holds domain item objects */
    WINDOW  *domain_columns_win;                            /** Holds domain_column */
    WINDOW  **domain_columns_sub_win;                       /** Pointer of subwindows holding columns */
    tui_domain_type domain_type[TUI_DOMAIN_COLUMN_SIZE];    /** Keeps track of domain index position */

    char *domain_memory_size;   /** RAM size of all domains */

    size_t domain_size;         /** Total number of domains */
} tui_domain_data;

/**
 * Set domain columns object to default state.
 * @param tui - pointer to the tui_domain_data that draws on the screen
 */
void tui_init_all_domain_columns(tui_domain_data *tui);

/**
 * Deinitialize the domain columns object.
 * @param tdata - pointer to the tui_domain_data that draws on the screen
 */
void tui_deinit_domain_columns(void *tdata);

/**
 * Draw column's header, right above it.
 */
void tui_draw_column_header();

/**
 * Create list of ITEM **, used by MENU * object (menu library).
 * @warning list of strings needs to end with the NULL object, this is a requirement 
 * by the menu library.
 * @param begin - pointer to the beginning of the list containing strings
 * @param end   - pointer to the end of the list containing strings
 * @return list of ITEM ** data used by MENU * object, NULL otherwise
 */
ITEM **tui_create_items(char **begin, char **end);

/**
 * Create string values for items in columns,
 * and copies other crucial data from the data object.
 * @param tui   - pointer to the tui_domain_data that draws on the screen
 * @param data  - pointer to data extracted from libvirt calls.
 */
void tui_create_columns(tui_domain_data *tui, virt_domain_data *data);

/**
 * Attach each column to the main window, and create subwindows
 * for each column. Attach subwindows to the main window
 * and format them with menu library API calls.
 * @param tui - pointer to the tui_domain_data that draws on the screen
 */
void tui_draw_domain_columns(tui_domain_data *tui);

#endif /* TUI_DOMAIN_H */
