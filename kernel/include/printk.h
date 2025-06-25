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
#include "font.h"

extern uint8_t ascfont[];

void draw_char(struct limine_framebuffer *fb, char c, int x, int y, uint32_t color) {
    // 获取字符的字体数据 (每个字符16字节)
    uint8_t *glyph = &ascfont[(unsigned char)c * 16];

    for (int row = 0; row < 16; row++) {
        uint8_t row_data = glyph[row];
        for (int col = 0; col < 8; col++) {
            // 检查每个位 (MSB first)
            if (row_data & (1 << (7 - col))) {
                uint32_t *pixel = (uint32_t*)((uint8_t*)fb->address +
                                            (y + row) * fb->pitch +
                                            (x + col) * (fb->bpp / 8));
                *pixel = color;
            }
        }
    }
}

// 绘制字符串
void draw_string(struct limine_framebuffer *fb, const char *str, int x, int y, uint32_t color) {
    for (size_t i = 0; str[i]; i++) {
        draw_char(fb, str[i], x + i * 9, y, color); // 9像素宽 (8+1间隔)
    }
}

void printk_init(void *fb_addr, uint64_t width, uint64_t height, uint64_t pitch, uint16_t bpp);
void printk(const char *fmt, ...);
void printk_va(const char *fmt, va_list args);
