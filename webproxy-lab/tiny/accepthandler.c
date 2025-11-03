#include "accepthandler.h"

int listenfd = 0;
char* hostname, port = NULL;

void handlerinit(int lfd, char* host, char* p)
{
    listenfd = lfd;
    hostname = host;
    port = p;
}
//멀티스레딩 함수
void* thread_acceptHandler(void* args)
{
    int connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    clientlen = sizeof(clientaddr);
    connfd = Accept(listenfd, (SA*)&clientaddr, &clientlen);
    //만약 Accecpt 요청을 받았다면,
    if (connfd == -1)
        return;

    //비동기 함수 실행.
    //비동기 함수라는 게 없어서 그냥 멀티스레딩으로 함.
    //인자 넣기
    acceptdata* acc = (acceptdata*)malloc(sizeof(acceptdata));
    acc->fd = connfd;
    acc->clientlen = clientlen;
    acc->clientaddr = clientaddr;
    
    pthread_t thread;
    pthread_create(&thread, NULL, thread_onaccept, acc);
    pthread_detach(thread);
}
//이거도 멀티 스레딩 함수임.
void* thread_onaccept(void* args)
{
    acceptdata* acc = (acceptdata*)args;
    Getnameinfo((SA *)&(acc->clientaddr), acc->clientlen, hostname, MAXLINE, port, MAXLINE,0);
    //todo : 이제 패킷을 분리하고 합치고 분리하고 그 패킷을 처리하면 될 듯함 ㅋㅋ
    //아니 근데 connect하고 패킷 확인 후에 close를 해줄 거면 설계상으로 이걸 분리해놔야하는데??
    //큰일 났띠
    free(args);
    
    thread_acceptHandler(NULL);
}