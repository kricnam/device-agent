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
	// TODO Auto-generated constructor stub

}

CmdPacket::~CmdPacket()
{
	// TODO Auto-generated destructor stub
}

void CmdPacket::SetCommand(const char* szCmd,unsigned char Machine)
{
	buildPacket(szCmd,2,Machine);
}

}
