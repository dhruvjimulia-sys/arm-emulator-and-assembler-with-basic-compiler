mov r0,#1
mov r1,#10
b .L0
.L1:
b .L2
.L3:
printn r0
sub r2,r1,#1
mov r1,r2
.L2:
cmp #6,r1
bgt .L4
b .L5
.L4:
b .L3
.L5:
add r1,r0,#1
mov r0,r1
.L0:
cmp #10,r0
blt .L6
b .L7
.L6:
b .L1
.L7:
andeq r0,r0,r0
