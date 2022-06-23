mov r0,#1
.L0:
mov r1,#0
add r1,r1,#72
lsl r1,#8
add r1,r1,#69
lsl r1,#8
add r1,r1,#76
lsl r1,#8
add r1,r1,#76
mov r2,#255
lsl r2,#8
add r2,r2,#8
str r1,[r2]
mov r1,#0
add r1,r1,#79
lsl r1,#8
lsl r1,#8
lsl r1,#8
mov r2,#255
lsl r2,#8
add r2,r2,#12
str r1,[r2]
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
