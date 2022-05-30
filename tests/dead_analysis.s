# push local variable in main(), i.e., i.
push

# call main
call 1

# Assignment
# load literal (rhs) to be assigned
ldc 1
# generate assignement codes
push
ldc 0
st
pop

# While Statement
# load vairiable index of i (0)
ldc 0
# acc = i
ld
# Generate i > 0
push
ldc 0  # rhs
gt     # op
pop
# jz END
jz 12
# Statements

ldc 0
ld
# acc = i
push
ldc 1
add
pop
# acc = i + 1
push
ldc 0
st
pop
# i = acc

# END:
jmp -17
