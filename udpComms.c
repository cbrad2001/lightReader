#include "include/udpComms.h"
#include "include/sampler.h"

#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <stdio.h>
#include <limits.h>
#include <netdb.h>
#include <unistd.h>	
#include <float.h>
#include <pthread.h>
#include <sys/socket.h>

#define PORT 12345
#define MAX_LEN 1024

#define CMD_HELP "help"
#define CMD_COUNT "count"
#define CMD_LENGTH "length"
#define CMD_HISTORY "history"
#define CMD_GET_N "get 10"
#define CMD_DIPS "dips"
#define CMD_STOP "stop"
#define ENTER "\n"

static pthread_t udpThreadID;

static struct sockaddr_in sock;
static int socketDescriptor;
static socklen_t sock_sz;

static void* udpCommandThread(void *vargp);

static void initSocket()
{
    memset(&sock, 0, sizeof(sock));
    sock.sin_family = AF_INET;
    sock.sin_addr.s_addr = htonl(INADDR_ANY);
    sock.sin_port = htons(PORT);

    socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);
    if (socketDescriptor == -1){
        perror("Can't create socket\n");
        exit(1);
    }

    if (bind (socketDescriptor, (struct sockaddr*) &sock, sizeof(sock)) == -1){
        perror("Failed to bind socket\n");
        exit(1);
    }
    sock_sz = sizeof(sock);
}

void udp_startSampling(void)
{
    initSocket();
    pthread_create(&udpThreadID, NULL, &udpCommandThread, NULL);
}

void udp_stopSampling(void)
{
    close(socketDescriptor);
    pthread_join(udpThreadID, NULL);
}


static void* udpCommandThread(void *vargp)
{
    printf("UDP Thread Starting\n");
    char recvBuffer[MAX_LEN];
    char cmdHistory[MAX_LEN];
    char sendBuffer[MAX_LEN];
    while(1){
        int bytesRx = recvfrom(socketDescriptor, recvBuffer, MAX_LEN, 0, (struct sockaddr*)&sock, &sock_sz);
        if (bytesRx == -1 ){
            printf("Can't receive data \n");
            exit(1);
        } 
        recvBuffer[MAX_LEN] = '\0'; //null terminated string

        if (strcmp(recvBuffer, ENTER) == 0){
            strncpy(recvBuffer, cmdHistory, MAX_LEN-1); // call prev command if 'enter'
        }
        strncpy(cmdHistory,recvBuffer,MAX_LEN-1);       //store history for enter command

        if (strcmp(recvBuffer, CMD_HELP)==0){
            sprintf(sendBuffer, "Accepted command examples:\n");
			strcat(sendBuffer, "count        -- display total number of samples taken.\n");
			strcat(sendBuffer, "length   -- display number of samples in history (both max, and current).\n");
			strcat(sendBuffer, "history    -- display the full sample history being saved.\n");
			strcat(sendBuffer, "get 10       -- display the 10 most recent history values.\n");
            strcat(sendBuffer, "dips       -- display number of photoresistor dips.\n");
			strcat(sendBuffer, "stop         -- cause the server program to end.\n");
            strcat(sendBuffer, "<enter>         -- repeat last command.\n");

			sendto(socketDescriptor,sendBuffer, strnlen(sendBuffer,MAX_LEN),0,(struct sockaddr *) &sock, sock_sz);
        }
        else if(strcmp(recvBuffer, CMD_COUNT) == 0){
			sprintf(sendBuffer, "Number of samples taken = %lld. \n",Sampler_getNumSamplesTaken());
			sendto(socketDescriptor,sendBuffer, strnlen(sendBuffer,MAX_LEN),0,(struct sockaddr *) &sock, sock_sz);
		}
        else if(strcmp(recvBuffer, CMD_LENGTH) == 0){
			sprintf(sendBuffer, "History can hold %d samples. \n Currently holding %d samples.",
                Sampler_getNumSamplesInHistory(),Sampler_getHistorySize());
			sendto(socketDescriptor,sendBuffer, strnlen(sendBuffer,MAX_LEN),0,(struct sockaddr *) &sock, sock_sz);
		}
        else if(strcmp(recvBuffer, CMD_HISTORY) == 0){
            //stub
        }

        else if (strcmp(recvBuffer, CMD_GET_N) == 0){
            //stub
        }

        else if (strcmp(recvBuffer, CMD_DIPS) == 0){
            //stub
        }

        else if (strcmp(recvBuffer, CMD_STOP) == 0){
            //stub
        }
        else{
            sprintf(sendBuffer, "Unknown command.\n");
			sendto(socketDescriptor,sendBuffer, strnlen(sendBuffer,MAX_LEN),0,(struct sockaddr *) &sock, sock_sz);
        }
    }
    return 0;
}

