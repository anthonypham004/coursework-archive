// Based on Bryant and O'Halloran's original "datalab" assignment

#if 0

STEP 1: Read the following instructions carefully.

You will provide your solution to this part of the project by
editing the collection of functions in this source file.

INTEGER CODING RULES:

  Replace the "return" statement in each function with one
  or more lines of C code that implements the function.

  Each expression in your code can use ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xFFFFFFFF.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>

  Some of the problems restrict the set of allowed operators even further.
  You are not restricted to one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operators, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int. This implies that you
     cannot use arrays, structs, or unions.

  You may assume that your machine:
  1. Uses twos complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting if the shift amount
     is less than 0 or greater than 31.

FLOATING POINT CODING RULES

  For the problems that require you to implement floating-point operations,
  the coding rules are less strict.  You are allowed to use looping and
  conditional control.  You are allowed to use both int and unsigned variables.
  You can use arbitrary integer and unsigned constants. You can use any arithmetic,
  logical, or comparison operations on int or unsigned data.

  You are expressly forbidden to:
    1. Define or use any macros.
    2. Define any additional functions in this file.
    3. Call any functions.
    4. Use any form of casting.
    5. Use any data type other than int or unsigned.  This means that you
       cannot use arrays, structs, or unions.
    6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the ndlc program (described in the spec) to check the legality of
     your solutions.
  2. Each function has a maximum number of operations (integer, logical,
     or comparison) that you are allowed to use for your implementation
     of the function. The max operator count is checked by ndlc.
     Note that assignment ('=') is not counted; you may use as many of
     these as you want without penalty.
  3. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies
     between the maximum ops in the spec and in this file, consider
     this file the authoritative source.

STEP 2: Modify the following functions according the coding rules.

IMPORTANT. TO AVOID GRADING SURPRISES:
  1. Use the ndlc tool to check that your solutions conform to the coding rules.

YOU WILL RECEIVE NO CREDIT IF YOUR CODE DOES NOT PASS THIS CHECK.

  2. Run the provided tests to check that your solutions achieve the
     desired results.

#endif

/*
 * isZero - returns 1 if x == 0, and 0 otherwise
 *   Examples: isZero(5) = 0, isZero(0) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 2
 *   Rating: 1
 */
int isZero(int x) {
    return !x;    // Non-zero = true, so NOT non-zero = false
}

/*
 * bitNor - Compute ~(x|y) using only ~ and &
 *   Example: bitNor(0x6, 0x5) = 0xFFFFFFF8
 *   Legal ops: ~ &
 *   Max ops: 8
 *   Rating: 1
 */
int bitNor(int x, int y) {
    return (~x & ~y);
    // Definition from discrete math:
    // NOR = NOT x or y == ~p v ~q
    // ~p v ~q == ~p ^ ~q
}

/*
 * leastBitPos - return a mask that marks the position of the
 *               least significant 1 bit. If x == 0, return 0
 *   Example: leastBitPos(96) = 0x20
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */
int leastBitPos(int x) {
    return (x & (~x + 1));
}
/*
Say x = 12
Binary: 0000 1100
              ^ LSB
Negative:
-12 = -256 + 244 = 1111 0100
                         ^ pos of LSB

  1111 0100     12
& 0000 1100    -12

  0000 0100
        ^ pos of LSB!

How to get -x?

Invert: 0000 1011
Add 1:  0000 1100

So -x = ~x + 1?

*/

/*
 * copyLSB - set all bits of result to least significant bit of x
 *   Example: copyLSB(5) = 0xFFFFFFFF, copyLSB(6) = 0x00000000
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int copyLSB(int x) {
    return ~(1 & x) + 1;
}
/*
Steps:
1. Get val of rightmost bit
2.	If 0, return 0000 0000
    If 1, return 1111 1111

How to get val of rightmost bit?

1 & x?
Say x = 12:

  0000 1100		12
& 0000 0001		1

  0000 0000     <- this can possibly be returned (nah.)

Say y = 13:

  0000 1101		13
& 0000 0001		1

  0000 0001

How to return 1111 1111 if 0000 0001?
0000 0000
0000 0001
1111 1111

~1 = 1111 1110
~1 + 1 = 1111 1111

~0 = 1111 1111
~0 + 1 = 1 0000 0000
         ^ overflow bit, gets discarded I think
Return ~(1 & x) + 1?
*/

/*
 * distinctNegation - returns 1 if x != -x.
 *     and 0 otherwise
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 5
 *   Rating: 2
 */
int distinctNegation(int x) {
    return !!(x ^ (~x + 1));
}
/*
x = -x if:
x = 0000 0000 0000 0000 0000 0000 0000 0000 = 0 = -0
or
x = 1000 0000 0000 0000 0000 0000 0000 0000 = -2147483648 = 2147483648 (w/o two's complement)

say x = 12:
12 != -12
0000 1100 != 1111 0100
~x = 1111 0011
~x + 1 = 1111 0100 = -x

how to get -x without using a negative sign?
(~x + 1) = y

say x = 0 => 0
     x = 0000 0000 0000 0000 0000 0000 0000 0000
    ~x = 1111 1111 1111 1111 1111 1111 1111 1111
    -x = 0000 0000 0000 0000 0000 0000 0000 0000

    x | y =
    0000 0000 0000 0000 0000 0000 0000 0000

    x ^ y =
    0000 0000 0000 0000 0000 0000 0000 0000

    x & y =
    0000 0000 0000 0000 0000 0000 0000 0000
say x = -2147483648 => 0
     x = 1000 0000 0000 0000 0000 0000 0000 0000
    ~x = 0111 1111 1111 1111 1111 1111 1111 1111
    -x = 1000 0000 0000 0000 0000 0000 0000 0000 (w/o T.C.)

    x | y =
    1000 0000 0000 0000 0000 0000 0000 0000

    x ^ y =
    0000 0000 0000 0000 0000 0000 0000 0000
say x = 2147483647 => 1
     x = 0111 1111 1111 1111 1111 1111 1111 1111
    ~x = 1000 0000 0000 0000 0000 0000 0000 0000
    -x = 1000 0000 0000 0000 0000 0000 0000 0001

    x ^ y = 1111 1111 1111 1111 1111 1111 1111 1110
say x = 12 => 1
     x = 0000 1100 (8-bit)
    ~x = 1111 0011
    -x = 1111 0100

    x ^ y =
    0000 0100

For the case where x = -x, x ^ y = 0. A 1 bit is present somewhere otherwise.
From isZero, return !(x ^ y) so it reduces value to 1 or 0 (true or false).
Returns opposite of desired result, so add an extra ! (looks kinda dumb but it works)
*/

/*
IEEE-754 breakdown (for me to use as reference):

sign    exponent		mantissa
1 		0101 0101 		010 1010 1010 1010 1010 1010
1 bit   8 bits          23 bits

bias for single precision = 127 (add to exponent)

int 123456 = 11110001001000000 = 1.1110001001000000 x 2^16

sign = 0
exponent = 16 + 127 = 143 = 10001111
mantissa = 1110001001000000

IEEE representation of 123456 = 0 10001111 1110001001000000
*/

/*
 * floatAbsVal - Return bit-level equivalent of absolute value of f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   When argument is NaN, return argument.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 10
 *   Rating: 2
 */
unsigned floatAbsVal(unsigned uf) {
    // Set sign bit to 0 by performing & with 0111 1111 ... 1111
    unsigned posNum = uf & 0x7FFFFFFF;
    // Get exponent bits by shifting out mantissa bits and masking with 1111 1111
    unsigned exponent = (uf >> 23) & 0xFF;
    // Get mantissa bits using a similar process
    unsigned mantissa = uf & 0x7FFFFF;

    // NaN when exponent = 1111 1111 and mantissa != 0
    if (exponent == 0xFF && mantissa != 0) {
        return uf;
    } else {
        return posNum;
    }
}
/*
2147483648 = 1000 0000 0000 0000 0000 0000 0000 0000 = -2147483648
binary-float representation: 01001111000000000000000000000000

Need to set sign bit to 0
uf & 0111 1111 1111 1111...

  0100 1111 0000 0000...
& 0111 1111 1111 1111...

  0100 1111 0000 0000...


  To check for NaN:
  - exponent bits need to be all 1's
  - mantissa != 0
  I had to look this up.
*/

/*
 * floatIsLess - Compute f < g for floating point arguments f and g.
 *   Both the arguments are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   If either argument is NaN, return 0.
 *   +0 and -0 are considered equal.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 3
 */
int floatIsLess(unsigned uf, unsigned ug) {
    // Similar float breakdown as in previous function
    unsigned sign = uf >> 31;
    unsigned exponent = (uf >> 23) & 0xFF;
    unsigned mantissa = uf & 0x7FFFFF;
    // Do the same for ug
    unsigned sign2 = ug >> 31;
    unsigned exponent2 = (ug >> 23) & 0xFF;
    unsigned mantissa2 = ug & 0x7FFFFF;

    // NaN check for uf and ug
    if ((exponent == 0xFF && mantissa != 0) || (exponent2 == 0xFF && mantissa2 != 0)) {
        return 0;
    }

    // +/- 0 check by shifting out the sign bits and comparing them to 0
    if ((uf << 1) == 0 && (ug << 1) == 0) {
        return 0;
    }

    // Compare uf's float attributes to ug's by
    // 1. first checking if they're equal (if so, move on to the next attribute)
    // 2. then checking if one is greater than the other
    // This is done in order from most important indicator of each size to least (hope the way I
    // worded that makes sense)
    if (sign != sign2) {
        return sign > sign2;    // Will return 1 if sign is greater than sign2 (saying uf has
                                // negative bit, ug doesn't)
    }
    if (exponent != exponent2) {
        // Two cases: both positive or both negative
        if (sign == 0) {
            return exponent < exponent2;
        } else {
            return exponent2 < exponent;
        }
    }
    if (mantissa != mantissa2) {
        if (sign == 0) {
            return mantissa < mantissa2;
        } else {
            return mantissa2 < mantissa;
        }
    }
    return 0;
}
/*
Methods for checking inequality (in this order):
1. sign (uf = 1, ug = 0)
2. compare exponents
3. compare mantissas
*/

/*
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x) {
    int upperVal = 0x39 + (~x + 1);
    int lowerVal = x + (~0x30 + 1);

    return (!(upperVal >> 31) & !(lowerVal >> 31));
}
/*
Goals:
Check if
-	x - 0x30 >= 0
-	0x39 - x >= 0
Use (~x + 1) and vice versa for negative for subtraction

How to check if greater than 0?
Ideas:
- Move sign bit over and mask
- < 0 if sign bit is 1 (negative sign)

*/

/*
 * addOK - Determine if we can compute x+y without overflow
 *   Example: addOK(0x80000000,0x80000000) = 0,
 *            addOK(0x80000000,0x70000000) = 1,
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 */
int addOK(int x, int y) {
    // Extract signs of x and y
    int xSign = x >> 31;
    int ySign = y >> 31;
    int sumSign = (x + y) >> 31;

    // Get 1/0 value representing same sign by doing XOR on the sign bits (then flipping it to get
    // the desired represntation of the answer)
    int same = !(xSign ^ ySign);
    int differs =
        xSign ^ sumSign;    // (Arbitrarily) chose x's sign to compare with same's. XOR returns 1 if
                            // only one bit or the other is 1, telling us they're different.

    // If both are true (x and y have same sign AND the result has a different sign), then an
    // overflow must have occured
    return !(same & differs);
}
/*
Overflow can be indicated if the signs of x or y don't match with the sum's sign
Steps:
1. Check to see if x and y have the same signs
2. If they do, check to see if the sign of (x + y) is the same as the sign of x and (or?) the sign
of y
!(((x + y) >> 31) ^ (x >> 31)) mmm no. not all at once at least.
*/

/*
 * isGreater - if x > y  then return 1, else return 0
 *   Example: isGreater(4,5) = 0, isGreater(5,4) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isGreater(int x, int y) {
    // Extract signs from x and y
    int xSign = x >> 31;
    int ySign = y >> 31;

    // Compare signs (1 if different)
    int differentSigns = xSign ^ ySign;

    // Bool representing if x is positive and y is negative (1 if true, 0 otherwise)
    int xGreaterSign = ~xSign & ySign;

    // Get x - y and its sign
    int diff = x + (~y + 1);
    int diffSign = diff >> 31;

    // If the sign is 0 (positive difference) or the two had the same sign (differentSigns = 0), x
    // may be greater by value (represented as 1)
    int xGreaterValue = !(diffSign | differentSigns);

    // Putting it all together: x is confirmed to be greater if x has a greater sign or is greater
    // by value.
    // The !!diff accounts for the case where x = y:
    // Nonzero diff = x != y. !diff reduces diff to 1 or 0 and the extra ! switches it back to the
    // right bool value.
    return (xGreaterSign | xGreaterValue) & !!diff;
}
/*
Straightforward I think?
First compare signs
Then compare the remaining bits--ok I see the challenge now...

Need to see if signs are same, a potential indicator from the beginning

To check for signs of x and y:
signX ^ signY: 1 if different, 0 if same

How to indicate the sign of one and the other?
Say signX = 0, signY = 1
signs = ~x & y
         1   1
If signs is 1, then both match, and since x's sign was flipped and they both match, then that means
x's sign was originally 0 = positive

0 0
1 0 = 0

1 1
0 1 = 0

THEN make an int for the difference of x and y regardless of sign
x + (~y + 1) = diff
Then need to check sign
(diff >> 31) = diffSign

If it's negative AND x and y originally had the same signs, then x is less than by value
- Check if diff sign is 1 and signs is 1
!(diffSign & signs)

To check if equal:
- Same signs (differentSigns = 0)
- x - y = 0
    (x & (~y + 1) ^ 1)
OR since diff may be nonzero, check if that's the case (since that means x - y != 0)

*/

/*
 * floatFloat2Int - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int floatFloat2Int(unsigned uf) {
    unsigned sign = uf >> 31;
    unsigned exponent = (uf >> 23) & 0xFF;
    unsigned mantissa = (uf & 0x7FFFFF) | 0x8000000;    // Last part adds that implied leading 1 bit

    // Removes the added 127 from bias
    int unbiased = exponent - 127;

    // NaN and exponent checks
    if ((exponent == 0xFF && mantissa != 0) || unbiased >= 31) {
        return 0x80000000u;
    }
    if (unbiased < 0) {
        return 0;
    }
    // Account for 2^1 where it'd equal 1
    if (unbiased == 0) {
        if (sign == 0) {
            return 1;
        } else {
            return -1;
        }
    }

    // Shift mantissa bits to their respective spot
    int result;

    if (unbiased >= 23) {
        result = mantissa << (unbiased - 23);
    } else {
        result = mantissa >> (23 - unbiased);
    }

    if (sign == 1) {
        result = -(result);
    }

    return result;
}
/*
Bounds check:
- NaN: exponent = 0xFF and mantissa != 0
- Exponent too big or small for int ( > 31 or < 0 )

Accounting for bias:
upper bound = 31 > exponent - 127 > 0

(for me to reference from)
int 123456 = 11110001001000000 = 1.1110001001000000 x 2^16

sign = 0
exponent = 16 + 127 = 143 = 10001111
mantissa = 1110001001000000

IEEE representation of 123456 = 0 10001111 1110001001000000

So to extract number:
Take bias away from exponent
Check if unbiased exponent is within int range

Using the exponent:
If it's greater than 23 (the 23 mantissa bits), then that means there's more bits to add
Shift the mantisa bits left by unbiased - 23(?)

Else if there's less than 23, shift them right I think


*/

/*
 * absVal - absolute value of x
 *   Example: absVal(-1) = 1.
 *   You may assume INT_MIN <= x <= INT_MAX
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 10
 *   Rating: 4
 */
int absVal(int x) {
    int mask = x >> 31;
    return (x + mask) ^ mask;
}
/*
Need to extract the sign bit and return x without it
x >> 31
if it's 0, then x ^ 0x00000000 is just x
if 1, need to get rid of it from sequence...

value of sign bit is the number that fills in from the left

add sign bit mask to x

let x = -12 = 1111 1111 1111 1111 1111 1111 1111 0100

+12 = 0000 0000 0000 0000 0000 0000 0000 1100


mask from -12 = 1111 1111 1111...

x + mask =

   1111 1111 1111 1111 1111 1111 1111 0100
   1111 1111 1111 1111 1111 1111 1111 1111

01 1111 1111 1111 1111 1111 1111 1111 0011

xor w/ mask

   1111 1111 1111 1111 1111 1111 1111 0011
   1111 1111 1111 1111 1111 1111 1111 1111
   0000 0000 0000 0000 0000 0000 0000 1100 = 12

*/
