/* kernel.ld file */
ASSERT(SIZEOF(.text) < 0x00100000, "Error: .text grew beyond the 1 MiB safety window");
OUTPUT_ARCH(arm)
OUTPUT_FORMAT("elf32-littlearm","elf32-bigarm","elf32-littlearm")


ENTRY(_start)

MEMORY
{
    ram : ORIGIN = 0x00000000, LENGTH = 0x07FFFFFF
}

SECTIONS
{
    . = 0x10000;    /* Start at 64KB for the kernel */
    .text BLOCK(4K) : ALIGN(4K)
    {
        *(.text)
        *(.text*)
    }

    .rodata BLOCK(4K) : ALIGN(4K)
    {
        *(.rodata)
        *(.rodata*)
    }

    .data BLOCK(4K) : ALIGN(4K)
    {
    	*(.data) 
    }
	
    .bss BLOCK(4K) : ALIGN(4K)
    {
    	__bss_start = .;
	    *(.bss)
        *(.bss*)
	    *(COMMON)
	    __bss_end   = .;
	}
    
    /* Top‐of‐RAM stack pointer */
    _estack = ORIGIN(ram) + LENGTH(ram);

    /* Heap starts right after .bss */
    _eheap  = ALIGN( __bss_end );
 
}