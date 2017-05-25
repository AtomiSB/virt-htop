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
#ifndef VIRT_H
#define VIRT_H
#include <stdlib.h>
#include <libvirt/libvirt.h>
#include <libvirt/virterror.h>
#include <syslog.h>
/** Extract libvirt's version number macros */
#define LIB_MAJOR_VERSION(x) (x / 1000000)
#define LIB_MINOR_VERSION(x) ((x - (LIB_MAJOR_VERSION(x) * 1000000)) / 1000)
#define LIB_VERSION(x)       (LIB_MAJOR_VERSION(x) + (LIB_MINOR_VERSION(x) * 0.1))
/** Version number of virt-htop */
#define VIRT_HTOP_VERSION ("0.1.0")
/** System connection */
#define CONNECTION_SYSTEM (":///system")
/** Session connection */
#define CONNECTION_SESSION (":///session")
/** Number of possible node data types */
#define VIRT_NODE_DATA_TYPE_SIZE (4)

/** List of virt errors */
typedef enum {
    VIRT_ERROR_FAILURE = -1,
    VIRT_ERROR_SUCCESS = 0
} virt_error_enum;

/** Libvirt's error callback function. */
void virt_error_function(void *userdata, virErrorPtr error);

/** Initiate libvirt, this function must be called before other virt_* functions. */
void virt_setup();

/** Handler to the libvirt's API. */
typedef struct {
    virConnectPtr   conn;           /** Connection pointer to target node */
    virDomainPtr    *domain;        /** Pointer to existing domains */
    size_t          domain_size;    /** Total number of existing domains */
} virt_data;

/**
 * Indecies of the virt_node_data array.
 * @see virt_node_data
 */
typedef enum {
    VIRT_NODE_DATA_TYPE_HOSTNAME,
    VIRT_NODE_DATA_TYPE_URI,
    VIRT_NODE_DATA_TYPE_LIB_VERSION,
    VIRT_NODE_DATA_TYPE_TOTAL_MEMORY
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
 * Set all virt data's parameters to default values.
 * @param virt - Pointer with virt data
 * @see virt_data
 */
void virt_init_default(virt_data *virt);

/**
 * Deinitialize virt data's pointers.
 * @param virt - Pointer with virt data
 * @see virt_data
 */
void virt_deinit(virt_data *virt);

/**
 * Deinitialize virt data first and then set it's data to default values.
 * @param virt - Pointer with virt data
 * @see virt_deinit
 * @see virt_init_default
 * @see virt_data
 */
void virt_reset(virt_data *virt);

/**
 * Initializes virt_node_data object to default values.
 * @param data - data to be initialized
 * @see virt_node_data
 */
void virt_init_node_default(virt_node_data *data);

/**
 * Deinitializes virt_node_data object.
 * @param data - data to be freed
 * @see virt_init_node_default
 */
void virt_deinit_node_default(virt_node_data *data);

/**
 * This function deinitializes the object and then sets it to default parameters.
 * @param virt - data to be deinitialized and initialized again.
 * @see virt_init_node_default
 * @see virt_deinit_node_default
 */
virt_node_data virt_node_collect_data(virt_data *virt);

#endif /* VIRT_H */
