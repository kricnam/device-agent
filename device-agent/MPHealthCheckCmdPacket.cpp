/*
 * MPHealthCheckCmdPacket.cpp
 *
 *  Created on: 2010-1-4
 *      Author: mxx
 */

#include "MPHealthCheckCmdPacket.h"
#include "CommunicationCommand.h"
#include "DebugLog.h"
#include <arpa/inet.h>
namespace bitcomm
{

MPHealthCheckCmdPacket::MPHealthCheckCmdPacket(unsigned int nStatus,unsigned char Machine,bool bIntErr,bool bExtErr)
{
	struct MPHealthCheckCmdContent content;
	content.cmd[0]=cmdWord[MPHealthCheck][0];
	content.cmd[1]=cmdWord[MPHealthCheck][1];
	content.length = 9;

	TRACE("Status : %08X",nStatus);
	if (bIntErr)
	{
		TRACE("Internal Comm Err");
		nStatus |= INT_COMM_ERR;
	}
	else nStatus &= ~INT_COMM_ERR;

	if (bExtErr)
	{
		TRACE("External Comm Err");
		nStatus |= EXT_COMM_ERR;
	}
	else nStatus &= ~EXT_COMM_ERR;

	TRACE("Status new: %08X",nStatus);

	content.nStatus = htonl(nStatus);
	BuildPacket((const char*)&content,sizeof(content),Machine);
}

MPHealthCheckCmdPacket::~MPHealthCheckCmdPacket()
{

}

}
