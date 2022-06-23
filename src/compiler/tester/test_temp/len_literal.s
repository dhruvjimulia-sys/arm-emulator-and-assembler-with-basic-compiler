mov r1,#1212501068
str r1,=0xff08
mov r1,#1325400064
str r1,=0xff0c
mov r1,#0
b .LEN0
.LEN1
add r1,r1,#1
.LEN0:
ldr r2,[r1,#0xff08]
cmp r2,#0
bne .LEN1
mov r0,r1
andeq r0,r0,r0
