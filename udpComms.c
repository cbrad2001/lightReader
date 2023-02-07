#include "include/udpComms.h"
#include "include/sampler.h"
#include "include/terminal.h"
#include "include/analogDisplay.h"

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

//run: netcat -u 192.168.7.2 12345              // temp: port 12334

#define PORT 12334                              // NOTE: edited temporarily
#define MAX_LEN 1024
#define INVALID_VAL DBL_MAX
    //all commands:
#define CMD_HELP    "help\n"
#define CMD_COUNT   "count\n"
#define CMD_LENGTH  "length\n"
#define CMD_HISTORY "history\n"     
#define CMD_GET_N   "get "        
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
        if (bytesRx == -1 )                             // create recipient buffer, check if connected
        {
            printf("Can't receive data \n");
            exit(1);
        } 

        if (recvBuffer[0] == ENTER)                       
        {
            strncpy(recvBuffer, cmdHistory, MAX_LEN);   // call prev command if 'enter'
        }
        strncpy(cmdHistory,recvBuffer,MAX_LEN);         //store history for enter command

// strncmp value size is the length of each command keyword in all cases
        if (strncmp(recvBuffer, CMD_HELP,4)==0)         // String data appended as a helpful output message
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
        else if(strncmp(recvBuffer, CMD_COUNT,5) == 0)  //reports all of the samples taken in history
        {
			sprintf(sendBuffer, "Number of samples taken = %lld. \n",Sampler_getNumSamplesTaken());
			sendto(socketDescriptor,sendBuffer, strnlen(sendBuffer,MAX_LEN),0,(struct sockaddr *) &sock, sock_sz);
		}
        else if(strncmp(recvBuffer, CMD_LENGTH,6) == 0) //reports the current buffer size (and how full it is)
        {
			sprintf(sendBuffer, "History can hold %d samples. \nCurrently holding %d samples.\n",
                Sampler_getNumSamplesInHistory(),Sampler_getHistorySize());
			sendto(socketDescriptor,sendBuffer, strnlen(sendBuffer,MAX_LEN),0,(struct sockaddr *) &sock, sock_sz);
		}
        else if(strncmp(recvBuffer, CMD_HISTORY,7) == 0)//reports all light samples currently in the buffer
        {
            int historySize = Sampler_getHistorySize();
            double *historyBuf = Sampler_getHistoryInOrder(historySize);
            int historyBufSize = Sampler_getNumSamplesInHistory();
            memset(sendBuffer, 0, MAX_LEN);      //1024 bytes per buffer

            for (int i = 0; i < historyBufSize; i++)    //print all elements
            {
                double val = historyBuf[i];
                if (val != INVALID_VAL)                 
                {
                    sprintf(sendBuffer, "%.3f, ", val); //send all valid elements of history
                    if ((i+1) % 20 == 0)                //newline every 20th index for visual clarity
                    {
                        strcat(sendBuffer, "\n");  
                    }
                sendto(socketDescriptor,sendBuffer, strnlen(sendBuffer,MAX_LEN),0,(struct sockaddr *) &sock, sock_sz);
                memset(sendBuffer, 0, MAX_LEN);
                }
            }
            free(historyBuf);
            char* newln = "\n";                         //send an extra newline for spacing
            sendto(socketDescriptor,newln, strnlen(newln,MAX_LEN),0,(struct sockaddr *) &sock, sock_sz);
        }
        else if (strncmp(recvBuffer, CMD_DIPS,4) == 0)  //reports the active number of light dips in the buffer
        {
            sprintf(sendBuffer, "# Dips = %i.\n", Sampler_analyzeDips());
            sendto(socketDescriptor,sendBuffer, strnlen(sendBuffer,MAX_LEN),0,(struct sockaddr *) &sock, sock_sz);
        }
        else if (strncmp(recvBuffer, CMD_STOP,4) == 0)  // shuts off all threads; quits local udp and remote sampler
        {
            sprintf(sendBuffer, "Program terminating: (enter to quit)\n");
            sendto(socketDescriptor,sendBuffer, strnlen(sendBuffer,MAX_LEN),0,(struct sockaddr *) &sock, sock_sz);
            Sampler_quit();
            Terminal_quit();                            //these calls will shut down all the threads
            Analog_quit();
            isConnected = false;
        }
        else if (strncmp(recvBuffer, CMD_GET_N, 4) == 0)//reports the N most recent samples
        {
            char *startOfN = recvBuffer + 4;            // 4th position is the start of n
            int n = atoi(startOfN);
            double *historyBuf = Sampler_getHistoryInOrder(n);
            int historyBufSize = Sampler_getNumSamplesInHistory();
            memset(sendBuffer, 0, MAX_LEN);             // 1024 bytes per buffer

            if (n > historyBufSize)                     // edge case handling: n too big
            {
                char *errMsg = "The value given is greater than the number of valid samples in the history. Please set it to under ";
                sprintf(sendBuffer, "%s %d.\n", errMsg, historyBufSize);
                sendto(socketDescriptor,sendBuffer, strnlen(sendBuffer,MAX_LEN),0,(struct sockaddr *) &sock, sock_sz);
            }
            else if (n < 0)                             // edge case handling: n too small
            {
                char *errMsg = "Please give a valid non-negative number.";
                sprintf(sendBuffer, "%s\n", errMsg);
                sendto(socketDescriptor,sendBuffer, strnlen(sendBuffer,MAX_LEN),0,(struct sockaddr *) &sock, sock_sz);
            }
            else {
                int currentPos = 0;
                while (currentPos < n)                  // 7 characters sent per sample
                {
                    sprintf(sendBuffer, "%.3f, ", historyBuf[currentPos]);
                    if ((currentPos+1) % 20 == 0)       // send 20 per line
                    {
                        strcat(sendBuffer, "\n");
                    }
                    sendto(socketDescriptor,sendBuffer, strnlen(sendBuffer,MAX_LEN),0,(struct sockaddr *) &sock, sock_sz);
                    memset(sendBuffer, 0, MAX_LEN);
                    currentPos += 1;
                }
            }
            char* newln = "\n";
            sendto(socketDescriptor,newln, strnlen(newln,MAX_LEN),0,(struct sockaddr *) &sock, sock_sz);
            free(historyBuf);
        }
        else                                            // default case: unknown
        {
            sprintf(sendBuffer, "Unknown command.\n");
			sendto(socketDescriptor,sendBuffer, strnlen(sendBuffer,MAX_LEN),0,(struct sockaddr *) &sock, sock_sz);
        }
        memset(recvBuffer, 0, MAX_LEN);
        memset(sendBuffer, 0, MAX_LEN);
    }
    close(socketDescriptor);
    return 0;
}

