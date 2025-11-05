#pragma once

#include <pthread.h>

typedef struct action action;

typedef struct action
{
    //인자
    void* args;
    //함수 포인터
    void (*fn)(void*);
    action* next;
};

typedef struct jq
{
    action* head;
    action* tail;
    pthread_mutex_t lock;
} jobqueue;

void enqueue(jobqueue* q, action* ac);

action* dequeue(jobqueue* q);

void jq_init(jobqueue* q);