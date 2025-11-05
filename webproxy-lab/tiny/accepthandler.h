#pragma once
#include "csapp.h"

typedef struct acceptdata
{
    int fd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;

}acceptdata;

void handlerinit();

void* thread_acceptHandler(void* args);
void* thread_onaccept(void* args);
