mov r0,#824209266
str r0,=0xff08
mov r0,#540163840
str r0,=0xff0c
prints #65288
inputn r0
cmp #1,r0
beq .L0
b .L1
.L0:
mov r13,#0xff04
str r15,[r13],#-4
b one
.L1:
printn #2
one:
printn #1
ldr r0,[r13]
add r0,#4
b r0
andeq r0,r0,r0
