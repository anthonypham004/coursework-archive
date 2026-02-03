// Construct memory diagrams for the blocks below
#include <stdio.h>

int main() {
    // BLOCK A
    int a = 5;
    int b = 7;
    double x = 4.5;
    int *ip = &a;
    ip = &b;
    int c = *ip;
    *ip = 19;
    // DRAW MEMORY HERE
    printf("%p\n", &a); // 0x16effaf04 or 6157217540
    printf("%p\n", &b); // 0x16effaf00 or 6157217536
    printf("%p\n", &x); // 0x16effaef8 or 6157217528
        // These two in separate area in memory?
    printf("%p\n", &ip);// 0x16d486ef0 or 6128430832
    printf("%p\n", &c); // 0x16d486eec or 6128430828


    // BLOCK B
    int arr[4] = {12, 14, 16, 18};
    int *arp = arr;
    int brr = 11;
    arr[1] = 23;
    arp[3] = 29;
    arp = &arr[2];
    *arp = brr;
    // DRAW MEMORY HERE
    printf("Block B:\n");
    printf("%p, %c\n", &arr, *arr);

    // BLOCK C
    char *cp = NULL;
    char str[8] = "hello";
    str[5] = 'w';
    cp = str + 6;
    *cp = '\0';
    str[0] = 'y';
    // DRAW MEMORY HERE
    printf("%s", cp);
}
