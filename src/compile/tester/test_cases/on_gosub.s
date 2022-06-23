start:
inputn r0
cmp #5,r0
beq .L0
b .L1
.L0:
mov r13,#0xff04
str r15,[r13],#-4
b start
.L1:
andeq r0,r0,r0
