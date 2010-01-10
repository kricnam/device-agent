/*
 * TCPPort.cpp
 *
 *  Created on: 2009-12-30
 *      Author: mxx
 */
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/file.h>

#include "TCPPort.h"
using namespace std;
namespace bitcomm
{
TCPPort::TCPPort()
{
	nPort = 0;
	socketID = -1;
	bConnected = false;
	timeout = 10000000;
}

TCPPort::~TCPPort()
{

}

int TCPPort::Open(const char* szServer,int nPort)
{
	if (strServerName!=szServer)
		strServerName = szServer;

	this->nPort = nPort;

	if (socketID < 0)
	{
		socketID = socket(AF_INET,SOCK_STREAM,0);
		if (socketID<0)
		{
			perror("TCPPort::Open");
			return -1;
		}
		SetTimeOut(timeout);
	}
	return Connect();
}

int TCPPort::Connect()
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int s;

    /* Obtain address(es) matching host/port */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;          /* Any protocol */
    char szPort[32];
    sprintf(szPort,"%d",nPort);
    s = getaddrinfo(strServerName.c_str(), szPort,
    		&hints, &result);
    if (s != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        return -1;
    }

    /* getaddrinfo() returns a list of address structures.
       Try each address until we successfully connect(2).
       If socket(2) (or connect(2)) fails, we (close the socket
       and) try the next address. */
    for (rp = result; rp != NULL; rp = rp->ai_next)
    {
    	if (socketID==-1)
    	{
    		socketID = socket(rp->ai_family, rp->ai_socktype,
                     rp->ai_protocol);
    		SetTimeOut(timeout);
    	}

        if (socketID == -1)
            continue;

        if (::connect(socketID, rp->ai_addr, rp->ai_addrlen) != -1)
        {
        	bConnected = true;
            break;                  /* Success */
        }

        close(socketID);
        socketID = -1;
    }

    if (rp == NULL)
    {               /* No address succeeded */
        fprintf(stderr, "Could not connect\n");
        return -1;
    }

    freeaddrinfo(result);           /* No longer needed */
    return -1;
}

void TCPPort::Close()
{
	if (socketID>0)
	{
		shutdown(socketID,SHUT_RDWR);
		close(socketID);
		socketID = -1;
		bConnected = false;
	}
}

int TCPPort::Read(char* buf,int len) throw(ChannelException)
{
	if (!bConnected) throw ChannelException();
	int n = recv(socketID,buf,len,0);
	if (n<0)
	{
		int err = errno;
		perror("TCPPort::Read");
		ChannelException excp(err);
		throw excp;
	}
	return n;
}

int TCPPort::Write(const char* buf,int len) throw(ChannelException)
{
	if (!bConnected) throw ChannelException();
	int n = send(socketID,buf,len,0);
	if (n<0)
	{
		int err = errno;
		perror("TCPPort::Write");
		ChannelException excp(err);
		throw excp;
	}
	return n;
}

void TCPPort::SetTimeOut(int uSecond)
{
	timeout=uSecond;

	if (socketID>0)
	{
		struct timeval tm;
		tm.tv_sec = uSecond/1000000;
		tm.tv_usec = uSecond%1000000;

		setsockopt(socketID,SOL_SOCKET,SO_RCVTIMEO,
			(char *)&tm,sizeof(struct timeval));
		setsockopt(socketID,SOL_SOCKET,SO_SNDTIMEO,
			(char *)&tm,sizeof(struct timeval));
	}
}

void  TCPPort::Lock(void)
{

}
void  TCPPort::Unlock(void)
{

}

}
