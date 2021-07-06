#include<stdio.h>
#include<signal.h>
#include<stdlib.h>
#include<unistd.h>

void handler(int signo) {
	printf("SIGINT \n");
}

int main(int argc, char *argv[])
{
	signal(SIGINT, handler);
	while(1);
	return 0;
}
