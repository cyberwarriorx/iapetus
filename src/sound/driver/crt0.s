.global start
.global _start
.global endfunc
.global main

start:	
_start:
	move.l		#__stack, %a7

bss_clear:
	move.l		#__bss_start, %d1
	move.l		#__bss_end, %d0
	cmp.l		%d0, %d1
	beq		start_exec
	move.l		%d1, %a0
	sub.l		%d1, %d0
	subq.l		#1, %d0
bss_clear_loop:
	clr.b		(%a0)+
	subq.l		#1, %d0
	bpl		bss_clear_loop	
	
start_exec:
	jsr		main

endfunc:
	bra.b		endfunc
