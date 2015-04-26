/*OUTPUT_FORMAT(binary);*/

SECTIONS
{
	. = 0x08000000;
	.main : {
		*(.text)
	}
	. = (. + 31) & ~ 31;
}




