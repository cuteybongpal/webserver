#include "jobqueue.h"
#include <stdio.h>

void enqueue(jobqueue* q, action* ac)
{
    //락 걸기
    pthread_mutex_lock(&(q->lock));
    ac->next = NULL;
    if (q->head == NULL)
    {
        q->head = ac;

        
        q->tail = ac;
        pthread_mutex_unlock(&(q->lock));
        return;
    }

    q->tail->next = ac;
    q->tail = ac;
    //락 해제
    pthread_mutex_unlock(&q->lock);
}

action* dequeue(jobqueue* q)
{   
    pthread_mutex_lock(&(q->lock));
    //잡큐 자체가 크기가 0이라면 NULL을 리턴
    if (q->head == NULL)
    {
        pthread_mutex_unlock(&(q->lock));
        return NULL;
    }
    
    action* ac = q->head;
    q->head = ac->next;
    ac->next = NULL;
    //요소를 전부 빼줬다면, tail도 NULL로 만듦
    if (q->head == NULL)
        q->tail = NULL;
    pthread_mutex_unlock(&(q->lock));
    return ac;
}
void init(jobqueue* q)
{
    q->head = NULL;
    q->tail = NULL;

    pthread_mutex_init(&(q->lock), NULL);
}