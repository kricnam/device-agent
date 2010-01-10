/*
 * CommunicationCommand.cpp
 *
 *  Created on: 2009-12-28
 *      Author: mxx
 */

#include "CommunicationCommand.h"

namespace bitcomm
{




CmdPacket::CmdPacket()
{

}

CmdPacket::~CmdPacket()
{

}

void CmdPacket::SetCommand(const char* szCmd,unsigned char Machine)
{
	buildPacket(szCmd,2,Machine);
}

enum CommunicationCommand CmdPacket::CommandType()
{
	int eCmd;
	const char *pCmd = strCache.data()+3;
	for (eCmd=0;eCmd<CMD_END;eCmd++)
	{
		if (pCmd[0]==cmdWord[eCmd][0]&&	pCmd[1]==cmdWord[eCmd][1])
			return (enum CommunicationCommand)eCmd;
	}
	return CMD_END;
}

}
