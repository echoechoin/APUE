#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void atexit_handler1(void) {
    printf ("at exit[1]..\n");
}

void atexit_handler2(void) {
    printf ("at exit[2]..\n");
}

void atexit_handler3(void) {
    printf ("at exit[4]..\n");
}

void atexit_handler4(void) {
    printf ("at exit[4]..\n");
}

int main() {
    atexit(atexit_handler1); 多次注册就会多次调用
    atexit(atexit_handler1);
    atexit(atexit_handler2);
    atexit(atexit_handler3);
    atexit(atexit_handler4);
    printf("exit..\n");
    //_exit(0);
    return 0;
}