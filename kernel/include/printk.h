/* ==> .../kernel/include/printk.h
*  -> Draw string and character to screen.
 *
 * --------------------------------------------------
 *
 * By linzhichen114, write on 2025-6-25
 * Copyright © 2025 linzhichen114 and contributors, Based on GPL-3.0 open source agreement.
 */

#pragma once

#include <stdarg.h>
#include "../freestnd-c-hdrs/stdint.h"
#include "../src/limine.h"

void printk_init(void *fb_addr, uint64_t width, uint64_t height, uint64_t pitch, uint16_t bpp);
void printk(const char *fmt, ...);
void printk_va(const char *fmt, va_list args);