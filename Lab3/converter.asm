lui x8, 0xABCDE # s0 = 0xABCDE000
addi x8, x8, 0x123 # s0 = 0xABCDE123
addi x1,x0,0x300
sw x8,0(x1)

lb x9, 0(x1)
lb x18,1(x1)
lb x19,2(x1)
lb x20, 3(x1)

sb x20, 0(x1)
sb x19, 1(x1)
sb x18, 2(x1)
sb x9,  3(x1)


