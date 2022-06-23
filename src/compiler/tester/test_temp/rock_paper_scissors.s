mov r14,#32644
mov r0,#2
str r0,#65532
mov r0,#1383031659
str r0,=0xff08
mov r0,#740323425
str r0,=0xff0c
mov r0,#1885696556
str r0,=0xff10
mov r0,#544433001
str r0,=0xff14
mov r0,#1936945010
str r0,=0xff18
mov r0,#1929379840
str r0,=0xff1c
prints #65288
game:
mov r0,#1130917743
str r0,=0xff08
mov r0,#1936007217
str r0,=0xff0c
mov r0,#543584114
str r0,=0xff10
mov r0,#544370531
str r0,=0xff14
mov r0,#1798053938
str r0,=0xff18
mov r0,#543584114
str r0,=0xff1c
mov r0,#544235888
str r0,=0xff20
mov r0,#1701981216
str r0,=0xff24
mov r0,#857761391
str r0,=0xff28
mov r0,#1914729315
str r0,=0xff2c
mov r0,#1769173871
str r0,=0xff30
mov r0,#1920139264
str r0,=0xff34
prints #65288
inputn r0
ldr r2,#65532
mul r2.r2,#1
add r2,r2,#0
b .RND0:
.RND1:
sub r2,r2,#3
.RND0:
cmp r2,#3
bge .RND1
str r2,#65532
mov r1,r2
add r2,r0,#1
sub r0,r2,r1
cmp #2,r0
beq .L0
b .L1
.L0:
mov r0,#1146241367
str r0,=0xff08
mov r0,#0
str r0,=0xff0c
prints #65288
.L1:
add r2,r0,#1
sub r0,r2,r1
cmp #1,r0
beq .L2
b .L3
.L2:
mov r0,#1280267091
str r0,=0xff08
mov r0,#0
str r0,=0xff0c
prints #65288
.L3:
add r2,r0,#1
sub r0,r2,r1
cmp #4,r0
beq .L4
b .L5
.L4:
mov r0,#1280267091
str r0,=0xff08
mov r0,#0
str r0,=0xff0c
prints #65288
.L5:
add r2,r0,#1
cmp r1,r2
beq .L6
b .L7
.L6:
mov r0,#1464421888
str r0,=0xff08
prints #65288
.L7:
add r2,r0,#1
sub r0,r2,r1
cmp #3,r0
beq .L8
b .L9
.L8:
mov r0,#1464421888
str r0,=0xff08
prints #65288
.L9:
mov r0,#1097756532
str r0,=0xff08
mov r0,#1751478816
str r0,=0xff0c
mov r0,#1734438245
str r0,=0xff10
mov r0,#1056964608
str r0,=0xff14
prints #65288
inputs r14
mov r0,r14
b .INP0
.INP1:
add r14,r14,#1
.INP0:
ldr r2,[r14]
cmp r2,#0
bne .INP1
add r14,r14,#2
mov r3,#0
b .STREQ0
.STREQ1
add r3,r3,#1
.STREQ0:
ldr r4,[r0,r3]
ldr r5,[r2,r3]
cmp r4,#0
beq .STREQ2
cmp r4,r5
beq .STREQ1
b .STREQ3
.STREQ2:
cmp r5,#0
bne .STREQ3
b game
.STREQ3:
andeq r0,r0,r0
