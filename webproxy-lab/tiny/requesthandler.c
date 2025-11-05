#include "requesthandler.h"
#include "csapp.h"
#include <sys/stat.h>
#include "sendsocket.h"
#include <stdlib.h> 
#include <unistd.h>
#include <stdio.h>


int parse_uri(requestdata* data, char* path);
void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg);
void serve_static(requestdata* data);
void serve_dynamic(requestdata* data);

const char* get_filetype(requestdata* data);
void job_doit(void* args)
{
    printf("job queue\n");
    requestdata* data = (requestdata*)args;
    int isstatic = parse_uri(data, data->path);
    //post 요청 자체를 못 받아옴 어려움 티비;;
    if (data->rt == post)
    {
        clienterror(data->fd, "Not Implement", "501", "Skill Issue ;(", "sry");
        return;
    }
    //유효하지 않을 때, 
    if (isstatic < 0)
    {
        return;
    }
    
    if (isstatic)
    {
        //정적 어쩌구
        serve_static(data);
        
    }
    else
    {
        //동적 어쩌구 하기
        serve_dynamic(data);
    }
}

int parse_uri(requestdata* data, char* path)
{
    printf("path: %s \n", path);
    if (strstr(data->path, "/cgi-bin"))
    {
        char buff[1024];
        sprintf(buff, ".%s", path);
        struct stat sb;
        // 파일이 존재 하지 않음.
        if (stat(buff, &sb) < 0)
        {
            clienterror(data->fd, "File doesn't exist", "404", "Not Found", "Server could not file the reqeusted file.");
            return -1;
        }
        if (!S_ISREG(sb.st_mode))
        {
            clienterror(data->fd, "No Authority", "403", "Forbidden", "Not a regular file.");
            return -1;
        }
        return 0;
    }
    else
    {
        char buff[1024];
        sprintf(buff, ".%s", path);
        struct stat sb;
        // 파일이 존재 하지 않음.
        if (stat(buff, &sb) < 0)
        {
            clienterror(data->fd, "File doesn't exist", "404", "Not Found", "Server could not file the reqeusted file.");
            return -1;
        }
        if (!S_ISREG(sb.st_mode))
        {
            clienterror(data->fd, "No Authority to access file", "403", "Forbidden", "Not a regular file.");
            return -1;
        }
        return 1;
    }
}

void clienterror(int fd, char* cause, char* errnum, char* shortmsg, char* longmsg)
{
    //todo : sendsocket으로 바로 보내기.
    errordata* err = (errordata*)malloc(sizeof(errordata));
    err->fd = fd;
    err->shortmsg = shortmsg;
    err->errnum = errnum;
    err->longmsg = longmsg;

    pthread_t thread;
    pthread_create(&thread, NULL, thread_senderror, err);
    pthread_detach(thread);
}

void serve_dynamic(requestdata* data)
{
    int p_id = fork();
    char* emptylist = { NULL };
    if (p_id == 0)
    {
        char p[100] = ".";
        sprintf(p,  ".%s", data->path);
        setenv("QUERY_STRING", data->data, 1);
        dup2(data->fd, STDOUT_FILENO);
        execve(p, emptylist, environ);
        return 0;
    }
    Wait(NULL);

}
void serve_static(requestdata* data)
{
    char p[100] = ".";
    sprintf(p,  ".%s", data->path);
    int fd = open(p, O_RDONLY, 0);
    char buff[1<<14];
    ssize_t n =  read(fd, buff, sizeof(buff));
    senddata* send = malloc(sizeof(senddata));
    memcpy(send->content, buff, n);
    struct stat s;
    fstat(fd, &s);
    send->length = s.st_size;
    send->fd = data->fd;
    memcpy(send->contenttype, get_filetype(data), 16);
    pthread_t thread;
    pthread_create(&thread, NULL, thread_sendstart, send);
    pthread_detach(thread);
    close(fd);
}
const char* get_filetype(requestdata* data)
{
    if (strstr(data->path, ".png"))
        return "image/png";
    else if (strstr(data->path, ".html"))
        return "text/html";
    else if (strstr(data->path, ".gif"))
        return "image/gif";
    else if (strstr(data->path, ".jpg"))
        return "image/jpeg";
    else
        return "text/plain";
}