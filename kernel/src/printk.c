/* ==> .../kernel/src/printk.c
*  -> impl: printk.h.
 *
 * --------------------------------------------------
 *
 * By linzhichen114, write on 2025-6-25
 * Copyright © 2025 linzhichen114 and contributors, Based on GPL-3.0 open source agreement.
 */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/printk.h"
#include "font.h"
#include "time.h"

// 控制台状态
static uint32_t *framebuffer = NULL;
static uint64_t fb_width = 0;
static uint64_t fb_height = 0;
static uint64_t fb_pitch = 0;
static uint16_t fb_bpp = 0;

static uint64_t cursor_x = 0;
static uint64_t cursor_y = 0;
static uint32_t text_color = 0x00FFFFFF;  // 白色
static uint32_t bg_color = 0x00000000;    // 黑色

// 格式化缓冲区
#define PRINTK_BUF_SIZE 256
static char printk_buf[PRINTK_BUF_SIZE];

// 绘制一个字符
static void draw_char(char c, uint64_t x, uint64_t y) {
    if (c < 32 || c > 126) return;  // 只支持可打印ASCII字符
    
    const uint8_t *glyph = &ascfont[(c - 32) * FONT_HEIGHT];
    
    for (uint64_t row = 0; row < FONT_HEIGHT; row++) {
        uint8_t row_data = glyph[row];
        for (uint64_t col = 0; col < FONT_WIDTH; col++) {
            bool pixel_set = row_data & (1 << (7 - col));
            uint32_t color = pixel_set ? text_color : bg_color;
            
            uint32_t *pixel = (uint32_t*)((uint8_t*)framebuffer + 
                                         (y + row) * fb_pitch + 
                                         (x + col) * (fb_bpp / 8));
            *pixel = color;
        }
    }
}

// 绘制字符串
static void draw_string(const char *str, uint64_t x, uint64_t y) {
    for (size_t i = 0; str[i]; i++) {
        draw_char(str[i], x + i * (FONT_WIDTH + 1), y);
    }
}

// 清屏
static void clear_screen() {
    for (uint64_t y = 0; y < fb_height; y++) {
        for (uint64_t x = 0; x < fb_width; x++) {
            uint32_t *pixel = (uint32_t*)((uint8_t*)framebuffer + y * fb_pitch + x * (fb_bpp / 8));
            *pixel = bg_color;
        }
    }
}

// 滚动屏幕
static void scroll_screen() {
    // 计算一行像素的大小
    uint64_t line_size = fb_pitch * FONT_HEIGHT;
    
    // 将内容向上移动一行
    for (uint64_t y = FONT_HEIGHT; y < fb_height; y++) {
        void *src = (uint8_t*)framebuffer + y * fb_pitch;
        void *dst = (uint8_t*)framebuffer + (y - FONT_HEIGHT) * fb_pitch;
        memmove(dst, src, fb_pitch);
    }
    
    // 清除最后一行
    for (uint64_t y = fb_height - FONT_HEIGHT; y < fb_height; y++) {
        for (uint64_t x = 0; x < fb_width; x++) {
            uint32_t *pixel = (uint32_t*)((uint8_t*)framebuffer + y * fb_pitch + x * (fb_bpp / 8));
            *pixel = bg_color;
        }
    }
    
    cursor_y -= FONT_HEIGHT;
}

// 初始化打印系统
void printk_init(void *fb_addr, uint64_t width, uint64_t height, uint64_t pitch, uint16_t bpp) {
    framebuffer = (uint32_t*)fb_addr;
    fb_width = width;
    fb_height = height;
    fb_pitch = pitch;
    fb_bpp = bpp;
    
    cursor_x = 0;
    cursor_y = 0;
    
    clear_screen();
    time_init();
}

// 打印字符串到屏幕（带自动换行）
static void console_write(const char *str) {
    for (size_t i = 0; str[i]; i++) {
        if (str[i] == '\n') {
            cursor_x = 0;
            cursor_y += FONT_HEIGHT;
        } else {
            draw_char(str[i], cursor_x, cursor_y);
            cursor_x += FONT_WIDTH + 1;
        }
        
        // 检查是否需要换行
        if (cursor_x + FONT_WIDTH > fb_width) {
            cursor_x = 0;
            cursor_y += FONT_HEIGHT;
        }
        
        // 检查是否需要滚动
        if (cursor_y + FONT_HEIGHT > fb_height) {
            scroll_screen();
        }
    }
}

// 格式化时间戳
static void format_timestamp(char *buf, size_t size) {
    timeval_t tv = get_current_time();
    snprintf(buf, size, "[%6llu.%06llu] ", tv.seconds, tv.microseconds);
}

// 核心打印函数
void printk_va(const char *fmt, va_list args) {
    // 格式化时间戳
    char timestamp_buf[32];
    format_timestamp(timestamp_buf, sizeof(timestamp_buf));
    
    // 组合完整消息
    char *buf_ptr = printk_buf;
    strcpy(buf_ptr, timestamp_buf);
    buf_ptr += strlen(timestamp_buf);
    
    // 格式化用户消息
    vsnprintf(buf_ptr, PRINTK_BUF_SIZE - strlen(timestamp_buf), fmt, args);
    
    // 输出到控制台
    console_write(printk_buf);
}

// 主打印函数
void printk(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    printk_va(fmt, args);
    va_end(args);
}