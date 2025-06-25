/* ==> .../kernel/include/time.h
 *  -> (none).
 *
 * --------------------------------------------------
 *
 * By linzhichen114, write on 2025-6-25
 * Copyright © 2025 linzhichen114 and contributors, Based on GPL-3.0 open source agreement.
 */
#pragma once

#include <stdint.h>

typedef struct {
    uint64_t seconds;
    uint64_t microseconds;
} timeval_t;

void time_init();
timeval_t get_current_time();