mov r1,#1212501068
str r1,=0xff08
mov r1,#1325400064
str r1,=0xff0c
ldr r0,=0xff08
mov r2,#0
b .LEN0
.LEN1
add r2,r2,#1
.LEN0:
ldr r3,[r2,#0xff08]
cmp r3,#0
bne .LEN1
add r3,r2,#1
mul r2,r3,#3
mov r1,r2
andeq r0,r0,r0
