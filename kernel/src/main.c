#include <stdint.h>
#include <stdbool.h>
#include <limine.h>

__attribute__((used, section(".limine_requests")))       static volatile LIMINE_BASE_REVISION(0);
__attribute__((used, section(".limine_requests_start"))) static volatile LIMINE_REQUESTS_START_MARKER;
__attribute__((used, section(".limine_requests_end")))   static volatile LIMINE_REQUESTS_END_MARKER;

// 现代 Limine 终端请求结构
static volatile struct limine_terminal *terminal = NULL;

// 终端回调函数
static void terminal_callback(struct limine_terminal *term,
                              uint64_t type,
                              uint64_t arg1,
                              uint64_t arg2,
                              uint64_t arg3) {
    // 保存终端引用
    terminal = term;
}

// 现代终端请求
static volatile struct limine_terminal_request terminal_request = {
    .id = LIMINE_TERMINAL_REQUEST,
    .revision = 0,
    .callback = terminal_callback
};

// 内核入口点
void kmain(void) {
    // 验证基本修订版支持
    if (!LIMINE_BASE_REVISION_SUPPORTED) {
      while (1) __asm__("hlt");
    }

    // 检查终端是否可用
    if (terminal == NULL) {
      while (1) __asm__("hlt");
    }

    // 使用现代写入函数
    const char *msg = "\e[32mM1 Milestone Reached! (Modern Limine API)\n\e[0m";
    struct limine_terminal *term = terminal;

    // 直接写入终端
    term->ops->write(term, msg, __builtin_strlen(msg));

    // 永久挂起
    while (1) __asm__("hlt");
}