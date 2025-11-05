#include "sendsocket.h"
#include <unistd.h>
#include <stdio.h>
#include "csapp.h"

const char* sendGETresponseformat = "HTTP/1.0 200 OK\r\nContent-Type: %s\r\nConection: close\r\nContent-Length: %ld\r\n\r\n";
const char* senderrorformat = "HTTP/1.0 %s %s\r\nContent-Type: text/html\r\nConection: close\r\nContent-Length: 55\r\n\r\n<html><body><h1>err</h1><p>errrrrrrrr</p></body></html>";

void* thread_sendstart(void* args)
{
    senddata* data = (senddata*)args;
    char buff[1<<15] ="";
    int headerlen = sprintf(buff, sendGETresponseformat, data->contenttype, data->length);
    char* s = buff + headerlen;
    memcpy(s, data->content, data->length);
    send(data->fd, buff, sizeof(buff), 0);
    free(data);
    close(data->fd);
    printf("send success!\n");
}

void* thread_senderror(void* args)
{
    errordata* data = (errordata*)args;
    char buff[1<<12] = "";
    snprintf(buff, sizeof(buff), senderrorformat, data->errnum, data->shortmsg);
    send(data->fd, buff, sizeof(buff), 0);
    printf("%s %s\n", data->errnum, data->shortmsg);
    free(data);
    close(data->fd);
}