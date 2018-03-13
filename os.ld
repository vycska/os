__stack_size = 512;

ENTRY(Reset_Handler)

MEMORY
{
	rom(rx)	: ORIGIN = 0x00000000, LENGTH = 512K
	ram(rw)	: ORIGIN = 0x10000000, LENGTH = 32K
        ram1(rw) : ORIGIN = 0x2007c000, LENGTH = 16K
        ram2(rw) : ORIGIN = 0x20080000, LENGTH = 16K
}

SECTIONS
{
	.intvecs : {
		_intvecs_start = .;
		KEEP(*(.intvecs))
		. = ALIGN(0x10);
		_intvecs_end = .;
	} > rom = 0xaaaaaaaa
	
	.text : {
		_text_start = .;
		*(.text*)
		LONG (0xbbbbbbbb);
		. = ALIGN(0x10);
		_text_end = .;
	} > rom = 0xcccccccc

	.rodata : {
		_rodata_start = .;
		*(.rodata*)
		. = ALIGN(0x10);
		_rodata_end = .;
	} > rom = 0xdddddddd

	.stack (NOLOAD) : {
		_stack_start = .;
		. += __stack_size;
		. = ALIGN(0x10);
		_stack_end = .;
	} > ram

	.data : {
		_data_start = .;
		*(.data*)
		_data_end = .;
	} > ram AT>rom /* nurodom kad sita sekcija bus patalpinta i rom */

	.bss (NOLOAD) :{
		_bss_start = .; /* these symbols hold vma addresses */
		*(.bss*)
		*(COMMON*)
		_bss_end = .;
	} > ram

	.heap (NOLOAD) : ALIGN(0x100)  {
		end = .;
		_heap_start = end;
		. = ORIGIN(ram) + LENGTH(ram);
		_heap_end = .;
	} > ram

	/*
	.heap ORIGIN(ram) + LENGTH(ram) - 32 - __heap_size (NOLOAD) : {
		_heap_start = .
		. += __heap_size;
		_heap_end = .;
	}
	*/

	_data_start_lma = LOADADDR (.data);
	_intvecs_size = SIZEOF (.intvecs);
	_text_size = SIZEOF (.text);
	_rodata_size = SIZEOF (.rodata);
	_data_size = SIZEOF (.data);
	_bss_size = SIZEOF (.bss);
	_stack_size = SIZEOF(.stack);
	_heap_size = SIZEOF(.heap);
}
