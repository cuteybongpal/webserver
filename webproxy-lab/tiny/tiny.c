/* $begin tinymain */
/*
 * tiny.c - A simple, iterative HTTP/1.0 Web server that uses the
 *     GET method to serve static and dynamic content.
 *
 * Updated 11/2019 droh
 *   - Fixed sprintf() aliasing issue in serve_static(), and clienterror().
 */

#include "tiny.h"
//일단 패킷의 헤더를 읽어와서 요청의 크기를 알아 온후에, 다 받았다면,
//그 때부터 doit을 실행하는 게 맞을 듯??
//근데 함수 이름이 굉장히 마음에 들지 않는데 흠 ㅋㅋㅋ 이름 바꿔도 되려나 모르겠다.
jobqueue* serverjobqueue = NULL;

int main(int argc, char **argv)
{
    serverjobqueue = calloc(1, sizeof(jobqueue));
    jq_init(serverjobqueue);
    
    int listenfd;
    char hostname[MAXLINE], port[MAXLINE];
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    
    /* Check command line args */
    if (argc != 2)
    {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }

    listenfd = Open_listenfd(argv[1]);
    pthread_t thread;
    handlerinit(listenfd, hostname, port);
    pthread_create(&thread, NULL, thread_acceptHandler, NULL);
    while (1)
    {
        action* cur = dequeue(serverjobqueue);
        while (cur != NULL)
        {
            cur->fn(cur->args);
            free(cur->args);
            free(cur);
            cur = dequeue(serverjobqueue);
        }
        usleep(50000);
    }
    
}

jobqueue* getjobqueue()
{
    return serverjobqueue;
}