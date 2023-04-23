
fib:
  addi sp, sp, -12        # allocate stack space
  sw   ra, 0(sp)          # save return address
  sw   s0, 4(sp)          # save s0
  sw   s1, 8(sp)          # save s1
  li   s0, 0              # initialize a = 0
  li   s1, 1              # initialize b = 1
  li   t0, 2              # initialize i = 2
  li   t1, 10             # set limit to 10
loop:
  bgt  t0, t1, end        # if i > 10, exit loop
  add  t2, s0, s1         # c = a + b
  add  s0, zero, s1       # a = b
  add  s1, zero, t2       # b = c
  addi t0, t0, 1          # i++
  j    loop               # jump back to loop
end:
  mv   a0, s1             # return b