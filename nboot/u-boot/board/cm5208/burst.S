.globl burst8
burst8:
;@ memcpy in INCR8 burst - assumes all pointers are aligned
;@ parameters same as in memcpy
;@ 
stmdb	sp!, {r4-r10}
l_burst8_next:
ldmia	r1!, {r3-r10}
stmia	r0!, {r3-r10}
subs	r2, r2, #32
bgt	l_burst8_next
ldmia	sp!, {r4-r10}
mov	pc, lr

