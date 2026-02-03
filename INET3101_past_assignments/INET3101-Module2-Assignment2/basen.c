/* basen.c -- prints arg 1 to the number base from arg 2 */
#include <stdio.h>

void to_base_n(unsigned long n, unsigned int base);

int main(void)
{
	unsigned long number;
    unsigned int base;
	printf("Enter an integer (q to quit):\n");
	while (scanf("%lu", &number) == 1)
	{
		printf("Enter a base between 2 and 16:\n");
        scanf("%iu", &base);

        if (base < 2 || base > 16) {
            printf("error: given base is outside of range\n");
            return 1;
        }

        // Prefixes
        if (base == 8) {
            printf("0");
        } else if (base == 16) {
            printf("0x");
        }
        to_base_n(number, base);

		putchar('\n');
		printf("Enter an integer (q to quit):\n");
	}
	printf("Done.\n");
	
	return 0;
}

void to_base_n(unsigned long n, unsigned int base)  /* recursive function */
{
    int q;
    int r;
    q = n / base;
    r = n % base;

    // Recursive case
    if (n >= base) {
        to_base_n(q, base);
    }

    if (r >= 10) { // HEX number to letter conversion
        char chr;
        chr = r + 55; // formula gets the ASCII value of A-F that corresponds to 10-15
        putchar(chr);
    } else {
        printf("%d", r);
    }
	return;
}