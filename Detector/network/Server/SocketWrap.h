////////////////////////////////////////////////////////////////////////////////
//
// "SocketWrap"
//                                       Socket Handler class for Linux systems.
//
// =============================================================================
//
//                                        (C)Copyright 2011 , Rageworx software.
//                                                            rageworx@gmail.com
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __SOCKETWRAP_H__
#define __SOCKETWRAP_H__

#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

#define MAX_ERROR_STRING_LEN            256
#define SERVER_SOCKET_INDEX             0

class SocketWrap
{
    public:
        SocketWrap();
        SocketWrap(int family,int type,int protocol);
        ~SocketWrap();

    public:
        int     Send(char * buf,int length,int flag = 0);
        int     SendTo(char * buf,int length,sockaddr* addr,socklen_t len);
        int     Recv(char * buf,int length,int flag = 0);
        int     RecvFrom(char * buf,int length,sockaddr* addr,socklen_t* len);
        int     Bind(void);
        int     Connect(int retry);
        int     Listen(int backlog = 5);
        int     Accept(int srvfd);
        int     Close();

    public:
        int     SetToBroadcast();
        int     SetSockOpt(int level, int optname, const void *optval, socklen_t optlen);
        int     SetNonBlockSock();
        int     SetAddr(const char* ip, int port);
        int     SetSvrAddr(const char* ip, int port);
        int     SetSocket(int family, int type, int protocol);
        int     SetSockfd(int fd);
        int     Getfd();

    public:
        int     GetLastError();
        char*   GetLastErrorMsg();

    protected:
        char            strLastError[MAX_ERROR_STRING_LEN];
        int             lastError;

    private:
        int             sockfd;
        short           port;
        unsigned int    ip;
        int             family;
        int             type;                         //SOCK_STREAM , SOCK_DGRAM
        int             flag;
        sockaddr_in     addr;                         // address of self.
        sockaddr_in     remote_addr;                  // remote address.

        static
        socklen_t       addrlen;
        static
        socklen_t       clientlen;

        bool            isNonBlock;
};

#endif // of __SOCKETWRAP_H__
