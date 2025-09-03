#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void be_a_child(int *pipe) {
    write(pipe[1], "Hi hi hi hi hi", 16);
}

void be_a_parent(int *pipe) {
    sleep(1);
    char *buf = calloc(1, 16);
    read(pipe[0], buf, 16);
    printf("Child said: %s\n", buf);
    free(buf);
    buf = NULL;
}

int main(int argc, char **argv) {
    int p[2];
    pipe(p);

    pid_t c = fork();
    if (c == 0) {
        be_a_child(p);
    } else {
        printf("My child has ID: %i\n", c);
        be_a_parent(p);
    }
    sleep(2);
}