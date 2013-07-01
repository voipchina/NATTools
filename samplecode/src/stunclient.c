/*
** talker.c -- a datagram "client" demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <pthread.h>

#include <stunlib.h>
#include <stunclient.h>

#include "utils.h"

#define SERVERPORT "4950"    // the port users will be connecting to
static const uint32_t TEST_THREAD_CTX = 1;

static void *tickTurn(void *ptr);
void StunStatusCallBack(void *ctx, StunCallBackData_T *retData);

int main(int argc, char *argv[])
{
    int sockfd;
    struct addrinfo *servinfo, *p;
    int numbytes;
    pthread_t stunTickThread;
    StunCallBackData_T stunCbData;

    StunMessage stunRequest, stunResponse;
    char buffer[256];
    int msg_len;

    struct sockaddr_storage their_addr;
    struct sockaddr my_addr;
    socklen_t addrLen = sizeof(my_addr);

    unsigned char buf[MAXBUFLEN];
    char s[INET6_ADDRSTRLEN];

    char username[] = "evtj:h6vY";
    char password[] = "VOkJxbRl1RmTxUk/WvJxBt";
    const uint32_t priority = 1845494271;
    const uint64_t tieBreaker = 0x932FF9B151263B36LL;
    char *software= "STUN test client\0";
    StunMsgId stunMsgId;
    char msgId[] = "\xb7\xe7\xa7\x01"
        "\xbc\x34\xd6\x86"
        "\xfa\x87\xdf\xae";

    memcpy(stunMsgId.octet, msgId, sizeof(stunMsgId.octet));

    if (argc != 2) {
        fprintf(stderr,"usage: stunclient hostname\n");
        exit(1);
    }

    sockfd = createSocket(argv[1], SERVERPORT, "stunclient", 0, servinfo, &p);
    freeaddrinfo(servinfo);

    StunClient_Init(TEST_THREAD_CTX, 50, 50, NULL, false, software);
    pthread_create(&stunTickThread, NULL, tickTurn, (void*) &TEST_THREAD_CTX);

    if (getsockname(sockfd, &my_addr, &addrLen) == -1) {
      perror("getsockname");
    }

    StunClient_startBindTransaction(TEST_THREAD_CTX,
                                    NULL,
                                    p->ai_addr,
                                    &my_addr,
                                    false,
                                    username,
                                    password,
                                    priority,
                                    false,
                                    false,
                                    tieBreaker,
                                    stunMsgId,
                                    sockfd,
                                    sendRawStun,
                                    NULL,
                                    StunStatusCallBack,
                                    &stunCbData,
                                    0);

    if((numbytes = recvStunMsg(sockfd, &their_addr, &stunResponse, buf)) != -1) {
        if( stunlib_checkIntegrity(buf,
                                   numbytes,
                                   &stunResponse,
                                   password,
                                   sizeof(password)) ) {
            printf("Integrity Check OK\n");

            printf("Could print some attributes here, but check wireshark instead..\n");

        }
    }

    close(sockfd);

    return 0;
}

void StunStatusCallBack(void *ctx, StunCallBackData_T *retData)
{
    //ctx points to whatever you initialized the library with. (Not used in this simple example.)
    printf("StunStatusCallBack called\n");
}

static void *tickTurn(void *ptr)
{
    struct timespec timer;
    struct timespec remaining;
    uint32_t  *ctx = (uint32_t *)ptr;

    timer.tv_sec = 0;
    timer.tv_nsec = 50000000;

    for(;;) {
        nanosleep(&timer, &remaining);
        StunClient_HandleTick(*ctx);
    }

}
