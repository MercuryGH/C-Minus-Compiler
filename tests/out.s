push
call 25
ldc 0
ld
push
ldc 1
lte
pop
jz 3
ldc 1
jmp 15
ldc 0
ld
push
ldc 0
ld
push
ldc 1
sub
pop
push
call -19
pop
mul
pop
ret
ldc 0
push
ldc 0
st
pop
ldc 0
ld
push
ldc 12
lte
pop
jz 20
ldc 1
push
ldc 0
ld
push
call -41
pop
out
ldc 0
ld
push
ldc 1
add
pop
push
ldc 0
st
pop
jmp -25
