# Read the following instructions carefully
#
# You will provide your solution to this part of the project by
# editing the collection of functions in this source file.
#
# Some rules from the C bitwise puzzles are still in effect for your assembly
# code here:
#  1. No global variables are allowed
#  2. You may not define or call any additional functions in this file
#  3. You may not use any floating-point assembly instructions
#
# You may assume that your machine:
#  1. Uses two's complement, 32-bit representations of integers.
#  2. Has unpredictable behavior when shifting if the shift amount
#     is less than 0 or greater than 31.

# TO AVOID GRADING SURPRISES:
#   Pay attention to the results of the call_cc script, which is run
#   as part of the provided tests.
#
#   This makes sure you have adhered to the x86-64 calling convention
#   in your assembly code. If it reports any errors, make sure to fix
#   them before you submit your code.
#
# YOU WILL RECEIVE NO CREDIT IF YOUR CODE DOES NOT PASS THIS CHECK

# isZero - returns 1 if x == 0, and 0 otherwise
#   Examples: isZero(5) = 0, isZero(0) = 1
#   Rating: 1

# For copy and paste:
# qemu-x86_64 ./btest

.global isZero
isZero:
    movl %edi, %eax     # Moves first argument into return register
    cmpl $0, %eax       # Compare eax to value 0
    jz equalsZero       # If eax is zero, jump to equalsZero label
    movl $0, %eax       # Assign 0 as return value
    ret

equalsZero:
    movl $1, %eax       # Assign return value as 1
    ret


# bitNor - Compute ~(x|y)
#   Example: bitNor(0x6, 0x5) = 0xFFFFFFF8
#   Rating: 1
.global bitNor
bitNor:
    # Solution adapted from project 2's bitNor (~x & ~y)
    movl %edi, %eax     # Moves first arg. to ret. register
    movl %esi, %r8d     # Moves second arg. to a caller-saved register
    notl %r8d           # Inverts the bits of value stored in r8d
    notl %eax           # Inverts the bits of value stored in eax
    andl %r8d, %eax     # Performs bitwise AND on eax and r8d, stores result in eax
    ret


# leastBitPos - return a mask that marks the position of the
#               least significant 1 bit. If x == 0, return 0
#   Example: leastBitPos(96) = 0x20
#   Rating: 2
.global leastBitPos
leastBitPos:
    # Solution adapted from P2's leastBitPos (x & (~x + 1))
    movl %edi, %eax     # Move arg. 1's value to eax
    movl %edi, %r8d     # Also move to r8d
    negl %r8d           # Perform two's complement negation to value in r8d
    andl %r8d, %eax     # Perform bitwise AND to r8d and eax, store result in eax
    ret


# copyLSB - set all bits of result to least significant bit of x
#   Example: copyLSB(5) = 0xFFFFFFFF, copyLSB(6) = 0x00000000
#   Rating: 2
.global copyLSB
copyLSB:
    # ~(1 & x) + 1
    movl %edi, %eax     # Move arg. 1 to eax
    andl $1, %eax       # Perform bitwise AND on value 1 and value in eax
    notl %eax           # Flip bits of value in eax
    addl $1, %eax       # Add 1 to value in eax
    ret


# distinctNegation - returns 1 if x != -x.
#     and 0 otherwise
#   Rating: 2
.global distinctNegation
distinctNegation:
    # (x ^ (~x + 1)) -> 1 if nonzero, 0 if 0
    movl %edi, %eax     # Move arg. 1 value to eax
    movl %edi, %r8d     # Also move arg. 1 value to r8d
    negl %r8d           # Invert value in r8d to get negative value
    xorl %r8d, %eax     # Perform bitwise XOR on eax and -eax, save value to eax
    cmpl $0, %eax       # Check to see if nonzero...
    jz isNotNonZero     # ...if not, jump and return 0...
    movl $1, %eax       # ...else, return 1
    ret
isNotNonZero:
    ret                 # Simply return 0 value in eax


# floatAbsVal - Return bit-level equivalent of absolute value of f for
#   floating point argument f.
#   Both the argument and result are passed as unsigned int's, but
#   they are to be interpreted as the bit-level representations of
#   single-precision floating point values.
#   When argument is NaN, return argument.
#   Rating: 2
.global floatAbsVal
floatAbsVal:
    movl %edi, %eax     # Move arg. 1 value to return reg.

    # Also store a copy of arg. 1 to r8d, r9d
    movl %edi, %r8d     # Will store exponent bits
    movl %edi, %r9d     # Will store mantissa bits

    # Manipulate float bits to get their respective bit representations
    shr $23, %r8d               # Shift bits in r8d right by 23
    andl $0xFF, %r8d            # Mask r8d with 0xFF to get the exponent bits

    andl $0x7fffff, %r9d        # Mask r9d with 0x7FFFFF to get the mantissa bits

    # First NaN check: exponent not at max value
    cmpl $0xff, %r8d        # Compare exponent value to max 2 byte value
    jne nanCheckCleared     # If exponent value is not at the maximum value, it passes the check. Jump.
                            # Else, move on to the next check.

    # Second NaN check: mantissa is 0
    cmpl $0, %r9d           # Compare mantissa value to 0
    jz nanCheckCleared      # If mantissa is 0, it passes the check. Jump.

    # If fallen through both checks, value is NaN. Return original argument in eax.
    ret

nanCheckCleared:
    andl $0x7fffffff, %eax # After confirming arg. is valid, set eax to positive by setting sign bit to 0 and return
    ret


# floatIsLess - Compute f < g for floating point arguments f and g.
#   Both the arguments are passed as unsigned int's, but
#   they are to be interpreted as the bit-level representations of
#   single-precision floating point values.
#   If either argument is NaN, return 0.
#   +0 and -0 are considered equal.
#   Rating: 3
.global floatIsLess
floatIsLess:
# NaN check for argument 1
    movl %edi, %eax     # Move arg. 1 value to return reg.
    # Also store a copy of arg. 1 to r8d, r9d
    movl %edi, %r8d     # Will store exponent bits
    movl %edi, %r9d     # Will store mantissa bits


    # Manipulate float bits to get their respective bit representations
    shr $23, %r8d               # Shift bits in r8d right by 23
    andl $0xff, %r8d            # Mask r8d with 0xFF to get only the exponent bits

    andl $0x7fffff, %r9d        # Mask r9d with 0x7FFFFF to get the mantissa bits


    # First NaN check: exponent not at max value
    cmpl $0xff, %r8d
    jne fNotNan

    # Second NaN check: mantissa is 0
    cmpl $0, %r9d
    je fNotNan

	# Falling through both jumps means f is NaN. Return 0.
	jmp returnZero

fNotNan:
# NaN check for argument 2
    movl %esi, %eax     # Move arg. 2 value to return reg.

    # Also store a copy of arg. 2 to r8d, r9d
    movl %esi, %r8d     # Will store exponent bits
    movl %esi, %r9d     # Will store mantissa bits

    # Manipulate float bits to get their respective bit representations
    shr $23, %r8d               # Shift bits in r8d right by 23
    andl $0xFF, %r8d            # Mask r8d with 0xFF to get the exponent bits

    andl $0x7fffff, %r9d        # Mask r9d with 0x7FFFFF to get the mantissa bits

    # First NaN check: exponent not at max value
    cmpl $0xff, %r8d
    jne gNotNan

    # Second NaN check: mantissa is 0
    cmpl $0, %r9d
    je gNotNan

	# g fails both checks, thus is NaN. Return 0.
	jmp returnZero

gNotNan:
# +/- 0 check
	# Store original arguments in registers
	movl %edi, %r8d		# r8d stores f
	shl $1, %r8d		# Shift bits in f 1 to the left to remove sign bit
	cmpl $0, %r8d
	jne compareSigns		# Check if f is 0. If nonzero, skip 0-checking g.

	movl %esi, %r8d		# r8d now stores g
	shl $1, %r8d		# Shift to remove sign bit
	cmpl $0, %r8d
	jne compareSigns		# If g is nonzero, skip the following procedure and move on to comparing signs.

	jmp returnZero		# If unsigned values of f and g are zero, return 0 (since they're both zero already)


compareSigns:
# Store and compare the sign bits for each arg.
    movl %edi, %r8d     # r8d holds arg. 1 sign bit
    shr $31, %r8d

    movl %esi, %r9d     # r9d holds arg. 2 sign bit
    shr $31, %r9d

    cmpl %r9d, %r8d		# Compare f sign bit to g
    je compareExponents	# If signs are equal, move on to comparing exponents instead
	jg fIsLess			# If f has negative bit and g is does not, f < g. Jump to the respective case.
	jl returnZero		# If f is positive and g is negative, then f < g is FALSE. Return 0.

# NOTE: from here, r8d is reserved to store a sign bit since f's and g's are equal. This is used to do the appropriate comparisons.
compareExponents:
	# Get f's exponent bits
	movl %edi, %r9d		# Move f's value in r9d. Will store f's exponent bits.
	shr $23, %r9d		# Shift bits over to remove mantissa bits
	andl $0xff, %r9d	# Mask to extract only the exponent bits

	# Get g's exponent bits
	movl %esi, %r10d	# Will store g's exponent bits.
	shr $23, %r10d
	andl $0xff, %r10d

	cmpl %r10d, %r9d
	je compareMantissas	# If exponents are equal, move on to comparing the mantissas

	cmpl $0, %r8d	# Check sign
	jz posCmpExp	# If signs are positive, jump to comparing exponents with positives

	cmpl %r10d, %r9d
	jg fIsLess		# If f has a greater exponent as a negative number, then f < g.
	jl returnZero	# If f has a smaller exponent as a negative number, f < g is FALSE. Return 0.

	posCmpExp:
	cmpl %r10d, %r9d
	jl fIsLess		# If f has a smaller exponent as a positive number, then f < g.
	jg returnZero	# Else, f < g is FALSE. Return 0.

compareMantissas:
	# Get f's mantissa bits
	movl %edi, %r9d
	andl $0x7fffff, %r9d

	# Get g's mantissa bits
	movl %esi, %r10d
	andl $0x7fffff, %r10d

	cmpl %r10d, %r9d
	je returnZero	# If mantissas are equal, then both values must be equal at this point and f < g is false. Return 0.

	cmpl $0, %r8d
	jz posCmpMant	# If signs are positive, jump to comparing mantissas as positive values

	cmpl %r10d, %r9d
	jg fIsLess		# If f's mantissa is less than g's as negatives, then f < g.
	jl returnZero	# Else, f < g is false. Return 0.

	posCmpMant:
	cmpl %r10d, %r9d
	jl fIsLess		# If positive f's mantissa is less, then f < g.
	jg returnZero	# Else, f < g is false. Return 0.


# Conditional labels -- sectioned off for organization purposes
fIsLess:
    movl $1, %eax	# If f is proven to be less than g, move 1 (true) to eax and return
    ret

returnZero:
	# General function that returns 0. Reason for returning 0 should be specified at jump line.
	movl $0, %eax
	ret



# isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
#   Example: isAsciiDigit(0x35) = 1.
#            isAsciiDigit(0x3a) = 0.
#            isAsciiDigit(0x05) = 0.
#   Rating: 3
.global isAsciiDigit
isAsciiDigit:
    movl %edi, %eax		# Move arg. 1 to return reg.
	cmpl $0x30, %eax	# Compare eax with the lower ASCII value bound
	jge withinLower		# If arg. within lower range, move on to comparing w/ upper bound

	movl $0, %eax		# If arg. is outside of lower bound, it is not an ASCII digit. Return 0.
    ret

withinLower:
	cmpl $0x39, %eax	# Check if arg. is within upper bound.
	jle withinUpper

	movl $0, %eax		# If arg. is outside of upper bound, return 0.
	ret

withinUpper:
	movl $1, %eax		# If arg. is confirmed to be within ASCII digit representation range, return 1.
	ret


# addOK - Determine if we can compute x+y without overflow
#   Example: addOK(0x80000000,0x80000000) = 0,
#            addOK(0x80000000,0x70000000) = 1,
#   Rating: 3
.global addOK
addOK:
	# Get the signs of the two args
	movl %edi, %r8d
	shr $31, %r8d	# x's sign bit

	movl %esi, %r9d
	shr $31, %r9d	# y's sign bit

	# Get the sum of the args and its sign
	movl %edi, %r10d
	addl %esi, %r10d	# sum value
	shr $31, %r10d		# is shifted right to store the sum's sign bit

	# Compare the signs of x and y. If they differ, adding is ok. Else compare signs to sum's sign.
	cmpl %r8d, %r9d
	je sameSigns

	movl $1, %eax
	ret

sameSigns:
	# Compare one of the signs to the sum's
	cmpl %r8d, %r10d
	jne sumSignDiffers

	# If they all share the same sign, adding is ok
	movl $1, %eax
	ret

sumSignDiffers:
	# If sum's sign differs with one of the args' signs, addOK is false
	movl $0, %eax
	ret


# isGreater - if x > y  then return 1, else return 0
#   Example: isGreater(4,5) = 0, isGreater(5,4) = 1
#   Rating: 3
.global isGreater
isGreater:
    movl %edi, %eax		# x
	movl %esi, %r8d		# y
	# Do inequality check between x and y
	cmpl %r8d, %eax
	jg xIsGreater
	# If x is not greater than y, return 0
	movl $0, %eax
	ret

xIsGreater:
	# If x is greater than y, return 1
	movl $1, %eax
    ret

# floatFloat2Int - Return bit-level equivalent of expression (int) f
#   for floating point argument f.
#   Argument is passed as unsigned int, but
#   it is to be interpreted as the bit-level representation of a
#   single-precision floating point value.
#   Anything out of range (including NaN and infinity) should return
#   0x80000000u.
#   Rating: 4
.global floatFloat2Int
floatFloat2Int:
    # Move value of float to registers, set them to represent its IEEE components
	movl %edi, %r8d		# sign bit
	shr $31, %r8d

	movl %edi, %r9d		# exponent bits
	shr $23, %r9d
	andl $0xFF, %r9d

	movl %r9d, %r11d	# unbiased exponent
	subl $127, %r11d

	movl %edi, %r10d	# mantissa bits
	andl $0x7FFFFF, %r10d
	orl $0x8000000, %r10d

	# Check for NaN
	cmpl $31, %r11d
	jge isNaN			# unbiased exponent exceeds range

	cmpl $0xFF, %r9d
	jne nonMaxedExp		# exponent is not maxed, no need to check mantissa

	testl %r10d, %r10d
	jne isNaN			# mantissa is nonzero w/ maxed exponent. is NaN.

nonMaxedExp:
	# Additional checks for unbiased exponent
	cmpl $0, %r11d
	jl negativeUnbiased		# jump to handle case where unbiased exponent < 0
	je zeroUnbiased			# jump to handle case where unbiased exponent = 0

	movl %r10d, %eax		# move mantissa bits to eax for return

	cmpl $1, %r8d
	je negateResult			# if sign is negative, return negative value

	ret

zeroUnbiased:
	movl $1, %eax
	testl %r8d, %r8d
	jnz negateResult		# if sign is negative, return -1
	ret

negateResult:
	negl %eax
	ret

negativeUnbiased:
	movl $0, %eax
	ret

isNaN:
	movl $0x80000000u, %eax
	ret


# absVal - absolute value of x
#   Example: absVal(-1) = 1.
#   You may assume INT_MIN <= x <= INT_MAX
#   Rating: 4
.global absVal
absVal:
    movl %edi, %eax
	cmpl $0, %eax
	jl doNegation
    ret
doNegation:
	negl %eax
	ret
