mov r0,#0
mov r1,#100
mov r2,#6
b .L0
.L1:
sub r3,r1,r2
mov r1,r3
add r1,r0,#1
mov r0,r1
.L0:
cmp r2,r1
bge .L2
b .L3
.L2:
b .L1
.L3:
printn r0
andeq r0,r0,r0
