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
#ifndef TUI_NODE_H
#define TUI_NODE_H
/** @file tui_node.h 
 * This file contains routines to draw terminal output using ncurses */
#include "tui.h"
#include "virt_node.h"
/** Size of array containing strings of node info */
#define TUI_NODE_INFO_SIZE (5)
/** Size of array containing strings of node summary */
#define TUI_NODE_INFO_SUMMARY_SIZE (6)

/** Represents node info type */
typedef enum {
    TUI_NODE_INFO_HOSTNAME = 0,
    TUI_NODE_INFO_URI,
    TUI_NODE_INFO_LIB_VERSION,
    TUI_NODE_INFO_TOTAL_MEMORY,
    TUI_NODE_INFO_DOMAINS_MEMORY,
} tui_node_panel_enum;

typedef tui_node_panel_enum tui_node_type;

/** Types of printable node info. */
const char *tui_node_info_type[TUI_NODE_INFO_SIZE];

/** TODO */
const char *tui_node_info_summary[TUI_NODE_INFO_SUMMARY_SIZE];

/**
 * Struct that holds node data.
 */
typedef struct tui_node_data {
    char            *node_data[TUI_NODE_INFO_SIZE]; /** Node data strings */
    tui_node_type   node_type[TUI_NODE_INFO_SIZE];  /** Keeps track of node info index position */

    char *node_memory_size;     /** RAM size of the node */
} tui_node_data;

/**
 * Set node data object to default state.
 * @param tui - pointer to the tui_node_data that draws on the screen
 */
void tui_init_default_node_data(tui_node_data *tui);

/**
 * Deinitialize the node data object.
 * @param tui - pointer to the tui_node_data that draws on the screen
 */
void tui_deinit_node_data(tui_node_data *tui);

/**
 * Copy the node information from data object to tui object.
 * @param tui   - pointer to the tui_node_data that draws on the screen
 * @param data  - pointer to data extracted from libvirt calls.
 */
void tui_create_node_panel(tui_node_data *tui, virt_node_data *data);

/**
 * Update domain memory data for tui_node_data object.
 * @param tui   - pointer to the tui_node_data that draws on the screen
 * @param data  - pointer to data extracted from libvirt calls.
 */
void tui_node_update_memory_data(tui_node_data *tui, virt_domain_data *data);

/**
 * Draw the node information at the top left side of the screen.
 * @param tui - pointer to the tui_node_data that draws on the screen
 */
void tui_draw_node_panel(tui_node_data *tui);

#endif /* TUI_NODE_H */
