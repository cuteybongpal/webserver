#pragma once
typedef enum
{
    get,
    post
} requesttype;

typedef struct
{
    int fd;
    requesttype rt;
    char path[1024];
    char data[1024];
} requestdata;

void job_doit(void* args);

