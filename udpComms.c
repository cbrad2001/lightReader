#include "include/udpComms.h"
#include "include/sampler.h"
#include "include/terminal.h"

#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <stdio.h>
#include <limits.h>
#include <netdb.h>
#include <unistd.h>	
#include <float.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/socket.h>

//run: netcat -u 192.168.7.2 12345

#define PORT 12345
#define MAX_LEN 1024
#define INVALID_VAL DBL_MAX

#define CMD_HELP    "help\n"
#define CMD_COUNT   "count\n"
#define CMD_LENGTH  "length\n"
#define CMD_HISTORY "history\n"     //todo
#define CMD_GET_N   "get "         //todo
#define CMD_DIPS    "dips\n"
#define CMD_STOP    "stop\n"
#define ENTER       '\n'

static pthread_t udpThreadID;

static struct sockaddr_in sock;
static int socketDescriptor;
static bool isConnected;
static socklen_t sock_sz;

static void* udpCommandThread(void *vargp);

void udp_startSampling(void)
{
    isConnected = true;
    pthread_create(&udpThreadID, NULL, &udpCommandThread, NULL);
}

void udp_stopSampling(void)
{
    isConnected = false;
    pthread_join(udpThreadID, NULL);
}

static void* udpCommandThread(void *vargp)
{
    memset(&sock, 0, sizeof(sock));
    sock.sin_family = AF_INET;
    sock.sin_addr.s_addr = htonl(INADDR_ANY);
    sock.sin_port = htons(PORT);

    socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);
    if (socketDescriptor == -1)
    {
        perror("Can't create socket\n");
        exit(1);
    }

    if (bind (socketDescriptor, (struct sockaddr*) &sock, sizeof(sock)) == -1)
    {
        perror("Failed to bind socket\n");
        exit(1);
    }
    sock_sz = sizeof(sock);
    printf("UDP Thread Starting\n");
    char recvBuffer[MAX_LEN];
    char cmdHistory[MAX_LEN];
    char sendBuffer[MAX_LEN];
    isConnected = true;

    while(isConnected){
        int bytesRx = recvfrom(socketDescriptor, recvBuffer, MAX_LEN, 0, (struct sockaddr*)&sock, &sock_sz);
        if (bytesRx == -1 )
        {
            printf("Can't receive data \n");
            exit(1);
        } 

        if (recvBuffer[0] == ENTER)
        {
            strncpy(recvBuffer, cmdHistory, MAX_LEN); // call prev command if 'enter'
        }
        strncpy(cmdHistory,recvBuffer,MAX_LEN);       //store history for enter command
        recvBuffer[MAX_LEN] = '\0'; //null terminated string

        if (strcmp(recvBuffer, CMD_HELP)==0)
        {
            sprintf(sendBuffer, "Accepted command examples:\n");
			strcat(sendBuffer, "count       -- display total number of samples taken.\n");
			strcat(sendBuffer, "length      -- display number of samples in history (both max, and current).\n");
			strcat(sendBuffer, "history     -- display the full sample history being saved.\n");
			strcat(sendBuffer, "get n       -- display the n most recent history values.\n");
            strcat(sendBuffer, "dips        -- display number of photoresistor dips.\n");
			strcat(sendBuffer, "stop        -- cause the server program to end.\n");
            strcat(sendBuffer, "<enter>     -- repeat last command.\n");

			sendto(socketDescriptor,sendBuffer, strnlen(sendBuffer,MAX_LEN),0,(struct sockaddr *) &sock, sock_sz);
        }
        else if(strcmp(recvBuffer, CMD_COUNT) == 0)
        {
			sprintf(sendBuffer, "Number of samples taken = %lld. \n",Sampler_getNumSamplesTaken());
			sendto(socketDescriptor,sendBuffer, strnlen(sendBuffer,MAX_LEN),0,(struct sockaddr *) &sock, sock_sz);
		}
        else if(strcmp(recvBuffer, CMD_LENGTH) == 0)
        {
			sprintf(sendBuffer, "History can hold %d samples. \nCurrently holding %d samples.\n",
                Sampler_getNumSamplesInHistory(),Sampler_getHistorySize());
			sendto(socketDescriptor,sendBuffer, strnlen(sendBuffer,MAX_LEN),0,(struct sockaddr *) &sock, sock_sz);
		}
        else if(strcmp(recvBuffer, CMD_HISTORY) == 0)
        {
            int historySize = Sampler_getHistorySize();
            double *historyBuf = Sampler_getHistoryInOrder(historySize);
            int historyBufSize = Sampler_getHistorySize();
            sprintf(sendBuffer, "Buffer History :\n");

            for (int i = 0; i < historyBufSize; i++)    //print all elements
                {
                double val = historyBuf[i];
                if (val != INVALID_VAL)
                {
                    char currVal[MAX_LEN];
                    sprintf(currVal, "%.3f, ", val);
                    strcat(sendBuffer, currVal);   // TODO: split and send UDP packets
                }
                if (i % 20 == 0)     //newline every 10th index for clarity
                {
                    strcat(sendBuffer, "\n");   // need to handle multiple packet case
                                                // might be easier to just sent a new packet every 20?
                }
            }
            if (historyBufSize%10 != 0)
                strcat(sendBuffer, "\n");
            
            free(historyBuf);
        }

        else if (strncmp(recvBuffer, CMD_GET_N, 4) == 0)
        {
            char *startOfN = recvBuffer + 4; // 4th position is the start of n
            int n = atoi(startOfN);
            double *historyBuf = Sampler_getHistoryInOrder(n);
            int historyBufSize = Sampler_getHistorySize();
            memset(sendBuffer, 0, MAX_LEN); // 1024 bytes per buffer

            if (n > historyBufSize)
            {
                char *errMsg = "The value given is greater than the number of valid samples in the history. Please set it to under ";
                sprintf(sendBuffer, "%s %d.\n", errMsg, historyBufSize);
                sendto(socketDescriptor,sendBuffer, strnlen(sendBuffer,MAX_LEN),0,(struct sockaddr *) &sock, sock_sz);
            }
            else if (n < 0)
            {
                char *errMsg = "Please give a valid non-negative number.";
                sprintf(sendBuffer, "%s\n", errMsg);
                sendto(socketDescriptor,sendBuffer, strnlen(sendBuffer,MAX_LEN),0,(struct sockaddr *) &sock, sock_sz);
            }
            else {
                // sprintf(sendBuffer, "N most recent:\n");
                int currentPos = 0;
                while (currentPos < n)
                {
                    // 7 characters sent per sample
                    snprintf(sendBuffer, "%.3d, ", historyBuf[currentPos]);
                    sendto(socketDescriptor,sendBuffer, strnlen(sendBuffer,MAX_LEN),0,(struct sockaddr *) &sock, sock_sz);
                    memset(sendBuffer, 0, MAX_LEN);
                    currentPos += 1;
                }
            }
            free(historyBuf);
        }

        else if (strcmp(recvBuffer, CMD_DIPS) == 0)
        {
            sprintf(sendBuffer, "# Dips = %i.\n", Sampler_analyzeDips());
            sendto(socketDescriptor,sendBuffer, strnlen(sendBuffer,MAX_LEN),0,(struct sockaddr *) &sock, sock_sz);
        }
        else if (strcmp(recvBuffer, CMD_STOP) == 0)
        {
            sprintf(sendBuffer, "Program terminating: (enter to quit)\n");
            sendto(socketDescriptor,sendBuffer, strnlen(sendBuffer,MAX_LEN),0,(struct sockaddr *) &sock, sock_sz);
                //these calls will shut down all the threads
            Sampler_quit();
            Terminal_quit();
            isConnected = false;
        }
        else
        {
            sprintf(sendBuffer, "Unknown command.\n");
			sendto(socketDescriptor,sendBuffer, strnlen(sendBuffer,MAX_LEN),0,(struct sockaddr *) &sock, sock_sz);
        }
    }
    close(socketDescriptor);
    return 0;
}

