import random
n = 90
with open('test.in', 'w') as testFile:
    testFile.write('{0}\n'.format(n))
    for i in range(n):
        testFile.write(str(random.randint(1, 10000)) + ' ')
