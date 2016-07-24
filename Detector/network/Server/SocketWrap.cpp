#include "SocketWrap.h"
#include <cstdio>
#include <cstdlib>

#define MAX_ADDRESS_LEN                 80

#define DEFAULT_IP                      "127.0.0.1"
#define DEFAULT_PORT                    8000
#define DEFAULT_DATA_SIZE               1024

using namespace std;

socklen_t SocketWrap::addrlen       = sizeof(struct sockaddr_in);
socklen_t SocketWrap::clientlen     = sizeof(struct sockaddr_in);

SocketWrap::SocketWrap()
:   sockfd(-1),
    port(0),
    ip(0),
    family(AF_INET),
    type(SOCK_STREAM),
    flag(0),
    isNonBlock(false)
{
}

SocketWrap::SocketWrap(int family,int type,int protocol)
:   sockfd(-1),
    port(0),
    ip(0),
    family(AF_INET),
    type(SOCK_STREAM),
    flag(0),
    isNonBlock(false)
{
    SetSocket(family,type,protocol);
}

int SocketWrap::SetSocket(int family, int type, int protocol)
{
    this->family = family;
    this->type   = type;
    this->sockfd = socket(this->family,this->type,protocol);

    if ( sockfd < 0 )
    {
        lastError = sockfd;
        sprintf(strLastError, "Socket creating failed.");
        return -1;
    }

    return 0;
}

SocketWrap::~SocketWrap()
{
    if ( sockfd != -1 )
    {
        close(sockfd);
    }
}
int SocketWrap::Send(char* buf,int length,int flag)
{
    if( sockfd < 0 )
    {
        lastError = sockfd;
        sprintf(strLastError, "Socket was not open, yet");
        return -1;
    }

    char*   tmp;
    size_t  leftN;
    ssize_t sendN;

    tmp   = (char*) buf;
    leftN = length;

    while ( leftN > 0 )
    {
        if ( ( sendN = send(sockfd, tmp, leftN, flag) ) <= 0 )
        {
            if(  errno == EWOULDBLOCK )
            {
                continue;
            }

            lastError = errno;
            sprintf(strLastError, "Socket send() failed!");

            return -1;
        }

        leftN -= sendN;
        tmp   += sendN;
    }

    return length;
}

int SocketWrap::SendTo(char* buf,int length,sockaddr* addr,socklen_t len)
{
    if( sockfd < 0 )
    {
        lastError = sockfd;
        sprintf(strLastError, "Socket was not open, yet");
        return -1;
    }

    char*   tmp;
    size_t  leftN;
    ssize_t sendN;

    tmp     = (char*) buf;
    leftN   = length;

    while ( leftN > 0 )
    {
        if ( ( sendN = sendto(sockfd, tmp, leftN,0,(sockaddr*) addr,len) ) <= 0 )
        {
            if(errno == EWOULDBLOCK)
            {
                continue;
            }

            lastError = errno;
            sprintf(strLastError, "Socket sendto failed");

            return -1;
        }

        leftN -= sendN;
        tmp   += sendN;
    }
    return length;
}

int SocketWrap::Recv(char* buf,int length,int flag)
{
    ssize_t len;

    if ( ( len = recv(sockfd, buf, length, flag) ) < 0 )
    {
        lastError = len;
        sprintf(strLastError, "Socket receive failed");
    }

    return len;
}

int SocketWrap::RecvFrom(char* buf,int length,sockaddr* addr,socklen_t* fromlen)
{
    ssize_t len;

    if ( ( len = recvfrom(sockfd, buf,length,0,addr, fromlen ) ) < 0 )
    {
        lastError = len;
        sprintf(strLastError, "Socket receive failed");
    }

    return len;
}

int SocketWrap::Bind(void)
{
    int err;

    if ( bind(sockfd, (struct sockaddr*) &addr, sizeof(addr)) < 0 )
    {
        lastError = errno;
        sprintf(strLastError, "Socket binding failed by : ");

        err = errno;

        switch ( err )
        {
            case EBADF:
                strcat(strLastError, "sockfd is bad.");
                break;

            case EACCES:
                strcat(strLastError, "address is protected. and current user is not superuser account.");
                break;

            case ENOTSOCK:
                strcat(strLastError, "sockfd is not for socket. it is file fd.");
                break;

            case EROFS:
                strcat(strLastError, "socket i-node in read-only file system.");
                break;

            case ENOENT:
                strcat(strLastError, "sockfd is not exists.");
                break;

            default:
                strcat(strLastError, "unknown");
                break;
        }

        return -1;
    }
    return 0;
}

int SocketWrap::Connect(int retry)
{
    if( isNonBlock == false )
    {
        flag = fcntl(sockfd, F_GETFL, 0);
        fcntl(sockfd, F_SETFL, flag | O_NONBLOCK);

        while ( (retry--) > 0 )
        {
            if ( connect(sockfd, (struct sockaddr*)&remote_addr, clientlen) == 0 )
            {
                fcntl(sockfd, F_SETFL, flag);
                return 1;
            }
            else
            if ( errno != EINPROGRESS )
            {
                sprintf(strLastError, "Socket Connection failed");
                usleep(10000);
            }
            else
            {
                return 0;
            }
        }

        sprintf(strLastError, "Socket connection times exceeded");
        return -1;
    }
    else
    {
        while ( (retry--) > 0 )
        {
        			int error = connect(sockfd, (struct sockaddr*)&remote_addr, clientlen);
            if ( error )
            {
                 return 1;
            }
            else
            if ( errno != EINPROGRESS )
            {
                sprintf(strLastError, "Socket Connection failed");
                usleep(10000);
            }
            else
            {
                return 0;
            }
        }
    }

    return 0;
}

int SocketWrap::Listen(int backlog)
{
    if ( listen(sockfd, backlog) < 0 )
    {
        sprintf(strLastError, "Socket listening failed");
        return -1;
    }

    return 0;
}
int SocketWrap::Accept(int srvfd)
{
    sockfd = accept(srvfd, (struct sockaddr*) &remote_addr, &clientlen);
    if ( sockfd < 0 )
    {
        sprintf(strLastError, "Socket acception failed");
        return -1;
    }
    for(int i=0 ; i < 1000 ; i++)
	    printf("\n\naccept!!");
    return 0;
}

int SocketWrap::Close()
{
    if ( close(sockfd) < 0 )
    {
        sprintf(strLastError, "Socket closing failed, but set to fd as closed.");
    }

    sockfd = -1;

    return 0;
}

int SocketWrap::SetNonBlockSock()
{
    flag       = fcntl(sockfd, F_GETFL, 0);
    int nRet   = fcntl(sockfd, F_SETFL, flag | O_NONBLOCK);
    isNonBlock = true;

    return nRet;
}

int SocketWrap::SetToBroadcast()
{
    int one = 1;
    return setsockopt(sockfd, SOL_SOCKET,SO_BROADCAST,(void*)&one,(socklen_t)sizeof(one));
}

int SocketWrap::SetSockOpt(int level, int optname, const void *optval, socklen_t optlen)
{
    return setsockopt(sockfd,level,optname,optval,optlen);
}

int SocketWrap::SetSockfd(int fd)
{
   sockfd = fd;

   return sockfd;
}

int SocketWrap::SetAddr(const char* ip, int port)
{
    memset(&remote_addr, 0, clientlen);

    remote_addr.sin_family = AF_INET;

    if ( !strcmp(ip,"INADDR_ANY") || ( ip == NULL ) )
    {
        // set to SERVER ..
        remote_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else
    {
        inet_pton(AF_INET, ip, &remote_addr.sin_addr);
    }

    remote_addr.sin_port = htons(port);

    return 0;
}

int SocketWrap::SetSvrAddr(const char* ip, int port)
{
    memset(&addr, 0, addrlen);
    addr.sin_family = AF_INET;

    if (!strcmp(ip,"INADDR_ANY") || (ip == NULL))
    {
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else
    {
        inet_pton(AF_INET, ip, &addr.sin_addr);
    }

    addr.sin_port = htons(port);

    return 0;
}


int SocketWrap::Getfd()
{
    return sockfd;
}


int SocketWrap::GetLastError()
{
    return lastError;
}

char* SocketWrap::GetLastErrorMsg()
{
    return strLastError;
}
