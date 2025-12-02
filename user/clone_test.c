#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define PGSIZE 4096

void thread_main() {
    printf("Inside child thread! My PID is %d\n", getpid());
    exit(0);
}

int main() {
    void *stack;

    // Allocate memory aligned to PGSIZE
    stack = malloc(PGSIZE);
    if(stack == 0) {
        printf("Failed to allocate stack\n");
        exit(1);
    }

    int pid = clone(stack);
    if(pid < 0) {
        printf("clone failed\n");
        exit(1);
    }

    if(pid == 0) {
        // Child thread
        thread_main();
    } else {
        // Parent process
        wait(0); // wait for child
        printf("Parent done waiting for child thread (pid=%d)\n", pid);
    }

    free(stack); // good hygiene
    exit(0);
}
