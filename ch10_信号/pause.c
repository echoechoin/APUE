#include <stdio.h>
#include <unistd.h>

int main ()
{
    printf("pause, waitting for a signal...\n");
    pause();
    return 0;
}