#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#define    COLOR_NONE                 "\033[0m"
#define    FONT_COLOR_RED             "\033[0;31m"
#define    FONT_COLOR_BLUE            "\033[1;34m"
#define    BACKGROUND_COLOR_RED       "\033[41m"    
#define    BG_RED_FONT_YELLOW         "\033[41;33m"

void processGroup()
{
	pid_t pid;
	printf(FONT_COLOR_RED">>> processGroup\n"COLOR_NONE);
	// 获取进程组ID
	pid = getpgrp();
	printf("pid = getpgroup() = %d\n", pid);
	pid = getpgid(0);
	printf("pid = getpgid(0)  = %d\n", pid); 
}

void session()
{
	pid_t pid;
	printf(FONT_COLOR_RED">>> session\n"COLOR_NONE);
	if((pid = fork()) < 0 ) {
		printf("fork error\n");
	} else if (pid == 0) {
		if ((pid = fork()) < 0) {
			printf("fork error\n");
		} else if (pid == 0) {
			printf("set new sid\n");
			printf("pid:          %d\n", getpid() );
			printf("original sid: %d\n", getsid(0));
			setsid();
			printf("new sid:      %d\n", getsid(0));
			exit(0);
		} else {
			printf("quit parent.\n");
			exit(0);
		}
	}	else {
		waitpid(pid, NULL, 0);
		sleep(1);
	}
}

void terminal()
{
	printf(FONT_COLOR_RED"controlTerminal\n"COLOR_NONE);
}
int main(int argc, char *argv[])
{
	processGroup();   // 进程组
	session();        // 会话
	terminal();       // 控制终端
	return 0;
}





