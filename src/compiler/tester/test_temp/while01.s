mov r0,#1
b .L0
.L1:
mov r1,#1212501068
str r1,=0xff08
mov r1,#1327519567
str r1,=0xff0c
mov r1,#1380729856
str r1,=0xff10
prints #65288
add r1,r0,#1
mov r0,r1
.L0:
cmp #5,r0
ble .L2
b .L3
.L2:
b .L1
.L3:
andeq r0,r0,r0
