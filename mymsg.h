#ifndef __MYMSG_H__
#define __MYMSG_H__

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#define NUM 32
#define SPACE "------------------------"
#define dataLen 1024

char buf[dataLen];

typedef struct Msg{
    int command;
    char sendto[12];
    char me[12];
    unsigned sendtolen;
    unsigned melen;
    char pwd[12];
    unsigned pwdlen;
    char mess[128];
    unsigned messlen;
    long groupid;
    long fileSize;
    char fileName[20];
    char buf[dataLen];
} msg;

#endif
