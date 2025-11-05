#pragma once
#include "csapp.h"


typedef struct recvdata
{
    int fd;
    char buff[4096];
    socklen_t clientlen;
    struct sockaddr clientaddr;
    //지금 현재 어느 값을 가르키고 있는지
    int pointer;
}recvdata;

void* thread_recv(void* args);

void* thread_onrecv(void* args);