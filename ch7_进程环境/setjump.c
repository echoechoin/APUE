#include <stdio.h>
#include <setjmp.h>
jmp_buf jump_buffer;

int main () {
    int i = 0;
    int j =1;
    volatile int k = 1;
    i = setjmp(jump_buffer); 
    printf("setjmp return %d\n", i);
    printf("j %d\n", j);
    printf("k %d\n", k);
    j = 2; // 这里的j = 2会被优化掉
    k = 2; // 这里的k = 2不会被优化掉
    if (i != 0) {
        return 0;
    }
    longjmp(jump_buffer,123); 
    return 0;
}

// 优化编译： gcc setjmp.c -o setjmp -O3