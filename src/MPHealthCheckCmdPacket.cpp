/*
 * MPHealthCheckCmdPacket.cpp
 *
 *  Created on: 2010-1-4
 *      Author: mxx
 */

#include "MPHealthCheckCmdPacket.h"
#include "CommunicationCommand.h"
#include <arpa/inet.h>
namespace bitcomm
{

MPHealthCheckCmdPacket::MPHealthCheckCmdPacket(unsigned int nStatus,unsigned char Machine,bool bIntErr,bool bExtErr)
{
	struct MPHealthCheckCmdContent content;
	content.cmd[0]=cmdWord[MPHealthCheck][0];
	content.cmd[1]=cmdWord[MPHealthCheck][1];
	content.length = 9;

	if (bIntErr) nStatus |= INT_COMM_ERR;
	else nStatus &= ~INT_COMM_ERR;
	if (bExtErr)nStatus |= EXT_COMM_ERR;
	else nStatus &= ~EXT_COMM_ERR;

	content.nStatus = htonl(nStatus);
	BuildPacket((const char*)&content,sizeof(content),Machine);
}

MPHealthCheckCmdPacket::~MPHealthCheckCmdPacket()
{

}

}
