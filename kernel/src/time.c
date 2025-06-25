/* ==> .../kernel/src/time.c
 *  -> impl: time.h.
 *
 * --------------------------------------------------
 *
 * By linzhichen114, write on 2025-6-25
 * Copyright © 2025 linzhichen114 and contributors, Based on GPL-3.0 open source agreement.
 */
#include <limine.h>
#include "time.h"

static volatile struct limine_system_time_request time_request = {
    .id = LIMINE_SYSTEM_TIME_REQUEST,
    .revision = 0
};

void time_init() {
    // 初始化函数，预留扩展
}

timeval_t get_current_time() {
    timeval_t tv = {0};

    if (time_request.response) {
        uint64_t ns = time_request.response->time;
        tv.seconds = ns / 1000000000;
        tv.microseconds = (ns % 1000000000) / 1000;
    }

    return tv;
}