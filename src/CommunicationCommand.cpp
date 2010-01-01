/*
 * CommunicationCommand.cpp
 *
 *  Created on: 2009-12-28
 *      Author: mxx
 */

#include "CommunicationCommand.h"

namespace bitcomm
{

CommunicationCommand::CommunicationCommand()
{
	// TODO Auto-generated constructor stub

}

CommunicationCommand::~CommunicationCommand()
{
	// TODO Auto-generated destructor stub
}

void CommunicationCommand::setCommand(const char* szCmd,unsigned char Machine)
{
	buildPacket(szCmd,2,Machine);
}

}
