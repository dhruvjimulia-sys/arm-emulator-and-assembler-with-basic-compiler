mov r0,#1
b .L0
.L1:
mul r1,#2,r0
printn r1
add r1,r0,#1
mov r0,r1
.L0:
cmp #6,r0
ble .L2
b .L3
.L2:
b .L1
.L3:
andeq r0,r0,r0
