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
#define VIRT_HTOP_VERSION ("0.1.2")
/** System connection */
#define CONNECTION_SYSTEM (":///system")
/** Session connection */
#define CONNECTION_SESSION (":///session")
/** Size of array containing function pointers to virt init functions */
#define VIRT_INIT_FUNCTION_SIZE (1)
/** Size of array containing function pointers to virt deinit functions */
#define VIRT_DEINIT_FUNCTION_SIZE (1)
/** Size of array containing function pointers to virt reset functions */
#define VIRT_RESET_FUNCTION_SIZE (1)
/** Size of array containing function pointers to virt get functions */
#define VIRT_GET_FUNCTION_SIZE (1)
/** Size of array containing function pointers to virt autostart functions */
#define VIRT_AUTOSTART_FUNCTION_SIZE (1)
/** Size of array containing function pointers to virt create functions */
#define VIRT_CREATE_FUNCTION_SIZE (1)
/** Size of array containing function pointers to virt pause functions */
#define VIRT_PAUSE_FUNCTION_SIZE (1)
/** Size of array containing function pointers to virt reboot functions */
#define VIRT_REBOOT_FUNCTION_SIZE (1)
/** Size of array containing function pointers to virt destroy functions */
#define VIRT_DESTROY_FUNCTION_SIZE (1)

/** List of virt errors */
typedef enum {
    VIRT_ERROR_FAILURE = -1,
    VIRT_ERROR_SUCCESS = 0
} virt_error_enum;

/** Libvirt's error callback function. */
void virt_error_function(void *userdata, virErrorPtr error);

/** Initiate libvirt, this function must be called before other virt_* functions. */
void virt_setup();

/**
 * Connect to target node
 * @param conn_args - Target domain URL with parameters
 * @return valid virConnectPtr, NULL otherwise
 */
virConnectPtr virt_connect_node(char **conn_args);

/** Handler to the libvirt's API. */
typedef struct {
    virConnectPtr   conn;           /** Connection pointer to target node */
    virDomainPtr    *domain;        /** Pointer to existing domains */
    size_t          domain_size;    /** Total number of existing domains */
} virt_data;

/**
 * Set all virt data's parameters to default values.
 * @param virt - Pointer with virt data
 * @see virt_data
 */
void virt_init_all(virt_data *virt);

/**
 * Deinitialize virt data's pointers.
 * @param virt - Pointer with virt data
 * @see virt_data
 */
void virt_deinit_all(virt_data *virt);

/**
 * Deinitialize virt data first and then set it's data to default values.
 * @param virt - Pointer with virt data
 * @see virt_deinit_all
 * @see virt_init_all
 * @see virt_data
 */
void virt_reset_all(virt_data *virt);

/*
 * Call the virt_autostart_domain function through virt_autostart
 * @param virt  - pointer with virt data
 * @param index - domain index
 * @see virt_autostart_domain
 * @see virt_autostart
 */
void virt_domain_autostart_wrapper(virt_data *virt, int index);

/*
 * Call the virt_create_domain function through virt_create
 * @param virt  - pointer with virt data
 * @param index - domain index
 * @see virt_create_domain
 * @see virt_create
 */
void virt_domain_create_wrapper(virt_data *virt, int index);

/*
 * Call the virt_pause_domain function through virt_pause
 * @param virt  - pointer with virt data
 * @param index - domain index
 * @see virt_pause_domain
 * @see virt_pause
 */
void virt_domain_pause_wrapper(virt_data *virt, int index);

/*
 * Call the virt_reboot_domain function through virt_reboot
 * @param virt  - pointer with virt data
 * @param index - domain index
 * @see virt_reboot_domain
 * @see virt_reboot
 */
void virt_domain_reboot_wrapper(virt_data *virt, int index);

/*
 * Call virt_destroy_domain function through virt_destroy
 * @param virt  - pointer with virt data
 * @param index - domain index
 * @see virt_destroy_domain
 * @see virt_destroy
 */
void virt_domain_destroy_wrapper(virt_data *virt, int index);

/** virt get functions */
typedef void *(*virt_get_function)(virt_data *virt);
virt_get_function virt_get[VIRT_GET_FUNCTION_SIZE];

/** virt autostart functions */
typedef void (*virt_autostart_function)(virt_data *virt, int index);
virt_autostart_function virt_autostart[VIRT_AUTOSTART_FUNCTION_SIZE];

/** virt create functions */
typedef void (*virt_create_function)(virt_data *virt, int index);
virt_create_function virt_create[VIRT_CREATE_FUNCTION_SIZE];

/** virt pause functions */
typedef void (*virt_pause_function)(virt_data *virt, int index);
virt_pause_function virt_pause[VIRT_PAUSE_FUNCTION_SIZE];

/** virt reboot functions */
typedef void (*virt_reboot_function)(virt_data *virt, int index);
virt_reboot_function virt_reboot[VIRT_REBOOT_FUNCTION_SIZE];

/** virt destroy functions */
typedef void (*virt_destroy_function)(virt_data *virt, int index);
virt_destroy_function virt_destroy[VIRT_DESTROY_FUNCTION_SIZE];

#endif /* VIRT_H */
