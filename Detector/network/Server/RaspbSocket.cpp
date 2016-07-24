/*
 * network.cpp
 *
 *  Created on: 2016. 7. 20.
 *      Author: cmk
 */

#include "RaspbSocket.h"
#include <iostream>

using namespace std;


void RaspbSocket::InitServer(){

	DoListen(AF_INET, SOCK_STREAM, 0, "INADDR_ANY", 8000, 1024);

	//DoListen(1,AF_INET, SOCK_DGRAM, 0, "INADDR_ANY", 9781, 1024);
}

void RaspbSocket::ConnectToDevices(){

	//DoConnect(AF_INET, SOCK_STREAM ,0 ,"127.0.0.1", 8000 , 6, 10000 );
	//DoConnect(AF_INET, SOCK_STREAM ,0 ,"127.0.0.1", 8101 , 6, 10000 );

}


void RaspbSocket::OnConnected(int idx){

	cout << "CON!!";


}

void RaspbSocket::OnRead(int idx){




}


void RaspbSocket::OnWait(int idx){


}

void RaspbSocket::OnClose(int idx){


}


void RaspbSocket::OnWrite(int idx){


}


void RaspbSocket::OnError(int id, char *pMsg){


}


void *RaspbSocket::CreateThread(){

	while(1){
		AsyncSock::AsyncNotificationLoop(false);

	}



}
