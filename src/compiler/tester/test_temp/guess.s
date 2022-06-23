mov r0,#6
mov r1,#1227320608
str r1,=0xff08
mov r1,#1952999790
str r1,=0xff0c
mov r1,#1802071655
str r1,=0xff10
mov r1,#544171552
str r1,=0xff14
mov r1,#1629515381
str r1,=0xff18
mov r1,#1835165042
str r1,=0xff1c
mov r1,#543319412
str r1,=0xff20
mov r1,#2003133806
str r1,=0xff24
mov r1,#540090465
str r1,=0xff28
mov r1,#1852055601
str r1,=0xff2c
mov r1,#808321024
str r1,=0xff30
prints #65288
b .L0
.L1:
mov r1,#1466458484
str r1,=0xff08
mov r1,#543781664
str r1,=0xff0c
mov r1,#1769225984
str r1,=0xff10
prints #65288
inputn r1
cmp r1,r0
beq .L2
b .L3
.L2:
b win
.L3:
.L0:
cmp #1,#1
beq .L4
b .L5
.L4:
b .L1
.L5:
win:
mov r0,#1416126836
str r0,=0xff08
mov r0,#661856355
str r0,=0xff0c
mov r0,#1869771365
str r0,=0xff10
mov r0,#1668555008
str r0,=0xff14
prints #65288
andeq r0,r0,r0
