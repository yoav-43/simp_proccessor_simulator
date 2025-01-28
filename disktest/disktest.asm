read:
    out $zero, $zero, $imm2, $s0, 0, 15             # Set disksector = $s0
    out $zero, $zero, $imm2, $s1, 0, 16             # Set diskbuffer = $s1
    out $zero, $zero, $imm2, $imm1, 1, 14           # set diskcmd to 1 (read)
wait1:
    in $s2, $zero, $imm2, $zero, 0, 17              # set $s2 to diskstatus
    bne $zero, $s2, $zero, $imm1, wait1, 0          # loop until diskstatus == 0
    add $s1, $s1, $imm1, $zero, 128, 0              # Increment memory address for next sector
    add $s0, $s0, $imm1, $zero, 1, 0                # Increment sector index
    beq $zero, $s0, $imm1, $imm2, 8, init           # If $s0 == 8, go to init
    beq $zero, $zero, $zero, $imm2, 0, read         # Repeat read
init:
    add $s0 , $zero, $zero, $imm1, 1, 0             # set $s0 to 1 - write to sector 1
    add $s1, $zero, $zero, $zero, 0, 0              # set $s1 to 0 - return to first memory address
write:
    out $zero, $zero, $imm2, $s0, 0, 15             # Set disksector = $s0
    out $zero, $zero, $imm2, $s1, 0, 16             # Set diskbuffer = $s1
    out $zero, $zero, $imm2, $imm1, 2, 14           # set diskcmd to 2 (write)
wait2:
    in $s2, $zero, $imm2, $zero, 0, 17              # read diskstatus
    bne $zero, $s2, $zero, $imm1, wait2, 0          # loop until diskstatus == 0
    add $s1, $s1, $imm1, $zero, 128, 0              # Increment memory address for next sector
    add $s0, $s0, $imm1, $zero, 1, 0                # Increment write sector index
    beq $zero, $s0, $imm1, $imm2, 9, end            # If $s0 == 9, go to end
    beq $zero, $zero, $zero, $imm2, 0, write        # Repeat write
end:
    halt $zero, $zero, $zero, $zero, 0, 0           # Halt program