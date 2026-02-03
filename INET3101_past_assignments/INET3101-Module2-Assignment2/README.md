# INET3101-Module2-Assignment2
PROBLEM:
The provided to_binary() function from the assignment needed to be remade into a generalized to_base_n() function.

SOLUTION: 
The basen.c program takes in two number inputs:
1. number = the number to be converted into base n form
2. base = the base the number will be converted to

The to_binary() function is rewritten to be a generalized function that can handle bases 2-16.
'number' will then be converted depending on the 'base' input using the recursive to_base_n() function.

The common bases will have their respective prefixes:
- Any number converted to base 8 will output "0" followed by the value
- Any number converted to base 16 will output "0x" followed by the value

PROS AND CONS:
+ Generalized the function to be able to convert a number to bases 2-16
+ Bases 8 and 16 are prefixed
+ Digit-to-char conversion for numbers 10-15
- Base 2 not prefixed with '0b'
- Program does not handle bases outside of 2-16 range




