
#include <cstdio>
#include <cstdlib>
#include <cerrno>

#include <sys/ioctl.h>

#include "AsyncSock.h"

#define SLEEP_TIME                      1000000
#define DEFAULT_TIMEOUT_SEC             5
#define DEFAULT_TIMEOUT_USEC            10


using namespace std;

AsyncSock::AsyncSock(int maxNum)
{
    int cnt;

    SockList.reserve(maxNum);
    sockInfo.reserve(maxNum);

    for ( cnt = 0; cnt < maxNum; cnt++ )
    {
        SocketWrap temp;
        temp.SetNonBlockSock();
        SockList.push_back(temp);
    }

    maxi        = -1;
    maxfd       = maxNum;
    connections = 0;
}

void AsyncSock::InitAsync()
{
}

int AsyncSock::Close(int idx)
{
    SockList[idx].Close();
    IncreaseSock(idx, false);
    OnClose(idx);

    return 0;
}

void AsyncSock::IncreaseSock(int idx, bool increase)
{
    if ( increase == true )
    {
        sockInfo[idx].status = 1;

        connections++;

        if ( idx > maxi )
        {
            maxi = idx;
        }
    }
    else
    {
        sockInfo[idx].status = -1;

        connections--;

        if ( idx == maxi )
        {
            int cnt;

            for ( cnt=maxi-1; cnt>-1; cnt-- )
            {
                if ( SockList[cnt].Getfd() < 0 )
                    continue;

                break;
            }

            maxi = cnt;
        }
    }
}

int AsyncSock::DoListen(int domain, int type, int protocol, const char* ip, int port, int backlog)
{
    if ( SockList[SERVER_SOCKET_INDEX].SetSocket(domain, type, protocol) < 0 )
    {
        OnError(SockList[SERVER_SOCKET_INDEX].GetLastError(),
                SockList[SERVER_SOCKET_INDEX].GetLastErrorMsg());
        return -1;
    }

    SockList[SERVER_SOCKET_INDEX].SetSvrAddr(ip, port);

    if ( ( SockList[SERVER_SOCKET_INDEX].Bind() < 0 )    ||
         ( SockList[SERVER_SOCKET_INDEX].Listen(backlog ) < 0 ) )
    {
        SockList[SERVER_SOCKET_INDEX].Close();
        OnError(SockList[SERVER_SOCKET_INDEX].GetLastError(),
                SockList[SERVER_SOCKET_INDEX].GetLastErrorMsg());
        return -1;
    }
    char port_str[6];
    snprintf(port_str, sizeof(port_str), "%d", port);


    printf("\nIP : %s PORT : %s로 소켓이 생성되었습니다", ip, port_str);

    IncreaseSock(0, true);

    return 0;
}

int AsyncSock::GetConnectionCount(void)
{
    return connections;
}

int AsyncSock::GetSockStatus(int idx)
{
    return sockInfo[idx].status;
}

int AsyncSock::DoConnect(int domain, int type, int protocol, const char* ip, int port, int retry, int to)
{
    int cnt;

    for ( cnt=1; cnt<maxfd; cnt++ )
    {
        if ( SockList[cnt].Getfd() < 0 )
        {
            if ( SockList[cnt].SetSocket(domain, type, protocol) < 0 )
            {
                OnError(SockList[cnt].GetLastError(),
                        SockList[cnt].GetLastErrorMsg());

                return -1;
            }

            SockList[cnt].SetAddr(ip, port);

            switch ( SockList[cnt].Connect(retry) )
            {
                case 0: // on while connect ...
                    IncreaseSock(cnt, true);
                    sockInfo[cnt].status  = 0;
                    sockInfo[cnt].conTime = time(NULL);
                    sockInfo[cnt].timeout = to;
                    return cnt;

                case 1: // already connectdd
                    IncreaseSock(cnt, true);
                    return cnt;

                case -1: // failed while retry count
                    SockList[cnt].Close();
                    return -1 ;
            }
        }
    }

    return -1;
}

void AsyncSock::AsyncNotificationLoop(bool noWrites)
{

    if( loopAlive != true )
    {
        loopAlive = true;
    }

#ifdef _DEBUG
    printf(" .. entering AsyncNotificationLoop() \n");
#endif

    timeVal.tv_sec  = DEFAULT_TIMEOUT_SEC;
    timeVal.tv_usec = DEFAULT_TIMEOUT_USEC;

    while ( loopAlive == true )
    {
        try
        {
            SockList.Lock();
            Select(&timeVal, noWrites);
            SockList.Unlock();
        }
        catch(...)
        {
            // if on exception, process error exceptions.
            OnError(0,"AsyncLoop exception occured!");
        }

        usleep(SLEEP_TIME);
    }

#ifdef _DEBUG
    printf(" .. going out of AsyncNotificationLoop() \n");
#endif
}

void AsyncSock::Select(timeval *timeout, bool noWrites)
{
    int numFds  = SockList.size();
    int max_fd  = 0;
    int res     = 0;

    if ( numFds > 0 )
    {
        FD_ZERO(&read_fds);
        FD_ZERO(&write_fds);

        for ( int cnt=0; cnt<numFds; cnt++ )
        {
            int fd = SockList[cnt].Getfd();

            if ( fd > max_fd )
            {
                max_fd = fd;
            }

            if ( fd < 0 )
                continue;

            if ( cnt == 0 ) // it is server socket...
            {
                FD_SET(fd, &read_fds);
                continue;
            }

            FD_SET(fd, &read_fds);

        }

        if ( noWrites == true )
        {
            FD_ZERO(&write_fds);
        }

        res = select(max_fd+1, &read_fds, &write_fds, NULL, timeout);
        if ( res < 0 )
        {
            if ( errno == EINTR )
                return;

            abort();
        }

        //timeout
        if ( res == 0 )
            return;

        if ( res > 0 )
        {
            int cnt;

            for ( cnt=0; cnt<numFds; cnt++ )
            {
                if ( SockList[cnt].Getfd() < 0 )
                    continue;

                if ( !FD_ISSET(SockList[cnt].Getfd(), &read_fds) &&
                     !FD_ISSET(SockList[cnt].Getfd(), &write_fds) )
                    continue;

                bool invalidated = false;

                if ( FD_ISSET(SockList[cnt].Getfd(), &read_fds) )
                {
                    int nRead = GetAvailBytes(cnt);

                    if ( nRead > 0 )
                    {
                        if ( cnt == 0 ) // it will server socket.
                        {
                            for ( int cnt2=1; cnt2<numFds; cnt2++ )
                            {
                                if ( SockList[cnt2].Getfd() == -1 )
                                {
                                    SockList[cnt2].Accept(SockList[SERVER_SOCKET_INDEX].Getfd());
                                    IncreaseSock(cnt2,true);
                                    break;
                                }
                            }
                         }
                         else
                         {
                             OnRead(cnt);
                         }
                    }
                    else
                    if( cnt == 0 ) // Just connected client.
                    {
                        for ( int cnt2=1; cnt2<numFds; cnt2++ )
                        {
                            if( SockList[cnt2].Getfd() == -1 )
                            {
                                SockList[cnt2].Accept(SockList[SERVER_SOCKET_INDEX].Getfd());
                                IncreaseSock(cnt2, true);
                                OnConnected(cnt2);

                                break;
                            }
                        }

                    }
                    else
                    {
                        invalidated = true;
                        OnWait(cnt);
                    }
                }

                if ( !invalidated && FD_ISSET(SockList[cnt].Getfd(), &write_fds) )
                {
                    OnWrite(cnt);
                }
            }
        }
    }

}

unsigned int AsyncSock::GetAvailBytes(int idx)
{
    unsigned int    nRead;

    if ( ioctl(SockList[idx].Getfd(), FIONREAD, &nRead) > 0 )
    {
        return nRead;
    }

    return 0;
}

void AsyncSock::SendStopAsyncNotiLoop()
{
    if( loopAlive == true )
        loopAlive = false;
}
