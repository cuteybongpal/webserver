#pragma once
#include <unistd.h>
#include <sys/socket.h>
typedef struct 
{
    int fd;
    unsigned long length;
    char content[1<<14];
    char contenttype[16];
}senddata;

typedef struct 
{
    int fd;
    char* errnum;
    char* shortmsg;
    char* longmsg; 
}errordata;

void* thread_sendstart(void* args);

void* thread_senderror(void *args);