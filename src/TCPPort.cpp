/*
 * TCPPort.cpp
 *
 *  Created on: 2009-12-30
 *      Author: mxx
 */

#include "TCPPort.h"
namespace bitcomm
{
TCPPort::TCPPort(int nInitPort)
{
	this->nInitPort = nInitPort;
	nPort = 0;
}

TCPPort::~TCPPort()
{

}

int TCPPort::Open(const char* szServer)
{

	return 0;
}

int TCPPort::Read(const char* buf,int len)
{
	return 0;
}

int TCPPort::Write(const char* buf,int len)
{
	return 0;
}
void  TCPPort::Lock(void)
{

}
void  TCPPort::Unlock(void)
{

}

}
