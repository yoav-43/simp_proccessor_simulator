	add $sp, $imm1, $imm2, $zero, 2047, 2047    # set stack pointer to 4094
	add $sp, $imm1, $sp, $zero, 1, 0            # set stack pointer to 4095
	lw $a0, $imm1, $zero, $zero, 0x100, 0	    # load n from address 0x100 into $a0
	lw $a1, $imm1, $zero, $zero, 0x101, 0	    # load k from address 0x101 into $a1
	blt $zero, $a0, $zero, $imm2, 0, end	    # edge case n < 0
	blt $zero, $a1, $zero, $imm2, 0, end	    # edge case k < 0
	jal $ra, $zero, $zero, $imm2, 0, binom	    # calculating $v0 = binom(n , k)
	sw $v0, $imm1, $zero, $zero, 0x102, 0	    # store the result in address 0x102
	beq $zero, $zero, $zero, $imm2, 0, end	    # finished the calculation
base:
	add $v0, $imm1, $zero, $zero, 1, 0	        # return 1
	beq $zero, $zero, $zero, $imm2, 0, exit	    # jump to exit
binom:
	sub $sp, $sp, $imm1, $zero, 4, 0		    # save space for 4 lines in the stack
	sw $zero, $sp, $imm1, $s0, 0, 0		        # save $s0 in the stack[0]
	sw $zero, $sp, $imm1, $ra, 1, 0		        # save $ra in the stack[1]
	sw $zero, $sp, $imm1, $a0, 2, 0		        # save $a0 in the stack[2]
	sw $zero, $sp, $imm1, $a1, 3, 0		        # save $a1 in the stack[3]

	beq $zero, $zero, $a1, $imm2, 0, base	    # if k==0 jump to base
	beq $zero, $a0, $a1, $imm2, 0, base	        # if n==k jump to base

	sub $a0, $a0, $imm1, $zero, 1, 0		    # n = n-1
	jal $ra, $zero, $zero, $imm2, 0, binom	    # calculating $v0 = binom(n-1 , k)
	add $s0, $v0, $zero, $zero, 0, 0		    # $s0 = binom(n-1 , k)
	add $v0, $zero, $zero, $zero, 0, 0          # reset $v0=0
	sub $a1, $a1, $imm1, $zero, 1, 0		    # k = k-1
	jal $ra, $zero, $zero, $imm2, 0, binom	    # calculating $v0 = binom(n-1 , k-1)
	add $v0, $v0, $s0, $zero, 0, 0			    # $v0 = binom(n-1 , k) + binom(n-1 , k-1)
exit:
	lw $a1, $sp, $imm1, $zero, 3, 0		        # load $a1 from stack[3]
	lw $a0, $sp, $imm1, $zero, 2, 0		        # load $a0 from stack[2]
	lw  $ra, $sp, $imm1, $zero, 1, 0		    # load $ra from stack[1]
	lw $s0, $sp, $imm1, $zero , 0, 0		    # load $s0 from stack[0]
	add $sp, $sp, $imm1, $zero, 4, 0		    # delete 4 lines from the stack
	beq $zero, $zero, $zero, $ra, 0, 0		    # return to the caller
end:
	halt $zero, $zero, $zero, $zero, 0, 0	    # halt - exit the simulator

    .word 0x100 6                               # write n to adsress 0x100 
    .word 0x101 4                               # write k to address 0x101 





