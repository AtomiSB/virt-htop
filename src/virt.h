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
/** Number of possible domain data types */
#define VIRT_DOMAIN_DATA_TYPE_SIZE (6)
/** Number of possible domain states */
#define VIRT_STATE_TEXT_SIZE (9)
/** Number of domain statistics */
#define VIRT_DOMAIN_STATS (5)
/** String for unknown data display */
#define VIRT_DOMAIN_UNKNOWN_DATA ("-")
/** @see virt_domain_nostate_reason */
#define VIRT_DOMAIN_NOSTATE_SIZE        (1)
/** @see virt_domain_running_reason */
#define VIRT_DOMAIN_RUNNING_SIZE        (11)
/** @see virt_domain_blocked_reason */
#define VIRT_DOMAIN_BLOCKED_SIZE        (1)
/** @see virt_domain_paused_reason */
#define VIRT_DOMAIN_PAUSED_SIZE         (14)
/** @see virt_domain_shutdown_reason */
#define VIRT_DOMAIN_SHUTDOWN_SIZE       (2)
/** @see virt_domain_shutoff_reason */
#define VIRT_DOMAIN_SHUTOFF_SIZE        (8)
/** @see virt_domain_crashed_reason */
#define VIRT_DOMAIN_CRASHED_SIZE        (2)
/** @see virt_domain_pmsuspended_reason */
#define VIRT_DOMAIN_PMSUSPENDED_SIZE    (1)

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

/**
 * Indecies of the virt_domain_data array.
 * @see virt_domain_data
 */
typedef enum {
    VIRT_DOMAIN_DATA_TYPE_ID,
    VIRT_DOMAIN_DATA_TYPE_NAME,
    VIRT_DOMAIN_DATA_TYPE_STATE,
    VIRT_DOMAIN_DATA_TYPE_AUTOSTART,
    VIRT_DOMAIN_DATA_TYPE_MEMORY_PRC,
    VIRT_DOMAIN_DATA_TYPE_REASON
} virt_domain_data_type_enum;

/** @see virt_domain_data_type_enum */
typedef virt_domain_data_type_enum domain_type;

/**
 * Indecies of the virt_domain_status array.
 * @see virt_domain_status
 */
typedef enum {
    VIRT_DOMAIN_STATS_ACTIVE,
    VIRT_DOMAIN_STATS_RUNNING,
    VIRT_DOMAIN_STATS_PAUSED,
    VIRT_DOMAIN_STATS_SUSPENDED,
    VIRT_DOMAIN_STATS_INACTIVE
} virt_domain_stats_enum;

/** Structure holding data of all domains */
typedef struct {
    /** Arrays containing varous domain data */
    char **domain_data[VIRT_DOMAIN_DATA_TYPE_SIZE];  
    /** Holds sum of allocated memory by domains */
    char *domain_memory_size;                        
    /** Statistics of domain states */
    int  domain_stats[VIRT_DOMAIN_STATS];            
    /** Indecies for current domain type positions in domain_data array */
    domain_type domain_type[VIRT_DOMAIN_DATA_TYPE_SIZE];    
    /** Number of domains */
    size_t domain_size;
} virt_domain_data;

/** Virt's domain status check enums */
typedef enum {
    VIRT_DOMAIN_NOSTATE      = 0,    /** No state*/
    VIRT_DOMAIN_RUNNING      = 1,    /** The domain is running*/
    VIRT_DOMAIN_BLOCKED      = 2,    /** The domain is blocked on resource*/
    VIRT_DOMAIN_PAUSED       = 3,    /** The domain is paused by user*/
    VIRT_DOMAIN_SHUTDOWN     = 4,    /** The domain is being shut down*/
    VIRT_DOMAIN_SHUTOFF      = 5,    /** The domain is shut off*/
    VIRT_DOMAIN_CRASHED      = 6,    /** The domain is crashed*/
    VIRT_DOMAIN_PMSUSPENDED  = 7,    /** The domain is suspended by guest power management*/
    VIRT_DOMAIN_LAST         = 8     /** NB: this enum value will increase over time
                                     As new events are added to the libvirt API.
                                     It reflects the last state supported by
                                     this version of the libvirt API.*/
} virt_domain_state_enum;

/**
 * TODO
 * @param data  - virt object to be filled with new data
 * @param state - state of the domain
 */
void state_to_stats(virt_domain_data *data, int state);

/** Array of domain's possible states in strings */
const char *virt_domain_state_text[VIRT_STATE_TEXT_SIZE];

/** Array containing strigs of domain's nostate reasons */
const char *virt_domain_nostate_reason[VIRT_DOMAIN_NOSTATE_SIZE];
/** Array containing strigs of domain's running reasons */
const char *virt_domain_running_reason[VIRT_DOMAIN_RUNNING_SIZE];
/** Array containing strigs of domain's blocked reasons */
const char *virt_domain_blocked_reason[VIRT_DOMAIN_BLOCKED_SIZE];
/** Array containing strigs of domain's paused reasons */
const char *virt_domain_paused_reason[VIRT_DOMAIN_PAUSED_SIZE];
/** Array containing strigs of domain's shutdown reasons */
const char *virt_domain_shutdown_reason[VIRT_DOMAIN_SHUTDOWN_SIZE];
/** Array containing strigs of domain's shutoff reasons */
const char *virt_domain_shutoff_reason[VIRT_DOMAIN_SHUTOFF_SIZE];
/** Array containing strigs of domain's crashed reasons */
const char *virt_domain_crashed_reason[VIRT_DOMAIN_CRASHED_SIZE];
/** Array containing strigs of domain's pmsuspended reasons */
const char *virt_domain_pmsuspended_reason[VIRT_DOMAIN_PMSUSPENDED_SIZE];

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
 * Set all data to default state.
 * @param data - Object to be filled with domains data
 * @see virt_deinit_domain
 */
void virt_init_domain_default(virt_domain_data *data);

/**
 * Deinitialize the data object.
 * @param data - Object filled with domains data
 * @see virt_init_domain_default
 */
void virt_deinit_domain(virt_domain_data *data);

/**
 * First deinitialize the data object and then default init it.
 * @param data - Object filled with domains data
 * @see virt_init_domain_default
 * @see virt_deinit_domain
 */
void virt_reset_domain(virt_domain_data *data);

/**
 * Connect to target node
 * @param conn_args - Target domain URL with parameters
 * @return valid virConnectPtr, NULL otherwise
 */
virConnectPtr virt_connect_node(char **conn_args);

/**
 * Gather all the information about domains states.
 * @param virt - Handler to the libvirt connection
 * @param data - Object to be filled with domains states data.
 * @return object filled with domain states data, NULL otherwise
 */
void virt_domain_collect_state_data(virt_data *virt, virt_domain_data *data);

/**
 * Gather all the information about domains memory.
 * @param virt - Handler to the libvirt connection
 * @param data - Object to be filled with domains memory data.
 * @return object filled with domain memory data, NULL otherwise
 */
void virt_domain_collect_memory_data(virt_data *virt, virt_domain_data *data);

/**
 * Gather all the information about domains.
 * @param virt - Handler to the libvirt connection
 * @return object filled with domain data, NULL otherwise
 * @see virt_domain_collect_memory_data
 * @see virt_domain_collect_state_data
 */
virt_domain_data virt_domain_collect_data(virt_data *virt);

/**
 * Set domain's autostart on/off
 * @param domain - Target domain to be autostarted
 * @return VIRT_ERROR_SUCCESS on success, VIRT_ERROR_FAILURE otherwise
 */
int virt_domain_autostart(virDomainPtr domain);

/**
 * If domain is shut off, start it.
 * If domain is suspended, resume it.
 * @param domain - Pointer to target domain
 * @return VIRT_ERROR_SUCCESS on success, VIRT_ERROR_FAILURE otherwise
 */
int virt_domain_create(virDomainPtr     domain);

/**
 * If domain is running, pause it.
 * @param domain - Pointer to target domain
 * @return VIRT_ERROR_SUCCESS on success, VIRT_ERROR_FAILURE otherwise
 */
int virt_domain_pause(virDomainPtr      domain);

/**
 * If domain is running, reboot it.
 * @param domain - Pointer to target domain
 * @return VIRT_ERROR_SUCCESS on success, VIRT_ERROR_FAILURE otherwise
 */
int virt_domain_reboot(virDomainPtr     domain);

/**
 * If domain is running destroy it.
 * @param domain - Pointer to target domain
 * @return VIRT_ERROR_SUCCESS on success, VIRT_ERROR_FAILURE otherwise
 */
int virt_domain_destroy(virDomainPtr    domain);

#endif /* VIRT_H */
