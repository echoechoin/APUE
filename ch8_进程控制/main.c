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
void processIdentifier()
{	
	printf(FONT_COLOR_RED">>> processIdentifier\n"COLOR_NONE);
	pid_t pid  = getpid();
	pid_t ppid = getppid();
	uid_t uid  = getuid();
	uid_t euid = geteuid();
	gid_t gid  = getgid();
	gid_t egid = getegid();
	printf("pid:  %d\n", pid);
	printf("ppid: %d\n", ppid);
	printf("uid:  %d\n", uid);
	printf("euid: %d\n", euid);
	printf("gid:  %d\n", gid);
	printf("egid: %d\n", egid);
	return;
}

void functionFork()
{
	pid_t pid, ppid;
	int flag = 0;
	printf(FONT_COLOR_RED">>> functionFork\n"COLOR_NONE);
	pid = fork();
	if (pid < 0) {
		printf("fork error");
		return;
	} else if (pid == 0) {
		pid  = getpid();
		ppid = getppid();
		flag = 1;
	} else if (pid > 0) {
		pid  = getpid();
		ppid = getppid();
		flag = 2;
	}
	printf("pid: %d, ppid: %d, flag: %d\n", pid, ppid, flag);
	wait(NULL);
	if (flag == 2) {
		exit(0);
	}
	return;
}

void fileSharing()
{
	printf(FONT_COLOR_RED">>> fileSharing\n"COLOR_NONE);
	return;
}

void functionVfork()
{
	pid_t pid, ppid, cpid = 0;
	char flag = '?';
	printf(FONT_COLOR_RED">>> functionVfork\n"COLOR_NONE);
	cpid = vfork();
	if (pid < 0) {
		printf("fork error");
		return;
	} else if (cpid > 0) {
		printf("Parent afer child exit\n");
		pid  = getpid();
		ppid = getppid();
		flag = 'P';
	} else if (cpid == 0) {
		printf("child first\n");
		pid  = getpid();
		ppid = getppid();
		flag = 'C';
	}
	printf("cpid: %d, pid: %d, ppid: %d, flag: %c\n", cpid, pid, ppid, flag);
	if (flag == 'C') {
		exit(0);
	}
	return;
}


void functionWait()
{
	/*
	 * wait(&status) 阻塞等待子进程状态
	 * waitpid可以不用阻塞，但是还是要轮询
	 * > 两次fork解决僵死进程
	 *
	 * */
	pid_t pid;
	int status;
	int zero = 0;

	// normal exit
	printf(FONT_COLOR_RED">>> functionWait\n"COLOR_NONE);
	if ((pid = fork()) < 0)
		printf("fork error\n");
	else if (pid == 0) // child process
		exit(7);
	if (!wait(&status)) {
		printf("wait error\n");
	}
	else
		printf("normal exit. WIFECITED(status): %d\n", WIFEXITED(status));
	
	// abnormal termination 1
	if ((pid = fork()) < 0)
		printf("fork error\n");
	else if (pid == 0) // child process
		abort();
	if (!wait(&status))
		printf("wait error\n");
	else {
		printf("abnormal exit. WIFSIGNALED(status): %d, ", WIFSIGNALED(status));
		printf("signal number:%d(SIGABORT)\n", WTERMSIG(status));
	}

	// abnormal termination 2
	if ((pid = fork()) < 0)
    printf("fork error\n");
  else if (pid == 0) // child process
    status /= zero;
  if (!wait(&status))
    printf("wait error\n");
  else {
    printf("abnormal exit. WIFSIGNALED(status): %d, ", WIFSIGNALED(status));
    printf("signal number: %d(SIGFPE)\n", WTERMSIG(status));
  }	
	return;
}

void raceCondition()
{
	pid_t pid;
	char *out_from_child  = "out from child\n";
	char *out_from_parent = "out from parent\n";
	printf(FONT_COLOR_RED">>> raceCondition\n"COLOR_NONE);
	setbuf(stdout, NULL);
	printf("###################\n");
	if ((pid = fork()) < 0 ) {
		printf("fork error!");
	} else if (pid > 0) {
		do {
			putc(*out_from_parent, stdout);
		} while(*out_from_parent++);
		while (pid != wait(NULL));
	} else if (pid == 0) {
			do {
			putc(*out_from_child, stdout);
		} while(*out_from_child++);
		exit(0);
	}
	printf("\n###################\n");
}

void execFunction()
{
	/*
	 * exec函数簇可以继承调用此函数簇的进程的进程ID等一系列资源
	 * */
	pid_t pid;
	printf(FONT_COLOR_RED">>> execFunction\n"COLOR_NONE);
	if ((pid = fork()) < 0) {
		printf("fork error!");
	} else if (pid > 0) {
		printf("wait child process: list file\n");
		while(pid != wait(NULL));
	} else if (pid == 0) {
		printf("ls: ");
		if (0 > execl("/usr/bin/ls","", NULL)) {
			printf("execl error\n");
			perror("");
		} else {
			printf("these infomation will never print\n");
		}	
	}
}

void uidUtils()
{
	/*
	 * 实际用户ID（RUID）: 用于在系统中标识一个用户是谁，当用户使用用户名和密码成功登录后一个UNIX系统后就唯一确定了他的RUID.
	 * 有效用户ID（EUID）：用于系统决定用户对系统资源的访问权限，通常情况下等于RUID
	 * 设置用户ID（SUID）：用于对外权限的开放。跟RUID及EUID是与用户绑定不同，它是与文件绑定。
	 *
	 *
	 * */
	printf(FONT_COLOR_RED">>> uidUtils\n"COLOR_NONE);
	printf("ruid: %d, euid: %d\n", getuid(), geteuid());
}
void systemFunction()
{
	printf(FONT_COLOR_RED">>> systemFunction\n"COLOR_NONE);

}
void exitFunction()
{
	pid_t pid;
	printf(FONT_COLOR_RED">>> exitFunction\n"COLOR_NONE);
	if ((pid = fork()) > 0) {
		while(pid != wait(NULL));
	} else {
		printf("This infomation may not display\n");
		fclose(stdout);
		_exit(1); // 此时printf不会打印出信息
	}
	if ((pid = fork()) > 0) {
		while(pid != wait(NULL));
	} else {
		printf("This infomation will display\n");
		exit(0); // 此时printf不会打印出信息
	}

}
int main(int argc, char *argv[])
{
	processIdentifier(); // 进程ID
	functionFork();      // fork函数
	exitFunction();      // exit函数 
	fileSharing();       // 子进程继承父进程
	functionVfork();     // 优先调用子进程直到exit
	functionWait();      // wait函数
	raceCondition();     // 竞争条件
	execFunction();      // exec函数
	uidUtils();          // uid相关函数
	systemFunction();    // system函数
	return 0;
}





