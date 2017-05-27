/* This file containes routines to connect to libvirt nodes, extracting 
 * data and manipulating domains using libvirt API
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
#ifndef VIRT_NODE_H
#define VIRT_NODE_H
#include "virt.h"
/** Number of possible node data types */
#define VIRT_NODE_DATA_TYPE_SIZE (5)

/**
 * Indecies of the virt_node_data array.
 * @see virt_node_data
 */
typedef enum {
    VIRT_NODE_DATA_TYPE_HOSTNAME,
    VIRT_NODE_DATA_TYPE_URI,
    VIRT_NODE_DATA_TYPE_LIB_VERSION,
    VIRT_NODE_DATA_TYPE_TOTAL_MEMORY,
    VIRT_NODE_DATA_TYPE_DOMAIN_MEMORY
} virt_node_data_enum;

/** @see virt_node_data_enum */
typedef virt_node_data_enum node_type;

/** Structure representing current node data. */
typedef struct {
    /** Array containing node's data */
    char        *node_data[VIRT_NODE_DATA_TYPE_SIZE];
    /** Array containing current node data indecies */
    node_type   node_type[VIRT_NODE_DATA_TYPE_SIZE];
} virt_node_data;

/**
 * Initializes virt_node_data object to default values.
 * @param vdata - data to be initialized
 * @see virt_node_data
 */
void virt_init_node_data(void *vdata);

/**
 * Deinitializes virt_node_data object.
 * @param vdata - data to be freed
 * @see virt_init_node_data
 */
void virt_deinit_node_data(void *vdata);

/**
 * First deinitialize the virt_node_data object and then default init it.
 * @param vdata - data to be freed
 * @see virt_init_node_data
 */
void virt_reset_node_data(void *vdata);

/**
 * This function deinitializes the object and then sets it to default parameters.
 * @param vdata - data to be deinitialized and initialized again.
 * @see virt_init_node_data
 * @see virt_deinit_node_data
 */
virt_node_data virt_get_node_data(virt_data *virt);

#endif /* VIRT_NODE_H */
