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
#include "virt_domain.h"
#include "utils.h"

const char *virt_domain_state_text[VIRT_STATE_TEXT_SIZE] = {
    "unknown     ",
    "running     ",
    "blocked     ",
    "paused      ",
    "shutdown    ",
    "shutoff     ",
    "crashed     ",
    "pmsuspended "
};

const char *virt_domain_nostate_reason[VIRT_DOMAIN_NOSTATE_SIZE] = {
    "the reason is unknown"
};

const char *virt_domain_running_reason[VIRT_DOMAIN_RUNNING_SIZE] = {
    "the reason is unknown",
    "normal startup from boot",
    "migrated from another host",
    "restored from a state file",
    "restored from snapshot",
    "returned from paused state",
    "returned from migration",
    "returned from failed save process",
    "returned from pmsuspended due to wakeup event",
    "resumed from crashed",
    "running in post-copy migration mode"
};

const char *virt_domain_blocked_reason[VIRT_DOMAIN_BLOCKED_SIZE] = {
    "the reason is unknown"
};

const char *virt_domain_paused_reason[VIRT_DOMAIN_PAUSED_SIZE] = {
    "the reason is unknown",
    "paused on user request",
    "paused for offline migration",
    "paused for save",
    "paused for offline core dump",
    "paused due to a disk I/O error",
    "paused due to a watchdog event",
    "paused after restoring from snapshot",
    "paused during shutdown process",
    "paused while creating a snapshot",
    "paused due to a guest crash",
    "the domain is being started",
    "paused for post-copy migration",
    "paused after failed post-copy"
};

const char *virt_domain_shutdown_reason[VIRT_DOMAIN_SHUTDOWN_SIZE] = {
    "the reason is unknown",
    "shutting down on user request"
};

const char *virt_domain_shutoff_reason[VIRT_DOMAIN_SHUTOFF_SIZE] = {
    "the reason is unknown",
    "normal shutdown",
    "forced poweroff",
    "domain crashed",
    "migrated to another host",
    "saved to a file",
    "domain failed to start",
    "restored from a snapshot which was taken while domain was shutoff"
};

const char *virt_domain_crashed_reason[VIRT_DOMAIN_CRASHED_SIZE] = {
    "crashed for unknown reason",
    "domain panicked"
};

const char *virt_domain_pmsuspended_reason[VIRT_DOMAIN_PMSUSPENDED_SIZE] = {
    "the reason is unknown"
};

void virt_init_domain_data(void *vdata)
{
    virt_domain_data *data = (virt_domain_data *)vdata;

    for (int i = 0; i != VIRT_DOMAIN_DATA_TYPE_SIZE; ++i) {
        data->domain_data[i] = NULL;
        data->domain_type[i] = i;
    }
    
    data->domain_stats[0]   = 0;
    data->domain_size       = 0;
}

void virt_deinit_domain_data(void *vdata)
{
    virt_domain_data *data = (virt_domain_data *)vdata;

    for (int i = 0; i != VIRT_DOMAIN_DATA_TYPE_SIZE; ++i) 
        free_pointer_char(data->domain_data[i], data->domain_data[i] + data->domain_size);
}

void virt_reset_domain(void *vdata)
{
    virt_deinit_domain_data(vdata);
    virt_init_domain_data(vdata);
}

void state_to_stats(virt_domain_data *data, int state)
{
    for (int i = 0; i != data->domain_size; ++i) {
        switch (state) {
            case VIRT_DOMAIN_NOSTATE:
                ++data->domain_stats[VIRT_DOMAIN_STATS_INACTIVE];
                return;
            case VIRT_DOMAIN_RUNNING: 
                ++data->domain_stats[VIRT_DOMAIN_STATS_ACTIVE];
                ++data->domain_stats[VIRT_DOMAIN_STATS_RUNNING];
                return;
            case VIRT_DOMAIN_BLOCKED:
                ++data->domain_stats[VIRT_DOMAIN_STATS_ACTIVE];
                return;
            case VIRT_DOMAIN_PAUSED:
                ++data->domain_stats[VIRT_DOMAIN_STATS_ACTIVE];
                ++data->domain_stats[VIRT_DOMAIN_STATS_PAUSED];
                return;
            case VIRT_DOMAIN_SHUTDOWN:
                ++data->domain_stats[VIRT_DOMAIN_STATS_INACTIVE];
                return;
            case VIRT_DOMAIN_SHUTOFF:
                ++data->domain_stats[VIRT_DOMAIN_STATS_INACTIVE];
                return;
            case VIRT_DOMAIN_CRASHED:
                ++data->domain_stats[VIRT_DOMAIN_STATS_INACTIVE];
                return;
            case VIRT_DOMAIN_PMSUSPENDED:
                ++data->domain_stats[VIRT_DOMAIN_STATS_ACTIVE];
                ++data->domain_stats[VIRT_DOMAIN_STATS_SUSPENDED];
                return;
        }
    }
}

void virt_get_domain_state_data(virt_data *virt, virt_domain_data *data)
{
    int state       = 0;
    int reason      = 0;
    for (int i = 0; i != virt->domain_size; ++i) {
        virDomainGetState(virt->domain[i], &state, &reason, 0);

        data->domain_data[VIRT_DOMAIN_DATA_TYPE_STATE][i]    = copy_str(virt_domain_state_text[state]);

        switch (state) {
            case VIR_DOMAIN_NOSTATE:     
                data->domain_data[VIRT_DOMAIN_DATA_TYPE_REASON][i] = copy_str(virt_domain_nostate_reason[reason]);
                break;
            case VIR_DOMAIN_RUNNING:    
                data->domain_data[VIRT_DOMAIN_DATA_TYPE_REASON][i] = copy_str(virt_domain_running_reason[reason]);
                break;
            case VIR_DOMAIN_BLOCKED:     
                data->domain_data[VIRT_DOMAIN_DATA_TYPE_REASON][i] = copy_str(virt_domain_blocked_reason[reason]);
                break;
            case VIR_DOMAIN_PAUSED:      
                data->domain_data[VIRT_DOMAIN_DATA_TYPE_REASON][i] = copy_str(virt_domain_paused_reason[reason]);
                break;
            case VIR_DOMAIN_SHUTDOWN:    
                data->domain_data[VIRT_DOMAIN_DATA_TYPE_REASON][i] = copy_str(virt_domain_shutdown_reason[reason]);
                break;
            case VIR_DOMAIN_SHUTOFF:     
                data->domain_data[VIRT_DOMAIN_DATA_TYPE_REASON][i] = copy_str(virt_domain_shutoff_reason[reason]);
                break;
            case VIR_DOMAIN_CRASHED:     
                data->domain_data[VIRT_DOMAIN_DATA_TYPE_REASON][i] = copy_str(virt_domain_crashed_reason[reason]);
                break;
            case VIR_DOMAIN_PMSUSPENDED: 
                data->domain_data[VIRT_DOMAIN_DATA_TYPE_REASON][i] = copy_str(virt_domain_pmsuspended_reason[reason]);
                break;
        }
    }
}

void virt_get_domain_memory_data(virt_data *virt, virt_domain_data *data)
{
    for (int i = 0; i != virt->domain_size; ++i) {
        /* get all memory statistics for each guest */
        virDomainMemoryStatStruct mem_stats[VIR_DOMAIN_MEMORY_STAT_NR];
        int mem_count = virDomainMemoryStats(virt->domain[i], mem_stats, VIR_DOMAIN_MEMORY_STAT_NR, 0);

        double mem_max  = 0;
        double mem_curr = 0;
        /*double mem_swap = 0;*/
        if (mem_count > 0) {
            for (int i = 0; i != mem_count; ++i) {
                switch (mem_stats[i].tag) {
                    case VIR_DOMAIN_MEMORY_STAT_SWAP_IN:        break;/*mem_swap    = mem_stats[i].val; break;*/
                    case VIR_DOMAIN_MEMORY_STAT_SWAP_OUT:       break;
                    case VIR_DOMAIN_MEMORY_STAT_MAJOR_FAULT:    break;
                    case VIR_DOMAIN_MEMORY_STAT_MINOR_FAULT:    break;
                    case VIR_DOMAIN_MEMORY_STAT_UNUSED:         break;
                    case VIR_DOMAIN_MEMORY_STAT_AVAILABLE:      break;
                    case VIR_DOMAIN_MEMORY_STAT_ACTUAL_BALLOON: mem_max     = mem_stats[i].val; break;
                    case VIR_DOMAIN_MEMORY_STAT_RSS:            mem_curr    = mem_stats[i].val; break;
                    case VIR_DOMAIN_MEMORY_STAT_USABLE:         break;
                    case VIR_DOMAIN_MEMORY_STAT_LAST_UPDATE:    break;
                }
            }
        }
        /* calculate memory usage % for each guest */
        double mem_usage_prc = 0;
        if (mem_max > 0) {
            mem_usage_prc = ((mem_curr) * 100) / mem_max;
            data->domain_data[VIRT_DOMAIN_DATA_TYPE_MEMORY_PRC][i] = double_to_str(mem_usage_prc);
        } else
            data->domain_data[VIRT_DOMAIN_DATA_TYPE_MEMORY_PRC][i] = copy_str(VIRT_DOMAIN_UNKNOWN_DATA);
    }
}

void *virt_get_domain_data(void *vdata)
{
    virt_data *virt = (virt_data *)vdata;

    /* get defined domains */
    virt->domain_size = virConnectListAllDomains(virt->conn, &virt->domain, 0);

    virt_domain_data *data = malloc(sizeof(virt_domain_data));
    virt_init_domain_data(data);

    if (virt->domain_size < 0)
        return data;

    data->domain_size = virt->domain_size;

    /* get libvirt data */
    for (int i = 0; i != VIRT_DOMAIN_DATA_TYPE_SIZE; ++i) 
        data->domain_data[i] = calloc(virt->domain_size+1, sizeof(char *));

    int autostart   = 0;
    int id          = 0;
    int type        = 0;

    /* get state info data */
    data->domain_type[type++] = VIRT_DOMAIN_DATA_TYPE_STATE;
    virt_get_domain_state_data(virt, data);

    /* get memory info data */
    data->domain_type[type++] = VIRT_DOMAIN_DATA_TYPE_MEMORY_PRC;
    virt_get_domain_memory_data(virt, data);

    data->domain_type[type++] = VIRT_DOMAIN_DATA_TYPE_ID;
    data->domain_type[type++] = VIRT_DOMAIN_DATA_TYPE_NAME;
    data->domain_type[type++] = VIRT_DOMAIN_DATA_TYPE_AUTOSTART;
    for (int i = 0; i != virt->domain_size; ++i) {
        id = virDomainGetID(virt->domain[i]);
        data->domain_data[VIRT_DOMAIN_DATA_TYPE_ID][i]       = id > 0 ? int_to_str(id) : copy_str(VIRT_DOMAIN_UNKNOWN_DATA);

        data->domain_data[VIRT_DOMAIN_DATA_TYPE_NAME][i]     = copy_str(virDomainGetName(virt->domain[i]));

        virDomainGetAutostart(virt->domain[i], &autostart);
        data->domain_data[VIRT_DOMAIN_DATA_TYPE_AUTOSTART][i] = copy_str(autostart ? "yes" : "no" );
    }
    ++data->domain_size;

    return data;
}

int virt_domain_autostart(virDomainPtr domain)
{
    int autostart = 0;
    if (virDomainGetAutostart(domain, &autostart))
        return VIRT_ERROR_FAILURE;

    if (autostart) {
        if (virDomainSetAutostart(domain, 0))
            return VIRT_ERROR_FAILURE;
    } else
        if (virDomainSetAutostart(domain, 1))
            return VIRT_ERROR_FAILURE;

    return VIRT_ERROR_SUCCESS;
}

int virt_domain_create(virDomainPtr domain)
{
    int state   = VIR_DOMAIN_NOSTATE;
    if (virDomainGetState(domain, &state, NULL, 0) < 0)
        return VIRT_ERROR_FAILURE;

    if (state == VIR_DOMAIN_SHUTOFF)
        if (virDomainCreate(domain))
            return VIRT_ERROR_FAILURE;
    if (state == VIR_DOMAIN_PAUSED)
        if (virDomainResume(domain))
            return VIRT_ERROR_FAILURE;

    return VIRT_ERROR_SUCCESS;
}

int virt_domain_pause(virDomainPtr domain)
{
    int state = VIR_DOMAIN_NOSTATE;
    if (virDomainGetState(domain, &state, NULL, 0) < 0)
        return VIRT_ERROR_FAILURE;

    if (state == VIR_DOMAIN_RUNNING)
        if (virDomainSuspend(domain))
            return VIRT_ERROR_FAILURE;

    return VIRT_ERROR_SUCCESS;
}

int virt_domain_reboot(virDomainPtr domain)
{
    int state = VIR_DOMAIN_NOSTATE;
    if (virDomainGetState(domain, &state, NULL, 0) < 0)
        return VIRT_ERROR_FAILURE;

    if (state == VIR_DOMAIN_RUNNING)
        if (virDomainReboot(domain, VIR_DOMAIN_REBOOT_DEFAULT))
            return VIRT_ERROR_FAILURE;

    return VIRT_ERROR_SUCCESS;
}

int virt_domain_destroy(virDomainPtr domain)
{
    int state = VIR_DOMAIN_NOSTATE;
    if (virDomainGetState(domain, &state, NULL, 0) < 0)
        return VIRT_ERROR_FAILURE;

    if (state == VIR_DOMAIN_RUNNING)
        if (virDomainDestroy(domain))
            return VIRT_ERROR_FAILURE;

    return VIRT_ERROR_SUCCESS;
}
