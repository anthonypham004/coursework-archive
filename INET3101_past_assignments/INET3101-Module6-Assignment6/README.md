# INET3101-Module6-Assignment6
### PROBLEM
- ls needed to be modified so that it outputs the long format outputted by the -l flag.
- whoami needed to be modified so that it prints "You are: " before the user id.

### SOLUTION
ls has an enum with different values associated with the types of output. 
"many_per_line" is the value for the default value, and "long_format" is the value for the desired -l output. So the solution here was to replace "many_per_line" with "long_format" in the right spots.
However, because of how long the ls source code is, it was difficult to know exactly where it'd be necessary to switch the values. As a result, there may be some value changes that weren't needed. Whatever.

whoami was much more straightforward. I just needed to replace the puts() call where the user id would be printed with printf() and add the "You are: " string before the user id.

### PROS AND CONS
+ ls prints the long format output by default
+ whoami outputs "You are: " before the user id
- There may be some unnecessary changes in ls.c that could disrupt the code's proper function