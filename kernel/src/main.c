#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limine.h>

#include "krnlibc.h"

#define TERMINAL_EMBEDDED_FONT
#include "os_terminal.h"

// Set the base revision to 3, this is recommended as this is the latest
// base revision described by the Limine boot protocol specification.
// See specification for further info.

__attribute__((used, section(".limine_requests")))
static volatile LIMINE_BASE_REVISION(3);

// The Limine requests can be placed anywhere, but it is important that
// the compiler does not optimise them away, so, usually, they should
// be made volatile or equivalent, _and_ they should be accessed at least
// once or marked as used with the "used" attribute as done here.

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

// Finally, define the start and end markers for the Limine requests.

__attribute__((used, section(".limine_requests_start")))
static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile LIMINE_REQUESTS_END_MARKER;

// Halt and catch fire function.
static void hcf(void) {
    for (;;) {
        asm ("hlt");
    }
}

// The following will be our kernel's entry point.
// If renaming kmain() to something else, make sure to change the
// linker script accordingly.
void kmain(void) {
    // Ensure the bootloader actually understands our base revision (see spec).
    if (LIMINE_BASE_REVISION_SUPPORTED == false) {
        hcf();
    }

    // Ensure we got a framebuffer.
    if (framebuffer_request.response == NULL
     || framebuffer_request.response->framebuffer_count < 1) {
        hcf();
    }

    // Fetch the first framebuffer.
    struct limine_framebuffer *fb = framebuffer_request.response->framebuffers[0];

    mem_init((void *)0x100000, 1024 * 1024);
    
    TerminalDisplay display = {
        .width = fb->width,
        .height = fb->height,
        .buffer = (uint32_t*)fb->address,
        .pitch = fb->pitch,
        .red_mask_size = fb->red_mask_size,
        .red_mask_shift = fb->red_mask_shift,
        .green_mask_size = fb->green_mask_size,
        .green_mask_shift = fb->green_mask_shift,
        .blue_mask_size = fb->blue_mask_size,
        .blue_mask_shift = fb->blue_mask_shift
    };

    terminal_init(&display, 14.0f, kmalloc, kfree);
    terminal_process("\x1B[1;32mLimine Terminal Ready!\x1B[0m\r\n");
    // // Note: we assume the framebuffer model is RGB with 32-bit pixels.
    // for (size_t i = 0; i < 100; i++) {
    //     volatile uint32_t *fb_ptr = framebuffer->address;
    //     fb_ptr[i * (framebuffer->pitch / 4) + i] = 0xffffff;
    // }

    terminal_destroy();
    // We're done, just hang...
    hcf();
}
