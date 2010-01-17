/*
 * MPHealthCheckCmdPacket.cpp
 *
 *  Created on: 2010-1-4
 *      Author: mxx
 */

#include "MPHealthCheckCmdPacket.h"
#include "CommunicationCommand.h"
namespace bitcomm
{

MPHealthCheckCmdPacket::MPHealthCheckCmdPacket(unsigned int nStatus,unsigned char Machine,bool bIntErr,bool bExtErr)
{
	struct MPHealthCheckCmdContent content;
	content.cmd[0]=cmdWord[MPHealthCheck][0];
	content.cmd[2]=cmdWord[MPHealthCheck][1];
	content.length = 9;
	content.nStatus = nStatus;
	if (bIntErr) content.nStatus |= INT_COMM_ERR;
	else content.nStatus &= ~INT_COMM_ERR;
	if (bExtErr)content.nStatus |= EXT_COMM_ERR;
	else content.nStatus &= ~EXT_COMM_ERR;
	BuildPacket((const char*)&content,sizeof(content),Machine);
}

MPHealthCheckCmdPacket::~MPHealthCheckCmdPacket()
{

}

}
