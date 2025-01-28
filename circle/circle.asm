radius:
    lw $s0, $imm1, $zero, $zero, 0x100, 0       # s0 = radius
    mac $s0, $s0, $s0, $zero, 0, 0              # s0 = radius^2
xy:
    add $s1, $zero, $zero, $imm1, -128, 0       # s1 = x = -128
    add $s2, $zero, $zero, $imm1, 128, 0        # s1 = y = 128
dist:
    mac $t0, $s1, $s1, $zero, 0, 0              # t0 = x^2
    mac $t1, $s2, $s2, $zero, 0, 0              # t1 = y^2
    add $t2, $t0, $t1, $zero, 0, 0              # t2 = x^2 + y^2
    bgt $zero, $t2, $s0, $imm1, advanceX, 0     # if (x^2 + y^2) > radius^2 -> junp to advanceX
offset:
    sub $t0, $s2, $imm1, $zero, 128, 0          # t0 = y-128
    sll $t1, $t0, $imm1, $zero, 1, 0            # t1 = 2*(y-128)
    sub $t0, $t0, $t1, $zero, 0, 0              # t0 = -(y-128)
    sll $t0, $t0, $imm1, $zero, 8, 0            # t0 = -(y-128)*256
    add $t1, $s1, $imm1, $zero, 128, 0          # t1 = (x+128)
    add $t2, $t0, $t1, $zero, 0, 0              # t2 = (x+128)+256*(-(y-128))
out:
    out $zero, $imm1, $zero, $t2, 20, 0         # set adress to t2
    out $zero, $imm1, $zero, $imm2, 21, 255     # set data to 255=white
    out $zero, $imm1, $zero, $imm2, 22, 1       # set command to write
    out $zero, $imm1, $zero, $imm2, 22, 0       # set command to !write
advanceX:
    add $s1, $s1, $imm1, $zero, 1, 0            # x = x + 1
    ble $zero, $s1, $imm1, $imm2, 128, dist     # if x <= 128 -> jump to dist
    add $s1, $zero, $zero, $imm1, -128, 0       # init x = -128
advnaceY:
    sub $s2, $s2, $imm1, $zero, 1, 0            # y = y - 1
    bge $zero, $s2, $imm1, $imm2, -128, dist    # if y >= -128 -> jump to dist

    halt $zero, $zero, $zero, $zero 0, 0        # exit

    .word 0x100 30                              # write radius to address 0x100 






