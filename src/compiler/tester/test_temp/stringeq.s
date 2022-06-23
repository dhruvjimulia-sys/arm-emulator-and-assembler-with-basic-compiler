mov r1,#1212500992
str r1,=0xff08
ldr r0,=0xff08
mov r2,#1207959552
str r2,=0xff0c
ldr r1,=0xff0c
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
