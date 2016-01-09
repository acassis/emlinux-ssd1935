SECTIONS
{
	. = 0xFFFF1000;		/* increase this value to change your load address */
	.text	:	AT (0x40001000) { *(start) *(.text) *(.rodata) }
}
