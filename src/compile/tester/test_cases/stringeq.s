mov r1,#0
add r1,r1,#72
lsl r1,#8
add r1,r1,#69
lsl r1,#8
add r1,r1,#76
lsl r1,#8
mov r2,#127
lsl r2,#8
add r2,r2,#132
str r1,[r2]
ldr r0,=0x7f84
mov r2,#0
add r2,r2,#72
lsl r2,#8
lsl r2,#8
lsl r2,#8
mov r3,#127
lsl r3,#8
add r3,r3,#136
str r2,[r3]
ldr r1,=0x7f88
mov r2,#0
b .STREQ0
.STREQ1
add r2,r2,#1
.STREQ0:
ldr r3,[r0,r2]
ldr r4,[r1,r2]
cmp r3,#0
beq .STREQ2
cmp r3,r4
beq .STREQ1
b .STREQ3
.STREQ2:
cmp r4,#0
bne .STREQ3
mov r0,#2
.STREQ3:
andeq r0,r0,r0
