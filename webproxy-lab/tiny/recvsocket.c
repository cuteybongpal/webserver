#include "recvsocket.h"
#include "requesthandler.h"
#include "jobqueue.h"
#include "tiny.h"

requestdata* makerequestdata(recvdata* arg);

void* thread_recv(void* args)
{
    recvdata* data = (recvdata*)args;
    char tempbuff[4096];

    long size = recv(data->fd, tempbuff, 4096, 0);
    printf("%s \n", tempbuff);
    //패킷 받아오기 실패.
    if (size == -1)
        return;
    if (size == 0)
        return;
    tempbuff[size] = '\0';
    memcpy(data->buff + data->pointer, tempbuff, size);
    data->pointer += size;
    pthread_t thread;
    pthread_create(&thread, NULL, thread_onrecv, data);
    pthread_detach(thread);
}

void* thread_onrecv(void* args)
{
    //일단 요청 전체를 받아왔는지 아닌지 알아야함.
    recvdata* data = (recvdata*)args;
    char* start = data->buff;
    //GET요청
    if (strncmp(data->buff, "GET ", 4) == 0)
    {
        int sliceflag = 0;
        while (data->pointer != 0)
        {
            int headersize = 0;
            sliceflag = 0;
            for (int i = 0; i <= data->pointer - 4; i++)
            {
                headersize++;
                if (strncmp(data->buff + i, "\r\n\r\n", 4) == 0)
                {
                    sliceflag = 1;
                    headersize += 4;
                    break;
                }
            }
            //todo : 여기에 요청 처리하는 함수를 잡큐에 넣어주면 될듯?
            
            if (sliceflag == 0)
                break;
            requestdata* requestd = makerequestdata(data);
            memmove(data->buff, data->buff + headersize, 4096 - headersize);
            data->pointer -= headersize;
            
            action* ac = malloc(sizeof(action));
            ac->args = requestd;
            ac->fn = job_doit;
            printf("success to read\n");
            enqueue(getjobqueue(), ac);
        }
        if (sliceflag == 0)
        {
            thread_recv(data);
        }
    }
    //POST요청
    //일단 스킵
    // if (strncmp(data->buff, "POST ", 5) == 0)
    // {
    //     char* line_start = strstr(data->buff, "Content-Length: ");
    //     char* line_end = line_start + 17;
    //     while (*line_end == '/r')
    //     {
    //         line_end += 1;
    //     }
        
    //     char* size;
        
    //     memcpy(size, line_start, line_end);
    //     long contentsize = strtol(size, line_end, 10);

    //     if (line_end + 4 + contentsize <= data->buff + data->pointer)
    //     {
    //         //todo : POST요청 처리하기
    //     }
    //     else
    //     {
    //         thread_recv(data);
    //     }
    // }

    free(data);
}

requestdata* makerequestdata(recvdata* data)
{
    unsigned char *p = (unsigned char*)data->buff;
    for (int i=0; i<16; i++)
        fprintf(stderr, "%02X ", p[i]);
    fprintf(stderr, "\n");
    printf("%s \n", data->buff);
    requestdata* d = (requestdata*)malloc(sizeof(requestdata));
    char* start = data->buff;
    printf("hhhh %c \n", *start);
    printf("hhhh %c \n", *(start+1));
    printf("hhhh %c \n", *(start+2));
    printf("hhhh %c \n", *(start+3));
    d->fd = data->fd;
    if (strncmp(data->buff, "GET ", 4) == 0)
    {
        d->rt = get;
        start += 4;
    }
    
    else
    {
        d->rt = post;
        start += 5;
    }
    int noargsflag = 1;
    char* ppath = d->path;
    for (char* i = start; *i && *i!=' ' && *i!='\r' && *i!='\n'; i++)
    {
        if (*i == '?')
        {
            noargsflag = 0;
            start = i + 1;
            break;
        }
        *ppath = *i;
        ppath++;
    }
    *ppath = '\0';
    if (!noargsflag)
    {
        char* data = d->data;
        for (char* i = start; *i != ' '; i++)
        {
            *data = *i;
            data++;
        }
    }
    return d;
}