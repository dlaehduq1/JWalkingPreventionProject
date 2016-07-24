/*
 * network.h
 *
 *  Created on: 2016. 7. 20.
 *      Author: cmk
 */

#pragma once

#include "AsyncSock.h"
#include <pthread.h>

class RaspbSocket : public AsyncSock
{
    public:
        char buff[1024];
        bool isConnecteToRaspb;
        bool isConnectToMonitor;
        pthread_t th;

    public:
        RaspbSocket(int sockNum) : AsyncSock(sockNum) {
        	isConnecteToRaspb = false;
        	isConnectToMonitor = false;
        	if(pthread_create(&th , NULL, &RaspbSocket::ThreadHelper ,  this) < 0)
        		pthread_join(th,NULL);
        	}

    public:
        void InitServer();
        void ConnectToDevices();
        void *CreateThread();
        static void *ThreadHelper(void *con){

			return 	((RaspbSocket*)con)->CreateThread();
              	     }

        virtual void OnConnected(int idx);
        virtual void OnRead(int idx);
        virtual void OnWait(int idx);
        virtual void OnClose(int idx);
        virtual void OnWrite(int idx);
        virtual void OnError(int id, char *pMsg);
};
