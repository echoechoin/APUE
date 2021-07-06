#include <stdio.h>

int main () {
    printf("linux 存储为小端字节序\n");
    int i = 0x04030201;
    printf("i = %p\n", i);
    char *c = (char *)&i;
    for (int i = 0; i <= 3; i++){
        printf("%d: %p  ", i, *c);
        c++;
    }
    printf("\n");
    return 0;
}