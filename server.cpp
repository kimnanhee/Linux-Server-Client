//server
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <error.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sysinfo.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>

#include "msgHeader.h"

#define SIZE sizeof(struct sockaddr_in)
#define MAX_SIZE 2048

void *recvMsg(void *parms);
int sendMsg(char *msg, int size, char *ip, int port);
int sendMsg2(char *msg, int size, struct sockaddr* sock, int sock_len);

int main()
{
    pthread_t tid;
    if(pthread_create(&tid, NULL, recvMsg, NULL)) // 스레드 생성, 스레드는 recvMsg함수를 수행
    {
        printf("thread create fail\n");
    }
    while(1)
        sleep(1);
}

void *recvMsg(void *parms)
{
    struct sockaddr_in mySock = {AF_INET, htons(PORT_SERVER_R), INADDR_ANY};
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	int ret = bind(sockfd, (struct sockaddr *) &mySock, sizeof(mySock));
	if(ret < 0)
	{
		extern int errno;
		printf("ret(%d), errno(%d, %s)\n", ret, errno, strerror(errno));
	}
	struct sockaddr_in sock;
	int sock_len;
	char msg[MAX_SIZE];
	while(1)
	{
		memset(msg, 0, MAX_SIZE);
		int ret = recvfrom(sockfd, &msg, MAX_SIZE, 0, (struct sockaddr *) &sock, (socklen_t*) &sock_len);
		if(ret < 0)
		{
			extern int errno;
			printf("ret(%d), errno(%d, %s)\n", ret, errno, strerror(errno));
		}
		else 
		{
			printf("%d msg received\n", ret);
            struct msgHeader hdr = *(struct msgHeader *)msg;

            Read(hdr);
            switch(hdr.cmdId)
            {
                case reqGetTime: // 시간 정보 요청
                {
                    printf("GetTime\n");
                    struct dataTime data = *(struct dataTime*) (msg + sizeof(struct msgHeader));
                    time_t clock;
                    data.time = time(&clock); // 시간 얻어오기
                    data.retValue = ret;
                    printf("%ld %d\n", data.time, data.retValue); // 서버에 출력

                    struct msgTime mTime;
                    mTime.hdr.cmdId = rspGetTime;
                    mTime.data.time = data.time;
                    mTime.data.retValue = data.retValue;
                    Write(mTime);

                    sock.sin_port = htons(PORT_CLIENT_R);
                    sendMsg2((char*)&mTime, sizeof(mTime), (struct sockaddr*)&sock, sock_len);
                    break;
                }
                case reqSysInfo: // 시스템 정보 요청
                {
                    printf("SysInfo\n");
                    struct dataSysInfo data = *(struct dataSysInfo*) (msg + sizeof(struct msgHeader));
                    char hostname[64];
                    gethostname(hostname, 64); // 호스트 이름 얻어오기
                    strcpy(data.hostname, hostname);
                    struct sysinfo info;
                    sysinfo(&info); // 시스템 정보 얻어오기
                    data.totalram = info.totalram;
                    data.freeram = info.freeram;
                    data.retValue = ret;
                    printf("%s %ld %ld %d\n", data.hostname, data.totalram, data.freeram, data.retValue); // 서버에 출력
                    
                    struct msgSysInfo mSys;
                    mSys.hdr.cmdId = rsqSysInfo;
                    strcpy(mSys.data.hostname, data.hostname);
                    mSys.data.totalram = data.totalram;
                    mSys.data.freeram = data.freeram;
                    mSys.data.retValue = data.retValue;
                    Write(mSys);

                    sock.sin_port = htons(PORT_CLIENT_R);
                    sendMsg2((char*)&mSys, sizeof(mSys), (struct sockaddr* )&sock, sock_len);
                    break;
                }
            }
		}
        printf("end\n");
	}
	close(sockfd);
}

int sendMsg2(char *msg, int size, struct sockaddr* sock, int sock_len)
{
    struct sockaddr_in mySock = {AF_INET, htons(PORT_SERVER_S), INADDR_ANY};
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    int ret = bind(sockfd, (struct sockaddr *)&mySock, sizeof(mySock));
    if(ret < 0)
    {
        extern int errno;
        printf("ret(%d), errno(%d. %s)\n", ret, errno, strerror(errno));
    }
    sendto(sockfd, msg, size, 0, sock, sock_len);
    close(sockfd);
    return 0;
}

int sendMsg(char *msg, int size, char *ip, int port)
{
    struct sockaddr_in mySock = {AF_INET, htons(PORT_SERVER_S), INADDR_ANY};
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    int ret = bind(sockfd, (struct sockaddr *)&mySock, sizeof(mySock));
    if(ret < 0)
    {
        extern int errno;
        printf("ret(%d), errno(%d. %s)\n", ret, errno, strerror(errno));
    }

    struct sockaddr_in sock = {AF_INET, htons(port), INADDR_ANY};
    sock.sin_addr.s_addr = inet_addr(ip);
    sendto(sockfd, msg, size, 0, (struct sockaddr *)&sock, sizeof(sock));
    close(sockfd);
    return 0;
}