ASSERT(SIZEOF(.text) < 0x00100000, "Error: .text grew beyond the 1 MiB safety window");
OUTPUT_ARCH(arm)
OUTPUT_FORMAT("elf32-littlearm","elf32-bigarm","elf32-littlearm")
ENTRY(_start)

MEMORY
{
    RAM (rwx) : ORIGIN = 0x00000000, LENGTH = 128M /* 128 MB */
}

SECTIONS
{
    /* Exception vector table at 0x00000000 
     *  the . line can be removed at some point
     *  if we want, as the VBAR alone determine where
     *  exceptions land. This will be done once the MMU
     *  is fully enabled. For this, we will need to have
     *  the page table in place (vector Virtual Addres (VA) mapped)
     */
    . = ORIGIN(RAM);
    .vectors ALIGN(32) : /* Aligned by 32 bytes */
    {
        KEEP(*(.vectors))
    } > RAM

    /* Main kernel starts at 64KB */
    . = 0x00010000;
    .text BLOCK(4K) : ALIGN(4K)
    {
        __text_start = .;
        *(.text .text.*)
        __text_end = .;
    } > RAM

    .rodata BLOCK(4K) : ALIGN(4K)
    {
        __rodata_start = .;
        *(.rodata .rodata.*)
        __rodata_end = .;
    } > RAM

    .data BLOCK(4K) : ALIGN(4K)
    {
        __data_start = .;
        *(.data)
        __data_end = .;
    } > RAM AT > RAM
    __data_load = LOADADDR(.data);

    /* Reserved space for initial page tables (MMU) */
    .ptables BLOCK(16K) : ALIGN(16K)
    {
        __ptables_start = .;
        /* Reserve 16 KiB for L1 table; extend as needed for L2 tables */
        . = . + 0x4000;
        __ptables_end = .;
    } > RAM

    .bss BLOCK(4K) : ALIGN(4K)
    {
        __bss_start = .;
        *(.bss .bss.*)
        *(.sbss .sbss.*)
        *(COMMON)
        __bss_end = .;
    } > RAM

    /* Stack pointer = top of RAM */
    __stack_top__ = ORIGIN(RAM) + LENGTH(RAM);
    
    /* Reserve space for kernel stacks (64 KiB total) */
    __stack_bottom__ = __stack_top__ - 64K;

    /* Heap starts right after .bss */
    __heap_start__ = .;
    __heap_end__ = __stack_bottom__;
    __heap_size__ = __heap_end__ - __heap_start__;
    
    /* Discard unneeded sections to keep image lean */
    /DISCARD/ :
    {
        *(.comment)
        *(.note*)
        *(.debug* .gnu.debug*)
        *(.stab* .stabstr*)
    }
}
