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
#ifndef VIRT_DOMAIN_H
#define VIRT_DOMAIN_H
#include "virt.h"
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
 * Set domain data to default state.
 * @param vdata - Object to be filled with domains data
 * @see virt_deinit_domain
 */
void virt_init_domain_data(void *vdata);

/**
 * Deinitialize the domain data object.
 * @param vdata - Object filled with domains data
 * @see virt_init_domain_default
 */
void virt_deinit_domain_data(void *vdata);

/**
 * First deinitialize the domain data object and then default init it.
 * @param vdata - Object filled with domains data
 * @see virt_init_domain_data
 * @see virt_deinit_domain_data
 */
void virt_reset_domain_data(void *vdata);

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

#endif /* VIRT_DOMAIN_H */
