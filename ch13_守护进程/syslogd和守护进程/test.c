#include <syslog.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
int main ()
{
    // 转换为守护进程
    umask(0);
    int pid = fork();
    if (pid > 0)
        exit(0);
    setsid();
    chroot("/");

    // 单实例
    int fd = open("/var/run/test.pid", O_RDWR|O_CREAT);
    if (fd < 0) {
        perror("open error:");
        exit(1);
    }
    pid = getpid();
    char pid_buff[10];
    struct flock flk;
    flk.l_type = F_WRLCK;
    flk.l_whence = SEEK_SET;
    flk.l_start = 0;
    flk.l_len = 0;
    if (fcntl(fd, F_SETLK, &flk) < 0) {
        perror("test is running");
        exit(0);
    }
    ftruncate(fd, 0);
    sprintf(pid_buff, "%d", pid);
    write(fd, pid_buff, strlen(pid_buff));

    for (int i = 0; i <= 2; i++) {
        close(i);
    }

    // syslog
    for(;;) {
        openlog ("test", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL2);

        syslog (LOG_NOTICE, "Program started by User %d", getuid (), "/var/log/test.log");
        syslog (LOG_INFO, "A tree falls in a forest", "/var/log/test.log");
        closelog ();
        sleep(1);
    }
    return 0;
}