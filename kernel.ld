ASSERT(SIZEOF(.text) < 0x00100000, "Error: .text grew beyond the 1 MiB safety window");
OUTPUT_ARCH(arm)
OUTPUT_FORMAT("elf32-littlearm","elf32-bigarm","elf32-littlearm")

ENTRY(_start)

MEMORY
{
    ram : ORIGIN = 0x00000000, LENGTH = 0x08000000 /* 128 MB */
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
    . = 0x00000000;
    .vectors ALIGN(32) :
    {
        KEEP(*(.vectors))
    } > ram

    /* Main kernel starts at 64KB */
    . = 0x10000;
    .text BLOCK(4K) : ALIGN(4K)
    {
        *(.text)
        *(.text*)
    } > ram

    .rodata BLOCK(4K) : ALIGN(4K)
    {
        *(.rodata)
        *(.rodata*)
    } > ram

    .data BLOCK(4K) : ALIGN(4K)
    {
        *(.data)
    } > ram

    .bss BLOCK(4K) : ALIGN(4K)
    {
        __bss_start = .;
        *(.bss)
        *(.bss*)
        *(COMMON)
        __bss_end = .;
    } > ram

    /* Stack pointer = top of RAM */
    _estack = ORIGIN(ram) + LENGTH(ram);

    /* Heap starts right after .bss */
    _eheap = ALIGN(__bss_end);
}
