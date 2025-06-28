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
#include "time.h"

__attribute__((used, section(".limine_requests"))) static volatile LIMINE_BASE_REVISION(3);
__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};
__attribute__((used, section(".limine_requests")))
extern struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};
__attribute__((used, section(".limine_requests")))
extern volatile struct limine_executable_address_request kernel_addr_request = {
    .id = LIMINE_EXECUTABLE_ADDRESS_REQUEST,
    .revision = 0
};
__attribute__((used, section(".limine_requests")))
volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0
};
__attribute__((used, section(".limine_requests_start"))) static volatile LIMINE_REQUESTS_START_MARKER;
__attribute__((used, section(".limine_requests_end"))) static volatile LIMINE_REQUESTS_END_MARKER;

void kmain(void) {
    if (!LIMINE_BASE_REVISION_SUPPORTED)
        for (;;)
            __asm__ volatile("hlt");

    struct limine_framebuffer_response *framebuffer_response = framebuffer_request.response;
    if (framebuffer_response == NULL || framebuffer_response->framebuffer_count < 1)
        for (;;)
            __asm__ volatile("hlt");

    struct limine_framebuffer *fb = framebuffer_response->framebuffers[0];  // Fetch the first framebuffer

    /*================================================================================================================*/

    time_init();
    printk_init(fb->address, fb->width, fb->height, fb->pitch, fb->bpp);
    printk("Ovionyix version %s (%s %s) #1 SMP %s %s\n", KERNL_VERS, COMPILER_NAME, COMPILER_VERSION, BUILD_DATE, BUILD_TIME);
    printk("Framebuffer at %p, %dx%d @ %u bpp\n", fb->address, fb->width, fb->height, fb->bpp);
    panic("Nothing to do");
}
