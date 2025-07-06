OUTPUT_FORMAT(elf64-x86-64)

/* kmain is the kernel entry. */
ENTRY(kmain)

PHDRS
{
    limine_requests PT_LOAD;
    text PT_LOAD;
    rodata PT_LOAD;
    data PT_LOAD;
}

SECTIONS
{
    . = 0xffffffff80000000;
    __kernel_start = .;

    /* Keep Limine requests */
    .limine_requests : {
        KEEP(*(.limine_requests_start))
        KEEP(*(.limine_requests))
        KEEP(*(.limine_requests_end))
    } :limine_requests

    . = ALIGN(CONSTANT(MAXPAGESIZE));

    .text : {
        *(.text .text.*)
    } :text

    . = ALIGN(CONSTANT(MAXPAGESIZE));

    .rodata : {
        *(.rodata .rodata.*)
    } :rodata

    . = ALIGN(CONSTANT(MAXPAGESIZE));

    .data : {
        *(.data .data.*)
    } :data

    .bss : {
        *(.bss .bss.*)
        *(COMMON)
    } :data

    /* Bitmap */
    . = ALIGN(4096);
    __bitmap_start = .;
    .bitmap : {
        /* 1MB free space */
        . += 1 * 1024 * 1024;
    }
    __bitmap_end = .;

    __kernel_end = .;

    /* Discard .eh_frame* and .note.* because they'll causes many errors in some hosts. */
    /* DO NOT COMMENT OR DELETE THIS!!!                                                 */
    /DISCARD/ : {
        *(.eh_frame*)
        *(.note .note.*)
    }
}
