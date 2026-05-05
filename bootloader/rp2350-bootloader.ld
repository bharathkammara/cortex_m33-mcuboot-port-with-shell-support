ENTRY(Reset_Handler)
#OUTPUT_FORMAT ("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")


SEARCH_DIR(..)
INCLUDE "common/linker/memory-sections.ld"

/* Section Definitions */
SECTIONS
{
	.isr_vector :
	{
		. = ALIGN(4);
		KEEP(*(.isr_vector))
		. = ALIGN(4);
	}> BOOTLOADER

	.pico_metadata :
	{
		. = ALIGN(4);
		KEEP(*(.picobin_block))
		. = ALIGN(4);
	}> BOOTLOADER

	.text :
	{
		. = ALIGN(4);
		*(.text)
		*(.text.*)
		*(.rodata)
		*(.rodata.*)
		*(.init)
		*(.fini)
		. = ALIGN(4);
		_etext = .;
	}> BOOTLOADER

	_la_data = LOADADDR(.data);
	.data :
	{
		. = ALIGN(4);
		_sdata = .;
		*(.data)
		*(.data.*)
		. = ALIGN(4);
		. = ALIGN(4);
        /* preinit data */
        PROVIDE_HIDDEN (__mutex_array_start = .);
        KEEP(*(SORT(.mutex_array.*)))
        KEEP(*(.mutex_array))
        PROVIDE_HIDDEN (__mutex_array_end = .);

        . = ALIGN(4);
		_edata = .;
	}> SRAM AT> BOOTLOADER

	.bss :
	{
		. = ALIGN(4);
		_sbss = .;
		__bss_start__ = .;
		*(.bss)
		*(.bss.*)
		*(COMMON)
		. = ALIGN(4);
		_ebss = .;
		__bss_end__ = .;
		. = ALIGN(4);
		end = .;
		__end__ = .;
	}> SRAM
}
