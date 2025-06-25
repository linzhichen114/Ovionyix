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

#include "printk.h"
#include "ovionyix.h"

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

    printk_init(fb->address, fb->width, fb->height, fb->pitch, fb->bpp);
    printk("Ovionyix version %s (%s version %s) #1 SMP %s %s\n", KERNL_VERS, COMPILER_NAME, COMPILER_VERSION, BUILD_DATE,
           BUILD_TIME);
    printk("Framebuffer %p, resolution = %dx%d\n", fb->address, fb->width, fb->height);

    // Halt forever
    for (;;) {
        __asm__("hlt");
    }
}
