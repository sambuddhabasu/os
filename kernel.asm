;; Author: Sambuddha Basu
bits 32
section .text
	align 4
	dd 0x1BADB002            ;magic
	dd 0x00                  ;flags
	dd - (0x1BADB002 + 0x00) ;checksum. m+f+c should be zero

global start
extern main_program	        ;main_program is defined in the c file

start:
	cli 			;block interrupts
	call main_program
	hlt		 
