/* ==> .../kernel/src/main.c
 *  -> The kernel entry.
 *
 * --------------------------------------------------
 *
 * By linzhichen114, write on 2025-6-25
 * Copyright © 2025 linzhichen114 and contributors, Based on GPL-3.0 open source agreement.
 */

#include <stdint.h>
#include <stddef.h>
#include <limine.h>

#include "strdraw.h"

__attribute__((used, section(".limine_requests"))) static volatile LIMINE_BASE_REVISION(0);
__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

__attribute__((used, section(".limine_requests_start"))) static volatile LIMINE_REQUESTS_START_MARKER;
__attribute__((used, section(".limine_requests_end"))) static volatile LIMINE_REQUESTS_END_MARKER;

void kmain(void) {
    if (!LIMINE_BASE_REVISION_SUPPORTED) {
        __builtin_unreachable();
    }

    struct limine_framebuffer_response *framebuffer_response = framebuffer_request.response;
    if (framebuffer_response == NULL || framebuffer_response->framebuffer_count < 1) {
        __builtin_unreachable();
    }
    struct limine_framebuffer *fb = framebuffer_response->framebuffers[0];  // Get the first framebuffer

    for (uint64_t y = 0; y < fb->height; y++) {
        for (uint64_t x = 0; x < fb->width; x++) {
            uint32_t *pixel = (uint32_t*)((uint8_t*)fb->address + y * fb->pitch + x * (fb->bpp / 8));
            *pixel = 0x000000; // Black background
        }
    }

    const char *message = "Hello world!";
    int start_x = (fb->width - 12 * 9) / 2;
    int start_y = (fb->height - 114) / 2; // 字体高度16像素

    draw_string(fb, message, start_x, start_y, 0xFFFFFF); // 白色文本

    // 绘制补充信息
    draw_string(fb, "OS DEVELOPMENT", start_x, start_y + 30, 0x00FF00); // 绿色文本
    draw_string(fb, "FRAMEBUFFER: OK", start_x, start_y + 50, 0x00FFFF); // 青色文本

    // Halt forever
    for (;;) {
        __asm__("hlt");
    }
}
