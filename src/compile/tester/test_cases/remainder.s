mov r0,#100
mov r1,#6
b .L0
.L1:
sub r2,r0,r1
mov r0,r2
.L0:
cmp r1,r0
bge .L2
b .L3
.L2:
b .L1
.L3:
printn r0
andeq r0,r0,r0
