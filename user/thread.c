#include "kernel/types.h"
#include "user/user.h"
#include "user/thread.h"
#include "kernel/riscv.h"


int thread_create(void *(start_routine)(void*), void *arg){

    //allocates exactly one page for the user stack.
    void *stack = malloc(PGSIZE);
    //for the parent, this routine returns 0 on success and -1 on failure.
    if (stack == 0)
        return -1;
    // stack grows downwards, so we want to sue the top of the page
    void *stack_top = (char *)stack + PGSIZE;

    // clone() creates a new kernel thread that shares the parent's
    // address space, but uses this new user stack.
    int pid = clone(stack_top);
    //for the parent, this routine returns 0 on success and -1 on failure.
    if (pid < 0) {
        return -1;
    }
    if (pid == 0) {
        //For the child, it calls start_routine() to start thread execution with the input argument arg.
        start_routine(arg);
        //When start_routine() returns, it should terminate the child thread by exit(). 
        exit(0);
    }
    return 0;

}

void lock_init(struct lock_t* lock){
    //creating and initilizing a lock
    lock->locked = 0;

}


void lock_acquire(struct lock_t* lock){
    // __sync_lock_test_and_set atomically sets "locked" to 1 and returns
    // the old value. We keep spinning while someone else holds the lock.
    while (__sync_lock_test_and_set(&lock->locked, 1) != 0)
        ;
    __sync_synchronize();
}

void lock_release(struct lock_t* lock){
    //releasing the lock so another thread can aquire it
      __sync_synchronize();
    __sync_lock_release(&lock->locked);
}