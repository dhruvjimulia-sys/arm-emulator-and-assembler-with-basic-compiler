mov r0,#7
str r0,#65532
ldr r1,#65532
mul r1.r1,#2
add r1,r1,#0
b .RND0:
.RND1:
sub r1,r1,#2
.RND0:
cmp r1,#2
bge .RND1
str r1,#65532
mul r2,r1,#2
add r1,#1,r2
mov r0,r1
andeq r0,r0,r0
