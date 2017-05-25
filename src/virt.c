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
#include "virt.h"
#include "utils.h"
#include "tui.h"
#include <stdio.h>

void virt_error_function(void *userdata, virErrorPtr error)
{
    syslog(LOG_ERR, "Failure of libvirt library call:\n Code: %d, Domain: %d, Message: %s, Level: %d\n",
                    error->code, error->domain, error->message, error->level);
                     /* "str1: %s, str2: %s, str3: %s, int1: %d, int2: %d\n" */
                    /* err->str1 err->str2 err->str3 err->int1 error->int2);*/
}

void virt_setup()
{
    virSetErrorFunc(NULL, virt_error_function);
}

void virt_init_default(virt_data *virt)
{
    virt->domain        = NULL;
    virt->domain_size   = 0;
}

static void virt_free_domains(virt_data *virt)
{
    /* free allocated domains */
    for (int i = 0; i != virt->domain_size; ++i) 
        virDomainFree(virt->domain[i]);
    free(virt->domain);
}

void virt_deinit(virt_data *virt)
{
    virt_free_domains(virt);

    virConnectClose(virt->conn);
}

void virt_reset(virt_data *virt)
{
    virt_free_domains(virt);
    virt_init_default(virt);
}

virConnectPtr virt_connect_node(char **conn_args)
{
    virConnectPtr conn = NULL;

    if (conn_args) {
        /* check if system or session connection */
        if (strcmp(conn_args[0], CONNECTION_SYSTEM)  == 0 || 
            strcmp(conn_args[0], CONNECTION_SESSION) == 0)
            conn = virConnectOpen(conn_args[0]);
        else
            conn = virConnectOpenAuth(conn_args[0], virConnectAuthPtrDefault, 0);
    }
    return conn;
}
