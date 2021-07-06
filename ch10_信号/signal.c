#include <stdio.h>
#include <signal.h>
#include <errno.h>
// 无法忽略的信号:
// SIGKILL: kill -9 pid 
// SIGSTOP：ctrl+z 或者 kill -STOP pid
//
// 交互式停止可以忽略：SIGTSTP：只能是CTRL + Z
// ctrl+d不是信号 是 end of file
void ctrl_z_handle(int signo)
{
	if (signo == SIGSTOP) {
		printf("catch SIGSTOP\n");		
	}
	if (signo == SIGTSTP) {
		printf("catch SIGTSTP\n");
	}
}

int main ()
{
	if( signal(SIGTSTP, ctrl_z_handle) == SIG_ERR ) {
		printf("signal error\n");
		perror("");
	} else if (signal(SIGSTOP, ctrl_z_handle) == SIG_ERR) {
		printf("signal error\n");
		perror("SIGSTOP无法捕获:");
	}
	printf("press ctrl+z to send SIGSTOP\n");
	for (;;);
	return 0;
}
