.section start
.global _start
.global burst8
.extern main

.equ LDR_BASE, 0xFFFF2000

_start:
;@ set up stack to end of MEMC
mov		sp, #-4

ldr		r0, =LDR_BASE
mov		r2, #0x2000

ldr     lr, l_main
mov     lr, lr, lsl #20
mov     lr, lr, lsr #20
add     lr, lr, r0
l_main:
bl		main

;@ loop forever
l_end:
b		l_end


burst8:
;@ memcpy in INCR8 burst - assumes all pointers are aligned
;@ parameters same as in memcpy
;@ 
stmdb	sp!, {r4-r10}
l_burst8_next:
ldmia	r1!, {r3-r10}
stmia	r0!, {r3-r10}
subs	r2, r2, #32
bgt		l_burst8_next
ldmia	sp!, {r4-r10}
mov		pc, lr

