
#include <time.h>
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
#include <ifaddrs.h>

#include <pthread.h>


#include "turnclient.h"
#include "sockaddr_util.h"


#include "gather.h"




static void TurnStatusCallBack(void *ctx, TurnCallBackData_T *retData)
{
    
    struct turn_allocation_result *turnResult = (struct turn_allocation_result *)ctx;

    
    if ( retData->turnResult == TurnResult_AllocOk ){

        sockaddr_copy((struct sockaddr *)&turnResult->activeTurnServerAddr, 
                      (struct sockaddr *)&retData->TurnResultData.AllocResp.activeTurnServerAddr);

        
        sockaddr_copy((struct sockaddr *)&turnResult->rflxAddr, 
                      (struct sockaddr *)&retData->TurnResultData.AllocResp.rflxAddr);

        sockaddr_copy((struct sockaddr *)&turnResult->relAddr, 
                      (struct sockaddr *)&retData->TurnResultData.AllocResp.relAddr);

        turnResult->update_turninfo();

    }else if (retData->turnResult == TurnResult_CreatePermissionOk) {
        
        turnResult->turnPerm.ok = true;
        
        turnResult->update_turninfo();

    }else if (retData->turnResult == TurnResult_RelayReleaseComplete){
        
        turnResult->stunCtx = -1;
        
        memset(&turnResult->activeTurnServerAddr, 0,sizeof(struct sockaddr_storage));
        //memset(&turnResult->hostAddr, 0,sizeof(struct sockaddr_storage));
        memset(&turnResult->rflxAddr, 0,sizeof(struct sockaddr_storage));
        memset(&turnResult->relAddr, 0,sizeof(struct sockaddr_storage));
        memset(&turnResult->turnPerm, 0,sizeof(struct turn_permissions));
        
        update_turninfo = NULL;

        turnResult->update_turninfo();

    }
    
}

int gather(struct sockaddr *host_addr, 
           int requestedFamily, 
           char *user, 
           char *pass,
           struct turn_allocation_result *turnResult){
    
    int stunCtx;
    //TurnCallBackData_T TurnCbData;
    
    
    stunCtx = TurnClient_startAllocateTransaction(TEST_THREAD_CTX,
                                                  turnResult,
                                                  host_addr,
                                                  user,
                                                  pass,
                                                  turnResult->sockfd,                       /* socket */
                                                  requestedFamily,
                                                  SendRawStun,             /* send func */
                                                  NULL,  /* timeout list */
                                                  TurnStatusCallBack,
                                                  &turnResult->TurnCbData,
                                                  false);
    turnResult->stunCtx = stunCtx;

    return stunCtx;
            
}


void gatherAll(struct turn_info *turnInfo, struct listenConfig *listenConfig, void(*update_turninfo)(void))
{

    int stunCtx;
    int idx = 0;

    if( sockaddr_isSet((struct sockaddr *)&turnInfo->localIp4) &&
        sockaddr_isSet((struct sockaddr *)&turnInfo->remoteIp4) )
    {
        turnInfo->turnAlloc_44.update_turninfo = update_turninfo;
        stunCtx = gather((struct sockaddr *)&turnInfo->remoteIp4, 
                         AF_INET, 
                         turnInfo->user, 
                         turnInfo->pass,
                         &turnInfo->turnAlloc_44);
        listenConfig->socketConfig[idx].stunCtx = stunCtx;
        listenConfig->socketConfig[idx].sockfd = turnInfo->turnAlloc_44.sockfd;
        listenConfig->socketConfig[idx].user = turnInfo->user;
        listenConfig->socketConfig[idx].pass = turnInfo->pass;
        idx++;
        
        turnInfo->turnAlloc_46.update_turninfo = update_turninfo;
        stunCtx = gather((struct sockaddr *)&turnInfo->remoteIp4, 
                         AF_INET6, 
                         turnInfo->user, 
                         turnInfo->pass,
                         &turnInfo->turnAlloc_46);
        listenConfig->socketConfig[idx].stunCtx = stunCtx;
        listenConfig->socketConfig[idx].sockfd = turnInfo->turnAlloc_46.sockfd;
        listenConfig->socketConfig[idx].user = turnInfo->user;
        listenConfig->socketConfig[idx].pass = turnInfo->pass;
        idx++;
    }
    
    if( sockaddr_isSet((struct sockaddr *)&turnInfo->localIp6) &&
        sockaddr_isSet((struct sockaddr *)&turnInfo->remoteIp6) )
    {

        turnInfo->turnAlloc_64.update_turninfo = update_turninfo;
        stunCtx = gather((struct sockaddr *)&turnInfo->remoteIp6, 
                         AF_INET, 
                         turnInfo->user, 
                         turnInfo->pass,
                         &turnInfo->turnAlloc_64);
        listenConfig->socketConfig[idx].stunCtx = stunCtx;
        listenConfig->socketConfig[idx].sockfd = turnInfo->turnAlloc_64.sockfd;
        listenConfig->socketConfig[idx].user = turnInfo->user;
        listenConfig->socketConfig[idx].pass = turnInfo->pass;
        idx++;

        turnInfo->turnAlloc_66.update_turninfo = update_turninfo;
        stunCtx = gather((struct sockaddr *)&turnInfo->remoteIp6, 
                         AF_INET6, 
                         turnInfo->user, 
                         turnInfo->pass,
                         &turnInfo->turnAlloc_66);
        listenConfig->socketConfig[idx].stunCtx = stunCtx;
        listenConfig->socketConfig[idx].sockfd = turnInfo->turnAlloc_66.sockfd;
        listenConfig->socketConfig[idx].user = turnInfo->user;
        listenConfig->socketConfig[idx].pass = turnInfo->pass;
        idx++;

    }
    
    listenConfig->numSockets = idx;
}



