mov r0,#1
.L0:
mov r1,#1212501068
str r1,=0xff08
mov r1,#1325400064
str r1,=0xff0c
prints #65288
add r1,r0,#1
mov r0,r1
cmp #5,r0
ble .L1
b .L2
.L1:
b .L0
.L2:
andeq r0,r0,r0
