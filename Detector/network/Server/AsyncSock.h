////////////////////////////////////////////////////////////////////////////////
//
// "ASyncSock"
//                            Asynchrous Socket Handler class for Linux systems.
//
// =============================================================================
//
//                                        (C)Copyright 2011 , Rageworx software.
//                                                            rageworx@gmail.com
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __ASYNCSOCK_H__
#define __ASYNCSOCK_H__

#include <pthread.h>

#include <signal.h>
#include <cerrno>
#include <sys/types.h>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "SocketWrap.h"
#include <vector>
#include <map>


using namespace std;

template <typename _T> class locker : public _T
{
    pthread_mutex_t mutex[1];

    public:	inline locker()
    {
#ifdef __CYGWIN__
        pthread_mutexattr_t attr;
        pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_ERRORCHECK);
        pthread_mutex_init(mutex, &attr);
#else
        pthread_mutexattr_t attr;
        pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_ERRORCHECK_NP);
        pthread_mutex_init(mutex, &attr);
#endif
    }

    inline ~locker()
    {
        if (pthread_mutex_destroy(mutex) == EBUSY)
        {
            Unlock();
            pthread_mutex_destroy(mutex);
        }
    }

    inline void Lock()
    {
        pthread_mutex_lock(mutex);
    }

    inline void Unlock()
    {
        pthread_mutex_unlock(mutex);
    }
};

struct SockInfo
{
    int     status;
    time_t  conTime;
    int     timeout;
};

class AsyncSock
{
    private:
        vector<int>         fds;
        vector<SockInfo>    sockInfo;
        int                 maxi;
        int                 maxfd;
        int                 connections;
        fd_set              read_fds;
        fd_set              write_fds;
        bool                loopAlive;
        struct
        timeval             timeVal;

        //don't used by caller.
        // please inherits this class instead new.
        AsyncSock() {};

    public:
        AsyncSock(int maxNum);
        virtual ~AsyncSock() {};

    public:
        locker < vector< SocketWrap > >  SockList;

    public:
        int          Close(int idx);
        void         InitAsync();
        int          DoListen(int domain, int type, int protocol, const char* ip, int port, int backlog);
        int          DoConnect(int domain, int type, int protocol, const char* ip, int port, int retry, int to);
        void         AsyncNotificationLoop(bool);

    public:
        unsigned int GetAvailBytes(int idx);
        int          GetConnectionCount(void);
        int          GetSockStatus(int idx) ;
        void         IncreaseSock(int idx, bool increase);
        void         Select(timeval *timeout, bool noWrites);
        void         SendStopAsyncNotiLoop();

    public:
        virtual void OnConnected(int idx) {};
        virtual void OnRead(int idx) {};
        virtual void OnWrite(int idx) {};
        virtual void OnWait(int idx) {};
        virtual void OnClose(int idx) {};
        virtual void OnError(int id, char *pMsg) {};
};


#endif // of __ASYNCSOCK_H__
