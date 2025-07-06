/* ==> .../kernel/src/printk.c
*  -> impl: printk.h.
 *
 * --------------------------------------------------
 *
 * By linzhichen114, write on 2025-6-25
 * Copyright © 2025 linzhichen114 and contributors, Based on GPL-3.0 open source agreement.
 */
#include "stdbool.h"
#include "stddef.h"
#include "stdint.h"
#include "stdarg.h"

#include "../include/printk.h"
#include "../include/font.h"
#include "../include/time.h"
#include "../include/krnlibc.h"

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

// 整数转字符串
static void itoa(char *buf, uint64_t num, int base) {
    char *p = buf;
    char *p1, *p2;
    uint64_t tmp = num;

    // 处理0的特殊情况
    if (num == 0) {
        *p++ = '0';
        *p = '\0';
        return;
    }

    // 生成数字字符串（逆序）
    while (tmp) {
        uint64_t rem = tmp % base;
        *p++ = (rem < 10) ? ('0' + rem) : ('a' + rem - 10);
        tmp = tmp / base;
    }

    // 反转字符串
    *p = '\0';
    p1 = buf;
    p2 = p - 1;
    while (p1 < p2) {
        char tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
        p1++;
        p2--;
    }
}

// 绘制一个字符
static void draw_char(char c, uint64_t x, uint64_t y) {
    if (c < 32 || c > 126) return;     // Only supports printable ASCII characters

    const uint8_t *glyph = &ascfont[(c) * FONT_HEIGHT];

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
    for (uint64_t y = FONT_HEIGHT; y < fb_height; y++) {
        void *src = (uint8_t*)framebuffer + y * fb_pitch;
        void *dst = (uint8_t*)framebuffer + (y - FONT_HEIGHT) * fb_pitch;
        memmove(dst, src, fb_pitch);
    }

    for (uint64_t y = fb_height - FONT_HEIGHT; y < fb_height; y++) {
        for (uint64_t x = 0; x < fb_width; x++) {
            uint32_t *pixel = (uint32_t*)((uint8_t*)framebuffer + y * fb_pitch + x * (fb_bpp / 8));
            *pixel = bg_color;
        }
    }

    cursor_y -= FONT_HEIGHT;
}

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

static void format_timestamp(char *buf, size_t size) {
    timeval_t tv = get_current_time();

    // 手动格式化时间戳 [seconds.microseconds]
    char *p = buf;
    *p++ = '[';

    // 格式化秒部分 (6位数字)
    uint64_t sec = tv.seconds;
    if (sec < 100000) {
        *p++ = ' ';
        if (sec < 10000) {
            *p++ = ' ';
            if (sec < 1000) {
                *p++ = ' ';
                if (sec < 100) {
                    *p++ = ' ';
                    if (sec < 10) {
                        *p++ = ' ';
                    }
                }
            }
        }
    }

    // 写入秒
    char sec_buf[12];
    itoa(sec_buf, sec, 10);
    for (char *s = sec_buf; *s; s++) {
        *p++ = *s;
    }

    *p++ = '.';

    // 格式化微秒部分 (6位数字)
    uint64_t usec = tv.microseconds;
    if (usec < 100000) {
        *p++ = '0';
        if (usec < 10000) {
            *p++ = '0';
            if (usec < 1000) {
                *p++ = '0';
                if (usec < 100) {
                    *p++ = '0';
                    if (usec < 10) {
                        *p++ = '0';
                    }
                }
            }
        }
    }

    // 写入微秒
    char usec_buf[12];
    itoa(usec_buf, usec, 10);
    for (char *s = usec_buf; *s; s++) {
        *p++ = *s;
    }

    *p++ = ']';
    *p++ = ' ';
    *p = '\0';
}

void kvsnprintf(char *buf, size_t size, const char *fmt, va_list args) {
    char *p = buf;
    char num_buf[32];
    const char *str_val;
    uint64_t uint_val;
    int int_val;

    for (; *fmt && (p - buf) < (int)(size - 1); fmt++) {
        if (*fmt != '%') {
            *p++ = *fmt;
            continue;
        }

        fmt++; // 跳过'%'
        if (!*fmt) break;

        switch (*fmt) {
            case 's': // 字符串
                str_val = va_arg(args, const char *);
                while (*str_val && (p - buf) < (int)(size - 1)) {
                    *p++ = *str_val++;
                }
                break;

            case 'd': // 有符号整数
            case 'i':
                int_val = va_arg(args, int);
                if (int_val < 0) {
                    *p++ = '-';
                    int_val = -int_val;
                }
                itoa(num_buf, (uint64_t)int_val, 10);
                for (char *n = num_buf; *n && (p - buf) < (int)(size - 1); n++) {
                    *p++ = *n;
                }
                break;

            case 'u': // 无符号整数
                uint_val = va_arg(args, uint64_t);
                itoa(num_buf, uint_val, 10);
                for (char *n = num_buf; *n && (p - buf) < (int)(size - 1); n++) {
                    *p++ = *n;
                }
                break;

            case 'x': // 十六进制
                uint_val = va_arg(args, uint64_t);
                itoa(num_buf, uint_val, 16);
                for (char *n = num_buf; *n && (p - buf) < (int)(size - 1); n++) {
                    *p++ = *n;
                }
                break;

            case 'p': // 指针
                *p++ = '0';
                *p++ = 'x';
                uint_val = (uint64_t)va_arg(args, void *);
                itoa(num_buf, uint_val, 16);
                for (char *n = num_buf; *n && (p - buf) < (int)(size - 1); n++) {
                    *p++ = *n;
                }
                break;

            case 'c': // 字符
                *p++ = (char)va_arg(args, int);
                break;

            case '%': // 百分号
                *p++ = '%';
                break;

            default: // 未知格式
                *p++ = '%';
                *p++ = *fmt;
                break;
        }
    }

    *p = '\0'; // 确保以null结尾
}

// 核心打印函数
void printk_va(const char *fmt, va_list args) {
    // Format timestamp
    char timestamp_buf[32];
    format_timestamp(timestamp_buf, sizeof(timestamp_buf));

    char full_buf[256];
    char *p = full_buf;

    // Copy timestamp
    for (char *s = timestamp_buf; *s; s++) {
        *p++ = *s;
    }

    // Format message
    char msg_buf[200];
    kvsnprintf(msg_buf, sizeof(msg_buf), fmt, args);

    // Copy message
    for (char *s = msg_buf; *s; s++) {
        *p++ = *s;
    }
    *p = '\0';

    // Write to console
    console_write(full_buf);
}

// 主打印函数
void printk(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    printk_va(fmt, args);
    va_end(args);
}

void dump_stack(void) {
    struct stack_frame {
        struct stack_frame *next;
        uintptr_t rip;
    } __attribute__((packed));

    struct stack_frame *rbp;
    uintptr_t rip;

    __asm__ volatile("movq %%rbp, %0" : "=r"(rbp));
    __asm__ volatile("leaq (%%rip), %0" : "=r"(rip));

    printk("Call Trace:\n");
    printk(" <TASK>\n");

    // 最多遍历16帧
    for (int i = 0; i < 16; i++) {
        if ((uintptr_t)rbp < 0x1000 || (uintptr_t)rbp > 0xFFFFFFFFFFFFF000) {
            printk("  [<0x%016lx>] invalid frame\n", rip);
            break;
        }

        printk("  [<0x%016lx>]\n", rip);

        rip = rbp->rip;
        rbp = rbp->next;

        if (rip == 0 || rbp == 0) {
            break;
        }
    }
    printk(" </TASK>\n");
}

void dump_registers(void) {
    uint64_t rax, rbx, rcx, rdx, rsi, rdi, r8, r9, r10, r11, r12, r13, r14, r15;
    uint64_t cr0, cr2, cr3, cr4;

    __asm__ volatile(
        "movq %%rax, %0; movq %%rbx, %1; movq %%rcx, %2; movq %%rdx, %3;"
        "movq %%rsi, %4; movq %%rdi, %5; movq %%r8, %6; movq %%r9, %7;"
        "movq %%r10, %8; movq %%r11, %9; movq %%r12, %10; movq %%r13, %11;"
        "movq %%r14, %12; movq %%r15, %13;"
        : "=r"(rax), "=r"(rbx), "=r"(rcx), "=r"(rdx),
          "=r"(rsi), "=r"(rdi), "=r"(r8), "=r"(r9),
          "=r"(r10), "=r"(r11), "=r"(r12), "=r"(r13),
          "=r"(r14), "=r"(r15)
    );

    __asm__ volatile("movq %%cr0, %0" : "=r"(cr0));
    __asm__ volatile("movq %%cr2, %0" : "=r"(cr2));
    __asm__ volatile("movq %%cr3, %0" : "=r"(cr3));
    __asm__ volatile("movq %%cr4, %0" : "=r"(cr4));

    // printk("REGISTERS INFO:\n");
    printk("RAX: 0x%016lx RBX: 0x%016lx RCX: 0x%016lx RDX: 0x%016lx\n", rax, rbx, rcx, rdx);
    printk("RSI: 0x%016lx RDI: 0x%016lx R8:  0x%016lx R9:  0x%016lx\n", rsi, rdi, r8, r9);
    printk("R10: 0x%016lx R11: 0x%016lx R12: 0x%016lx R13: 0x%016lx\n", r10, r11, r12, r13);
    printk("R14: 0x%016lx R15: 0x%016lx\n", r14, r15);
    printk("CR0: 0x%016lx CR2: 0x%016lx CR3: 0x%016lx CR4: 0x%016lx\n", cr0, cr2, cr3, cr4);
}


void panic(const char *desp) {
    // 打印堆栈跟踪

    printk("\n");
    printk("Kernel panic - not syncing: %s\n", desp);
    dump_stack();
    printk("--- [ end Kernel panic - not syncing: %s ] ---\n", desp);

    // 永久停机
    for (;;)
        __asm__ volatile("hlt");
}