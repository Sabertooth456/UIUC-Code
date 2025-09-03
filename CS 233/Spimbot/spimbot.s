# syscall constants
PRINT_STRING            = 4
PRINT_CHAR              = 11
PRINT_INT               = 1

# memory-mapped I/O
VELOCITY                = 0xffff0010
ANGLE                   = 0xffff0014
ANGLE_CONTROL           = 0xffff0018

BOT_X                   = 0xffff0020
BOT_Y                   = 0xffff0024

OTHER_X                 = 0xffff00a0
OTHER_Y                 = 0xffff00a4

TIMER                   = 0xffff001c
GET_MAP                 = 0xffff2008
GET_TILE_INFO 			= 0xffff200c

REQUEST_PUZZLE          = 0xffff00d0  ## Puzzle
SUBMIT_SOLUTION         = 0xffff00d4  ## Puzzle

BONK_INT_MASK           = 0x1000
BONK_ACK                = 0xffff0060

TIMER_INT_MASK          = 0x8000
TIMER_ACK               = 0xffff006c

REQUEST_PUZZLE_INT_MASK = 0x800       ## Puzzle
REQUEST_PUZZLE_ACK      = 0xffff00d8  ## Puzzle

FALLING_INT_MASK        = 0x200
FALLING_ACK             = 0xffff00f4

STOP_FALLING_INT_MASK   = 0x400
STOP_FALLING_ACK        = 0xffff00f8

OUT_OF_WATER_INT_MASK 	= 0x2000	
OUT_OF_WATER_ACK 		= 0xffff00f0

POWERWASH_ON            = 0xffff2000
POWERWASH_OFF           = 0xffff2004

GET_WATER_LEVEL         = 0xffff201c

GET_SEED 				= 0xffff2040

MMIO_STATUS             = 0xffff204c

.data
### Puzzle
puzzlewrapper:     .byte 0:400
#### Puzzle

has_puzzle: .word 0

has_bonked: .byte 0
# -- string literals --
.text
main:
    sub $sp, $sp, 4
    sw  $ra, 0($sp)

    # Construct interrupt mask
    li      $t4, 0
    or      $t4, $t4, TIMER_INT_MASK            # enable timer interrupt
    or      $t4, $t4, BONK_INT_MASK             # enable bonk interrupt
    or      $t4, $t4, REQUEST_PUZZLE_INT_MASK   # enable puzzle interrupt
	or		$t4, $t4, OUT_OF_WATER_INT_MASK		# enable out of water interrupt
    or      $t4, $t4, 1 # global enable
    mtc0    $t4, $12
        
    # YOUR CODE GOES HERE!!!!!!
	
	lw	$t0, BOT_X
	li	$t1, 50
	bge	$t0, $t1, bot_1

bot_0:
    # {
		li	$a0, 20
		jal solve_puzzles

		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 4
		jal move_north_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
    
    # {
		li	$a0, 20
		jal solve_puzzles

		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 4
		jal move_north_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
    
    # {
		li	$a0, 20
		jal solve_puzzles

		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 4
		jal move_north_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
	
	# {
		li	$a0, 20
		jal solve_puzzles

		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 5
		jal move_east_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
    
    # {
		li	$a0, 20
		jal solve_puzzles

		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 4
		jal move_north_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
    
    # {
		li	$a0, 20
		jal solve_puzzles

		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 5
		jal move_north_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
    
    # {
		li	$a0, 20
		jal solve_puzzles

		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 4
		jal move_east_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
    
    # {
		li	$a0, 20
		jal solve_puzzles

		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 4
		jal move_east_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
    
    # {
		li	$a0, 20
		jal solve_puzzles

		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 4
		jal move_east_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
    
    # {
		li	$a0, 20
		jal solve_puzzles

		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 4
		jal move_east_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
    
    # {
		li	$a0, 20
		jal solve_puzzles

		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 4
		jal move_east_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
    
    # {
		li	$a0, 20
		jal solve_puzzles

		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 4
		jal move_east_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
    
    # {
		li	$a0, 20
		jal solve_puzzles

		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 4
		jal move_south_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
    
    # {
		li	$a0, 20
		jal solve_puzzles

		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 5
		jal move_south_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
    
    # {
		li	$a0, 20
		jal solve_puzzles

		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 4
		jal move_west_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
    
    # {
		li	$a0, 20
		jal solve_puzzles

		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 4
		jal move_west_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
    
    # {
		li	$a0, 20
		jal solve_puzzles

		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 4
		jal move_south_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
    
    # {
		li	$a0, 20
		jal solve_puzzles

		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 4
		jal move_south_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
    
    # {
		li	$a0, 20
		jal solve_puzzles

		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 4
		jal move_south_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
    
    # {
		li	$a0, 20
		jal solve_puzzles

		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 7
		jal move_west_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
    
    # {
		li	$a0, 20
		jal solve_puzzles
		
		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 4
		jal move_north_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
    
    # {
		li	$a0, 20
		jal solve_puzzles
		
		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 4
		jal move_north_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
    
    # {
		li	$a0, 20
		jal solve_puzzles
		
		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 4
		jal move_north_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
    
    # {
		li	$a0, 20
		jal solve_puzzles
		
		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 8
		jal move_west_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
    
    # {
		li	$a0, 60
		jal solve_puzzles
		
		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 20
		jal move_south_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
    
    # {		
		li	$a0, 200
		jal move_north_tiles
    # }
    
    j main_end

bot_1:
	# {
		li	$a0, 20
		jal solve_puzzles

		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 4
		jal move_north_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
    
    # {
		li	$a0, 20
		jal solve_puzzles

		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 4
		jal move_north_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
    
    # {
		li	$a0, 20
		jal solve_puzzles

		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 4
		jal move_north_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
	
	# {
		li	$a0, 20
		jal solve_puzzles

		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 5
		jal move_west_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
    
    # {
		li	$a0, 20
		jal solve_puzzles

		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 4
		jal move_north_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
    
    # {
		li	$a0, 20
		jal solve_puzzles

		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 5
		jal move_north_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
    
    # {
		li	$a0, 20
		jal solve_puzzles

		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 4
		jal move_west_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
    
    # {
		li	$a0, 20
		jal solve_puzzles

		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 4
		jal move_west_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
    
    # {
		li	$a0, 20
		jal solve_puzzles

		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 4
		jal move_west_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
    
    # {
		li	$a0, 20
		jal solve_puzzles

		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 4
		jal move_west_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
    
    # {
		li	$a0, 20
		jal solve_puzzles

		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 4
		jal move_west_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
    
    # {
		li	$a0, 20
		jal solve_puzzles

		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 4
		jal move_west_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
    
    # {
		li	$a0, 20
		jal solve_puzzles

		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 4
		jal move_south_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
    
    # {
		li	$a0, 20
		jal solve_puzzles

		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 5
		jal move_south_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
    
    # {
		li	$a0, 20
		jal solve_puzzles

		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 4
		jal move_east_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
    
    # {
		li	$a0, 20
		jal solve_puzzles

		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 4
		jal move_east_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
    
    # {
		li	$a0, 20
		jal solve_puzzles

		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 4
		jal move_south_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
    
    # {
		li	$a0, 20
		jal solve_puzzles

		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 4
		jal move_south_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
    
    # {
		li	$a0, 20
		jal solve_puzzles

		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 4
		jal move_south_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
    
    # {
		li	$a0, 20
		jal solve_puzzles

		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 7
		jal move_east_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
    
    # {
		li	$a0, 20
		jal solve_puzzles
		
		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 4
		jal move_north_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
    
    # {
		li	$a0, 20
		jal solve_puzzles
		
		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 4
		jal move_north_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
    
    # {
		li	$a0, 20
		jal solve_puzzles
		
		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 4
		jal move_north_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
    
    # {
		li	$a0, 20
		jal solve_puzzles
		
		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 8
		jal move_east_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
    
    # {
		li	$a0, 60
		jal solve_puzzles
		
		li $t4, 0x00040000							# Turn on the water
		sw $t4, POWERWASH_ON
		
		li	$a0, 20
		jal move_south_tiles
		
		li $t4, 0x00000001
		sw $t4, POWERWASH_OFF
    # }
    
    # {		
		li	$a0, 200
		jal move_north_tiles
    # }

	j main_end

main_end:
    
loop: # Once done, enter an infinite loop so that your bot can be graded by QtSpimbot once 10,000,000 cycles have elapsed
    j loop
    
j func_definition_end


##################################################
solve_puzzles:								# Solves the specified number of puzzles ($a0)
	sub	$sp, $sp, 24							# Allocate 6 word(s) of memory from the stack
	sw	$ra, 0($sp)								# Store the $return address
	sw	$a0, 4($sp)								# Store the initial $a0
	sw	$a1, 8($sp)								# Store the initial $a1
	sw	$a2, 12($sp)							# Store the initial $a2
	sw	$a3, 16($sp)							# Store the initial $a3
	sw	$s0, 20($sp)							# Store the initial $s0
	
	li	$s0, 0									# $s0: int i = 0
solve_puzzles_for:								# for (int i = 0; i < $a0; i++)
		bge $s0, $a0, solve_puzzles_for_end			# Branch to end if ($s0 >= $a0)
		
		sw	$0, has_puzzle							# *(has_puzzle) = 0
			# This stores the address of puzzlewrapper and in $s1 and REQUEST_PUZZLE
		la	$s1, puzzlewrapper						# $s1 = &puzzlewrapper
		sw 	$s1, REQUEST_PUZZLE						# *(REQUEST_PUZZLE) = $s1 = &puzzlewrapper
		
	puzzle_interrupt_while:							# while(has_puzzle == 0)
		lw	$t0, has_puzzle							# $t0 = has_puzzle
		beq	$t0, $0, puzzle_interrupt_while			# Branch to while if $t0 == 0
		
		
		lw	$a0, 4($s1)								# char **board
		lw 	$a1, 0($s1)								# unsigned int board_size
		lw	$a2, 8($s1)								# list_node_t *given_queens
		lw	$a3, 12($s1)							# unsigned int queens_to_place
		jal solve_queens							# solve_queens(board, board_size, given_queens, queens_to_place)
		lw	$a0, 4($sp)								# Restore the initial $a0
		lw	$a1, 8($sp)								# Restore the initial $a1
		lw	$a2, 12($sp)							# Restore the initial $a2
		lw	$a3, 16($sp)							# Restore the initial $a3
		
		sw	$s1, SUBMIT_SOLUTION					# *(SUBMIT_SOLUTION) = $s1 = &puzzlewrapper
		
		add	$s0, $s0, 1								# i++
		j solve_puzzles_for							# Loop
solve_puzzles_for_end:
	
	lw 	$s0, 20($sp)							# Restore the initial $s0
	lw	$a3, 16($sp)							# Restore the initial $a3
	lw	$a2, 12($sp)							# Restore the initial $a2
	lw	$a1, 8($sp)								# Restore the initial $a1
	lw 	$a0, 4($sp)								# Restore the initial $a0
    lw 	$ra, 0($sp)								# Restore the $return address
    add $sp, $sp, 24							# Deallocate 6 word(s) of memory from the stack
    jr 	$ra										# return
##################################################


##################################################
# wait(cycles)
wait:										# Waits for specified amount of cycles to elapse ($a0)
	sub $sp, $sp, 4								# Allocate 1 word of memory to the stack
	sw 	$ra, 0($sp)								# Store the $return address
	
	lw	$t0, TIMER								# $t0 = current time
	add	$t1, $t0, $a0							# $t1 = Stop_Time
continue_wait_loop:								# A for loop that runs for the specified amount of cycles
		lw	$t0, TIMER								# $t0 = current time
		ble	$t1, $t0 wait_loop_end					# Branch to wait_loop_end if Stop_Time <= TIMER
		j continue_wait_loop
wait_loop_end:

	lw 	$ra, 0($sp)								# Load the $return address
	add $sp, $sp, 4								# Deallocate 1 word of memory from the stack
	jr	$ra										# return
##################################################


##################################################
# move_absolute(direction)
move_absolute:								# Move 1 pixel in the specified direction ($a0)
	sub $sp, $sp, 8								# Allocate 5 words of memory from the stack
	sw 	$ra, 0($sp)								# Store the $return address
	sw 	$a0, 4($sp)								# Store the initial $a0
	
	move $t0, $a0								# $t0 = $a0 degrees
	sw 	$t0, ANGLE								# ANGLE = $a0 degrees
	li 	$t0, 1									# 1
	sw 	$t0, ANGLE_CONTROL						# ANGLE_CONTROL = absolute angle
	li 	$t0, 10									# 10 pixel per 10,000 cycles
	sw 	$t0, VELOCITY							# VELOCITY = 10 pixel per 10,000 cycles
	
	li	$a0, 1000								# $a0 = 1,000
	jal wait									# wait(1000)
	lw 	$a0, 4($sp)								# Restore the initial $a0
	
												# Reset the controls
	sw	$0, ANGLE								# ANGLE = 0
	sw	$0, ANGLE_CONTROL						# ANGLE_CONTROL = 0
	sw	$0, VELOCITY							# VELOCITY = 0
	
	lw	$a0, 4($sp)								# Restore the initial $a0
	lw 	$ra, 0($sp)								# Restore the $return address
	add $sp, $sp, 8								# Deallocate 5 words of memory from the stack
	jr	$ra										# return
##################################################


##################################################
# move_steps(direction, number_of_pixels)
move_steps:									# Move ($a1) pixels in the specified direction ($a0)
	sub $sp, $sp, 16							# Allocate 4 word(s) of memory from the stack
	sw	$ra, 0($sp)								# Store the $return address
	sw	$a0, 4($sp)								# Store the initial $a0
	sw	$a1, 8($sp)								# Store the initial $a1
	sw	$s0, 12($sp)							# Store the intitial $s0
	
	li	$s0, 0									# $s0 = 0
move_steps_for:									# for ($s0 = 0; $s0 < $a1; $s0++)
		bge	$s0, $a1, move_steps_end

		move $a0, $a0								# Point towards ($a0) degrees
		jal move_absolute							# move_absolute($a0)
		lw	$a0, 4($sp)								# Restore the initial $a0
		
		add $s0, $s0, 1
		j move_steps_for
move_steps_end:

	lw	$s0, 12($sp)							# Restore the intitial $s0
	lw	$a1, 8($sp)								# Restore the initial $a1
	lw	$a0, 4($sp)								# Restore the initial $a0
	lw	$ra, 0($sp)								# Restore the $return address
	add	$sp, $sp, 16							# Deallocate 4 word(s) of memory from the stack
	jr	$ra										# return
##################################################


##################################################
# move_east_pixels(number_of_pixels)
move_east_pixels:							# Move ($a0) pixels east
	sub $sp, $sp, 12							# Allocate 3 word(s) of memory from the stack
	sw	$ra, 0($sp)								# Store the $return address
	sw	$a0, 4($sp)								# Store the initial $a0
	sw	$a1, 8($sp)								# Store the initial $a1
	
	move $a1, $a0								# $a1 = $a0
	li	$a0, 0									# $a0 = 0 degrees
	jal move_steps								# move_steps(0, $a0)
	lw $a1, 8($sp)								# Restore the initial $a1
	lw $a0, 4($sp)								# Restore the initial $a0
	
	lw	$a1, 8($sp)								# Restore the initial $a1
	lw	$a0, 4($sp)								# Restore the initial $a0
	lw	$ra, 0($sp)								# Restore the $return address
	add	$sp, $sp, 12							# Deallocate 3 word(s) of memory from the stack
	jr	$ra	
##################################################


##################################################
# move_west_pixels(number_of_pixels)
move_west_pixels:							# Move ($a0) pixels west
	sub $sp, $sp, 12							# Allocate 3 word(s) of memory from the stack
	sw	$ra, 0($sp)								# Store the $return address
	sw	$a0, 4($sp)								# Store the initial $a0
	sw	$a1, 8($sp)								# Store the initial $a1
	
	move $a1, $a0								# $a1 = $a0
	li	$a0, 180								# $a0 = 180 degrees
	jal move_steps								# move_steps(180, $a0)
	lw $a1, 8($sp)								# Restore the initial $a1
	lw $a0, 4($sp)								# Restore the initial $a0
	
	lw	$a1, 8($sp)								# Restore the initial $a1
	lw	$a0, 4($sp)								# Restore the initial $a0
	lw	$ra, 0($sp)								# Restore the $return address
	add	$sp, $sp, 12							# Deallocate 3 word(s) of memory from the stack
	jr	$ra	
##################################################


##################################################
# move_north_pixels(number_of_pixels)
move_north_pixels:							# Move ($a0) pixels north
	sub $sp, $sp, 12							# Allocate 3 word(s) of memory from the stack
	sw	$ra, 0($sp)								# Store the $return address
	sw	$a0, 4($sp)								# Store the initial $a0
	sw	$a1, 8($sp)								# Store the initial $a1
	
	move $a1, $a0								# $a1 = $a0
	li	$a0, 270								# $a0 = 270 degrees
	jal move_steps								# move_steps(270, $a0)
	lw $a1, 8($sp)								# Restore the initial $a1
	lw $a0, 4($sp)								# Restore the initial $a0
	
	lw	$a1, 8($sp)								# Restore the initial $a1
	lw	$a0, 4($sp)								# Restore the initial $a0
	lw	$ra, 0($sp)								# Restore the $return address
	add	$sp, $sp, 12							# Deallocate 3 word(s) of memory from the stack
	jr	$ra	
##################################################


##################################################
# move_south_pixels(number_of_pixels)
move_south_pixels:							# Move ($a0) pixels south
	sub $sp, $sp, 12							# Allocate 3 word(s) of memory from the stack
	sw	$ra, 0($sp)								# Store the $return address
	sw	$a0, 4($sp)								# Store the initial $a0
	sw	$a1, 8($sp)								# Store the initial $a1
	
	move $a1, $a0								# $a1 = $a0
	li	$a0, 90									# $a0 = 90 degrees
	jal move_steps								# move_steps(90, $a0)
	lw $a1, 8($sp)								# Restore the initial $a1
	lw $a0, 4($sp)								# Restore the initial $a0
	
	lw	$a1, 8($sp)								# Restore the initial $a1
	lw	$a0, 4($sp)								# Restore the initial $a0
	lw	$ra, 0($sp)								# Restore the $return address
	add	$sp, $sp, 12							# Deallocate 3 word(s) of memory from the stack
	jr	$ra	
##################################################


##################################################
# move_east_tiles(number_of_tiles)			# Move ($a0) tiles east
move_east_tiles:
	sub	$sp, $sp, 8								# Allocate 2 word(s) of memory from the stack
	sw	$ra, 0($sp)								# Store the $return address
	sw	$a0, 4($sp)								# Store the initial $a0
	
	mul	$a0, $a0, 8								# number_of_pixels = number_of_tiles * 8
	jal move_east_pixels						# move_east_pixels(number_of_pixels)
	
	lw	$a0, 4($sp)								# Restore the initial $a0
	lw	$ra, 0($sp)								# Restore the $return address
	add	$sp, $sp, 8								# Deallocate 2 word(s) of memory from the stack
	jr	$ra
##################################################


##################################################
# move_west_tiles(number_of_tiles)			# Move ($a0) tiles west
move_west_tiles:
	sub	$sp, $sp, 8								# Allocate 2 word(s) of memory from the stack
	sw	$ra, 0($sp)								# Store the $return address
	sw	$a0, 4($sp)								# Store the initial $a0
	
	mul	$a0, $a0, 8								# number_of_pixels = number_of_tiles * 8
	jal move_west_pixels						# move_west_pixels(number_of_pixels)
	
	lw	$a0, 4($sp)								# Restore the initial $a0
	lw	$ra, 0($sp)								# Restore the $return address
	add	$sp, $sp, 8								# Deallocate 2 word(s) of memory from the stack
	jr	$ra
##################################################


##################################################
# move_north_tiles(number_of_tiles)			# Move ($a0) tiles north
move_north_tiles:
	sub	$sp, $sp, 8								# Allocate 2 word(s) of memory from the stack
	sw	$ra, 0($sp)								# Store the $return address
	sw	$a0, 4($sp)								# Store the initial $a0
	
	mul	$a0, $a0, 8								# number_of_pixels = number_of_tiles * 8
	jal move_north_pixels						# move_north_pixels(number_of_pixels)
	
	lw	$a0, 4($sp)								# Restore the initial $a0
	lw	$ra, 0($sp)								# Restore the $return address
	add	$sp, $sp, 8								# Deallocate 2 word(s) of memory from the stack
	jr	$ra
##################################################


##################################################
# move_south_tiles(number_of_tiles)			# Move ($a0) tiles south
move_south_tiles:
	sub	$sp, $sp, 8								# Allocate 2 word(s) of memory from the stack
	sw	$ra, 0($sp)								# Store the $return address
	sw	$a0, 4($sp)								# Store the initial $a0
	
	mul	$a0, $a0, 8								# number_of_pixels = number_of_tiles * 8
	jal move_south_pixels						# move_south_pixels(number_of_pixels)
	
	lw	$a0, 4($sp)								# Restore the initial $a0
	lw	$ra, 0($sp)								# Restore the $return address
	add	$sp, $sp, 8								# Deallocate 2 word(s) of memory from the stack
	jr	$ra
##################################################


##################################################
.text
.globl is_attacked
is_attacked:
    li $t0,0 #counter i=0
    li $t1,0 #counter j=0
    
    move $t2,$a1 #counter N
    j forloopvertical
    
forloopvertical:
    bge $t0,$t2,forloophorizontal  # if i >= n move on to next for loop
    bne $t0,$a2,verticalcheck  #checking i != row, if i != row move onto next check
    add $t0,$t0,1  # incrementing i = i+1
    j forloopvertical # jump back to for
    
verticalcheck:
    mul $t3, $t0, 4     # convert index to offset address for row
    add $t4, $a0, $t3   # add offset to base address of board
    lw  $t5, 0($t4)     # load address of board[row(i)] in $t5, $t5 is pointing to the beginning of the char*
    add $t6, $t5, $a3   # add offset to base address of board[row(i)]
    lb  $t7, 0($t6)     # load board[row(i)][col] in $t7
    beq $t7,1,return1   # if board[i][col] == 1 return 1
    add $t0,$t0,1       # increment i = i+1
    j forloopvertical   # jump to for loop

forloophorizontal:
    bge $t1,$t2,resetiandjleft  # if j >= n move on to next for loop
    bne $t1,$a3,horizontalcheck  #checking j != col, if j != col move onto next check
    add $t1,$t1,1  # incrementing j = j+1
    j forloophorizontal # jump back to for
    
horizontalcheck:
    mul $t3, $a2, 4     # convert index to offset address for row
    add $t4, $a0, $t3   # add offset to base address of board
    lw  $t5, 0($t4)     # load address of board[row] in $t5, $t5 is pointing to the beginning of the char*
    add $t6, $t5, $t1   # add offset to base address of board[row]
    lb  $t7, 0($t6)     # load board[row][col(j)] in $t7
    beq $t7,1,return1   # if board[row][j] == 1 return 1
    add $t1,$t1,1       # increment j = j+1
    j forloophorizontal   # jump to for loop

resetiandjleft:
    li $t0,0    # i = 0
    li $t1,0    # j = 0
    j forleftdiagonal

forleftdiagonal:
    bge $t0,$t2,resetiandjright #for int i = 0; i <n; i++
    beq $t0,$a2,incrementileft # (i != row)
    
    sub $t3,$t0,$a2
    add $t1,$t3,$a3 #int j = (i-row) + col
    
    blt $t1,0,incrementileft # j>=0
    bge $t1,$t2,incrementileft # j < n
    
    mul $t3, $t0, 4     # convert index to offset address for row
    add $t4, $a0, $t3   # add offset to base address of board
    lw  $t5, 0($t4)     # load address of board[row] in $t5, $t5 is pointing to the beginning of the char*
    add $t6, $t5, $t1   # add offset to base address of board[row]
    lb  $t7, 0($t6)     # load board[row][col(j)] in $t7
    beq $t7,1,return1   # if board[row][j] == 1 return 1
    
    add $t0,$t0,1
    j forleftdiagonal

incrementileft:
    add $t0,$t0,1
    j forleftdiagonal
    

resetiandjright:
    li $t0,0
    li $t1,0
    j forrightdiagonal

forrightdiagonal:
    bge $t0,$t2,return0 #for int i = 0; i <n; i++
    beq $t0,$a2,incrementiright # (i != row)
    
    sub $t3,$a2,$t0
    add $t1,$t3,$a3 #int j = (row-i) + col
    
    blt $t1,0,incrementiright # j>=0
    bge $t1,$t2,incrementiright # j < n
    
    mul $t3, $t0, 4     # convert index to offset address for row
    add $t4, $a0, $t3   # add offset to base address of board
    lw  $t5, 0($t4)     # load address of board[row] in $t5, $t5 is pointing to the beginning of the char*
    add $t6, $t5, $t1   # add offset to base address of board[row]
    lb  $t7, 0($t6)     # load board[row][col(j)] in $t7
    beq $t7,1,return1   # if board[row][j] == 1 return 1
    
    add $t0,$t0,1
    j forrightdiagonal

incrementiright:
    add $t0,$t0,1
    j forrightdiagonal
    
return1:
    li $v0,1            # output 1
    jr $ra              # return

return0:
    li $v0,0            # output 0
    jr $ra              # return
##################################################


##################################################
.globl place_queen_step
place_queen_step:
    li      $v0, 1
    beq     $a3, $0, pqs_return     # if (queens_left == 0)

pqs_prologue: 
    sub     $sp, $sp, 36 
    sw      $s0, 0($sp)
    sw      $s1, 4($sp)
    sw      $s2, 8($sp)
    sw      $s3, 12($sp)
    sw      $s4, 16($sp)
    sw      $s5, 20($sp)
    sw      $s6, 24($sp)
    sw      $s7, 28($sp)
    sw      $ra, 32($sp)

    move    $s0, $a0                # $s0 = board
    move    $s1, $a1                # $s1 = n
    move    $s2, $a2                # $s2 = pos
    move    $s3, $a3                # $s3 = queens_left

    move    $s4, $a2                # $s4 = i = pos

pqs_for:
    mul     $t0, $s1, $s1           # $t0 = n * n
    bge     $s4, $t0, pqs_for_end   # break out of loop if !(i < n * n)

    div     $s5, $s4, $s1           # $s5 = row = i / n
    rem     $s6, $s4, $s1           # $s6 = col = i % n

    sll     $s7, $s5, 2             # $s7 = row * 4
    add     $s7, $s7, $s0           # $s7 = &board[row] = board + row * 4
    lw      $s7, 0($s7)             # $s7 = board[row]

    add     $s7, $s7, $s6           # $s7 = &board[row][col] = board[row] + col
    lb      $t1, 0($s7)             # $t1 = board[row][col]

    bne     $t1, $0, pqs_for_inc    # skip if !(board[row][col] == 0)

    move    $a0, $s0                # board
    move    $a1, $s1                # n
    move    $a2, $s5                # row
    move    $a3, $s6                # col
    jal     is_attacked             # call is_attacked(board, n, row, col)

    bne     $v0, $0, pqs_for_inc    # skip if !(is_attacked(board, n, row, col) == 0)

    li      $t0, 1
    sb      $t0, 0($s7)             # board[row][col] = 1

    move    $a0, $s0                # board
    move    $a1, $s1                # n
    add     $a2, $s2, 1             # pos + 1
    sub     $a3, $s3, 1             # queens_left - 1
    jal     place_queen_step        # call place_queen_step(board, n, pos + 1, queens_left - 1)

    beq     $v0, $0, pqs_reset_square       # skip return if !(place_queen_step(board, n, pos + 1, queens_left - 1) == 0)

    li      $v0, 1
    j       pqs_epilogue            # return 1

pqs_reset_square:
    sb      $0, 0($s7)              # board[row][col] = 0

pqs_for_inc:
    add     $s4, $s4, 1             # ++i
    j       pqs_for

pqs_for_end:
    move    $v0, $0                  # return 0

pqs_epilogue:
    lw      $s0, 0($sp)
    lw      $s1, 4($sp)
    lw      $s2, 8($sp)
    lw      $s3, 12($sp)
    lw      $s4, 16($sp)
    lw      $s5, 20($sp)
    lw      $s6, 24($sp)
    lw      $s7, 28($sp)
    lw      $ra, 32($sp)
    add     $sp, $sp, 36 

pqs_return:
    jr      $ra
##################################################


##################################################
.globl solve_queens
solve_queens:
sq_prologue:
    sub     $sp, $sp, 20
    sw      $s0, 0($sp)
    sw      $s1, 4($sp)
    sw      $s2, 8($sp)
    sw      $s3, 12($sp)
    sw      $ra, 16($sp)

    move    $s0, $a0
    move    $s1, $a1
    move    $s2, $a2
    move    $s3, $a3

    li      $t0, 0      # $t0 is i

sq_for_i:
    beq     $t0, $s1, sq_end_for_i
    li      $t1, 0      # $t1 is j

sq_for_j:
    beq     $t1, $s1, sq_end_for_j

    sll     $t3, $t0, 2             # $t3 = i * 4
    add     $t3, $t3, $s0           # $t3 = &board[i] = board + i * 4
    lw      $t3, 0($t3)             # $t3 = board[i]

    add     $t3, $t3, $t1           # $t3 = &board[i][j] = board[i] + j
    sb      $0, 0($t3)              # board[i][j] = 0

    add     $t1, $t1, 1     # ++j
    j       sq_for_j

sq_end_for_j:
    add     $t0, $t0, 1     # ++i
    j       sq_for_i

sq_end_for_i:
sq_ll_setup:
    move    $t5, $a2        # $t5 is curr

sq_ll_for:
    beq     $t5, $0, sq_ll_end
    
    lw      $t6, 0($t5)         # $t6 = curr->pos
    div     $t0, $t6, $s1       # $t0 = row = pos / n
    rem     $t1, $t6, $s1       # $t1 = col = pos % n
    
    sll     $t3, $t0, 2             # $t3 = row * 4
    add     $t3, $t3, $s0           # $t3 = &board[row] = board + row * 4
    lw      $t3, 0($t3)             # $t3 = board[row]

    add     $t3, $t3, $t1           # $t3 = &board[row][col] = board[row] + col
    li      $t7, 1
    sb      $t7, 0($t3)             # board[row][col] = 1

    lw      $t5, 4($t5)             # curr = curr->next

    j       sq_ll_for

sq_ll_end:
    move    $a2, $0
    jal     place_queen_step        # call place_queen_step(sol_board, n, 0, queens_to_place)

sq_epilogue:
    lw      $s0, 0($sp)
    lw      $s1, 4($sp)
    lw      $s2, 8($sp)
    lw      $s3, 12($sp)
    lw      $ra, 16($sp)

    add     $sp, $sp, 20
    jr      $ra
##################################################

func_definition_end:

.kdata
chunkIH:    .space 40
non_intrpt_str:    .asciiz "Non-interrupt exception\n"
unhandled_str:    .asciiz "Unhandled interrupt type\n"
.ktext 0x80000180
interrupt_handler:
.set noat
    move    $k1, $at        # Save $at
                            # NOTE: Don't touch $k1 or else you destroy $at!
.set at
    la      $k0, chunkIH
    sw      $a0, 0($k0)        # Get some free registers
    sw      $v0, 4($k0)        # by storing them to a global variable
    sw      $t0, 8($k0)
    sw      $t1, 12($k0)
    sw      $t2, 16($k0)
    sw      $t3, 20($k0)
    sw      $t4, 24($k0)
    sw      $t5, 28($k0)

    # Save coprocessor1 registers!
    # If you don't do this and you decide to use division or multiplication
    #   in your main code, and interrupt handler code, you get WEIRD bugs.
    mfhi    $t0
    sw      $t0, 32($k0)
    mflo    $t0
    sw      $t0, 36($k0)

    mfc0    $k0, $13                # Get Cause register
    srl     $a0, $k0, 2
    and     $a0, $a0, 0xf           # ExcCode field
    bne     $a0, 0, non_intrpt



interrupt_dispatch:                 # Interrupt:
    mfc0    $k0, $13                # Get Cause register, again
    beq     $k0, 0, done            # handled all outstanding interrupts

    and     $a0, $k0, BONK_INT_MASK     # is there a bonk interrupt?
    bne     $a0, 0, bonk_interrupt

    and     $a0, $k0, TIMER_INT_MASK    # is there a timer interrupt?
    bne     $a0, 0, timer_interrupt

    and     $a0, $k0, REQUEST_PUZZLE_INT_MASK
    bne     $a0, 0, request_puzzle_interrupt

    and     $a0, $k0, FALLING_INT_MASK
    bne     $a0, 0, falling_interrupt

    and     $a0, $k0, STOP_FALLING_INT_MASK
    bne     $a0, 0, stop_falling_interrupt
	
	and		$a0, $k0, OUT_OF_WATER_INT_MASK
	bne		$a0, 0, out_of_water_interrupt

    li      $v0, PRINT_STRING       # Unhandled interrupt types
    la      $a0, unhandled_str
    syscall
    j       done

bonk_interrupt:
    sw      $0, BONK_ACK
    la      $t0, has_bonked
    li      $t1, 1
    sb      $t1, 0($t0)
    #Fill in your bonk handler code here
    j       interrupt_dispatch      # see if other interrupts are waiting

timer_interrupt:
    sw      $0, TIMER_ACK
    #Fill your timer interrupt code here
    j        interrupt_dispatch     # see if other interrupts are waiting

request_puzzle_interrupt:
    sw      $0, REQUEST_PUZZLE_ACK
    #Fill in your puzzle interrupt code here
    li 	$t0, 1
    sw	$t0, has_puzzle
    
    j       interrupt_dispatch

falling_interrupt:
    sw      $0, FALLING_ACK
    #Fill in your falling interrupt handler code here
    j       interrupt_dispatch

stop_falling_interrupt:
    sw      $0, STOP_FALLING_ACK
    #Fill in your stop falling interrupt handler code here
    j       interrupt_dispatch

out_of_water_interrupt:
	sw		$0, OUT_OF_WATER_ACK
	#Fill in your out of water interrupt handler code here
    j       interrupt_dispatch

non_intrpt:                         # was some non-interrupt
    li      $v0, PRINT_STRING
    la      $a0, non_intrpt_str
    syscall                         # print out an error message
    # fall through to done

done:
    la      $k0, chunkIH

    # Restore coprocessor1 registers!
    # If you don't do this and you decide to use division or multiplication
    #   in your main code, and interrup 	t handler code, you get WEIRD bugs.
    lw      $t0, 32($k0)
    mthi    $t0
    lw      $t0, 36($k0)
    mtlo    $t0

    lw      $a0, 0($k0)             # Restore saved registers
    lw      $v0, 4($k0)
    lw      $t0, 8($k0)
    lw      $t1, 12($k0)
    lw      $t2, 16($k0)
    lw      $t3, 20($k0)
    lw      $t4, 24($k0)
    lw      $t5, 28($k0)

.set noat
    move    $at, $k1        # Restore $at
.set at
    eret
