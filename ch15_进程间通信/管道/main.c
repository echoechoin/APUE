#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

// 关闭读管道，产生SIGPIPE信号，处理或者忽略该信号后，write返回-1，errno被设置为EPIPE
void close_read (int pipe_in, int pipe_out) {
    close(pipe_out);
    if (0 > write(pipe_in, "test", 4)) {
        if (errno == EPIPE)
            printf("pipe_out has been closed.\n");
        else
            perror("write");
    }
}

// 关闭写管道，read返回0
void close_write(int pipe_in, int pipe_out) {
    close(pipe_in);
    char buff[1024];
    if (0 >= read(pipe_in, buff, sizeof(buff))) {
        printf("pipe_in has been closed.\n");
    }
}

int main () {
    signal(SIGPIPE,SIG_IGN);
    int pipe_fd[2], pipe_in, pipe_out;
    if (0 > pipe(pipe_fd)) {
        perror("pipe");
        exit(0);
    }
    pipe_in = pipe_fd[1];
    pipe_out = pipe_fd[0];
    // close_read(pipe_in, pipe_out);
    close_write(pipe_in, pipe_out);
    return 0;
}

