#pragma once

#include <stdint.h>
#include <stddef.h>

// GCC and Clang reserve the right to generate calls to the following
// 4 functions even if they are not directly called.
// Implement them as the C specification mandates.
// DO NOT remove or rename these functions, or stuff will eventually break!

void *memcpy(void *restrict dest, const void *restrict src, size_t n) {
    uint8_t *restrict pdest = (uint8_t *restrict)dest;
    const uint8_t *restrict psrc = (const uint8_t *restrict)src;

    for (size_t i = 0; i < n; i++) {
        pdest[i] = psrc[i];
    }

    return dest;
}

void *memset(void *s, int c, size_t n) {
    uint8_t *p = (uint8_t *)s;

    for (size_t i = 0; i < n; i++) {
        p[i] = (uint8_t)c;
    }

    return s;
}

void *memmove(void *dest, const void *src, size_t n) {
    uint8_t *pdest = (uint8_t *)dest;
    const uint8_t *psrc = (const uint8_t *)src;

    if (src > dest) {
        for (size_t i = 0; i < n; i++) {
            pdest[i] = psrc[i];
        }
    } else if (src < dest) {
        for (size_t i = n; i > 0; i--) {
            pdest[i-1] = psrc[i-1];
        }
    }

    return dest;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
    const uint8_t *p1 = (const uint8_t *)s1;
    const uint8_t *p2 = (const uint8_t *)s2;

    for (size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) {
            return p1[i] < p2[i] ? -1 : 1;
        }
    }

    return 0;
}

// Memory allocation

typedef struct mem_block {
    size_t size;
    struct mem_block *next;
    bool is_free;
} mem_block_t;

static mem_block_t *free_list = NULL;

void mem_init(void *start_addr, size_t size) {
    // 对齐地址（按 8 字节对齐）
    uintptr_t addr = (uintptr_t)start_addr;
    addr = (addr + 7) & ~7;

    // 初始化第一个空闲块
    mem_block_t *block = (mem_block_t *)addr;
    block->size = size - sizeof(mem_block_t);
    block->next = NULL;
    block->is_free = true;

    free_list = block;
}

void *kmalloc(size_t size) {
    // 最小分配单位（避免碎片）
    size = (size + 7) & ~7; // 8 字节对齐

    mem_block_t *curr = free_list;
    mem_block_t *prev = NULL;

    while (curr) {
        if (curr->is_free && curr->size >= size) {
            // 空间足够：分割块（剩余空间需能容纳新头部）
            if (curr->size > size + sizeof(mem_block_t)) {
                mem_block_t *new_block = (mem_block_t *)((char *)curr + sizeof(mem_block_t) + size);
                new_block->size = curr->size - size - sizeof(mem_block_t);
                new_block->is_free = true;
                new_block->next = curr->next;

                curr->size = size;
                curr->next = new_block;
            }

            // 标记为已分配
            curr->is_free = false;

            // 返回用户空间地址（跳过头部）
            return (void *)((char *)curr + sizeof(mem_block_t));
        }
        prev = curr;
        curr = curr->next;
    }
    return NULL; // 分配失败
}

void kfree(void *ptr) {
    if (!ptr) return;

    // 获取块头部
    mem_block_t *block = (mem_block_t *)((char *)ptr - sizeof(mem_block_t));
    block->is_free = true;

    // 合并相邻空闲块（向前合并）
    mem_block_t *curr = free_list;
    mem_block_t *prev = NULL;

    while (curr) {
        if (curr->is_free && curr->next && curr->next->is_free) {
            // 检查物理相邻性
            if ((char *)curr + sizeof(mem_block_t) + curr->size == (char *)curr->next) {
                curr->size += sizeof(mem_block_t) + curr->next->size;
                curr->next = curr->next->next;
            }
        }
        prev = curr;
        curr = curr->next;
    }
}