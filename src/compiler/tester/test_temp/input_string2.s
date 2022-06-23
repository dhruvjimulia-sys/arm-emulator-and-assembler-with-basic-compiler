inputs r14
mov r0,r14
b .INP0
.INP1:
add r14,r14,#1
.INP0:
ldr r1,[r14]
cmp r1,#0
bne .INP1
add r14,r14,#2
inputs r14
mov r1,r14
b .INP2
.INP3:
add r14,r14,#1
.INP2:
ldr r2,[r14]
cmp r2,#0
bne .INP3
add r14,r14,#2
prints r0
prints r1
andeq r0,r0,r0
