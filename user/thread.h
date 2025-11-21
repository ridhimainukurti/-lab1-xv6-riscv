#ifndef THREAD_H
#define THREAD_H

#include "kernel/types.h"

struct lock_t {
  uint locked;
};

typedef struct lock_t lock_t;

int thread_create(void *(start_routine)(void*), void *arg);
void lock_init(struct lock_t* lock);
void lock_acquire(struct lock_t* lock);
void lock_release(struct lock_t* lock);

#endif