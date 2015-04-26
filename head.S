	// simple GBA fastmem bootloader
	// GreaseMonkey, 2014-2015 - Public Domain
	.arm
	.section .text
	.extern _start
	b _start

