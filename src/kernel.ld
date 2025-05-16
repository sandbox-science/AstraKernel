/* kernel.ld file */
ENTRY(_start)

SECTIONS
{
    . = 2M;
    .text BLOCK(4K) : ALIGN(4K)
    {
        *(.multiboot)
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
	    *(COMMON)
	    *(.bss)
	    __bss_end   = .;
	}
}