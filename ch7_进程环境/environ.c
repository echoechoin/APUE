#include <stdio.h>

int main() {
    extern char **environ;
    for (int i = 0; environ[i] != NULL; i++){
        printf("environ[%d]: %s\n", i, environ[i]);
    }
}