push
call 25

# fact()
ldc 0
ld
# acc = n

push
ldc 1
lte
pop
# acc = (n <= 1)

# jz END
jz 3

# return 1
ldc 1
jmp 15

# END:
ldc 0
ld
# acc = n
push
# top = n
ldc 0
ld
push
ldc 1
sub
pop
# acc = n - 1

push
call -19
# call fact(). acc = fact(n - 1)

pop
mul
pop
# acc = n * fact(n - 1)
ret
