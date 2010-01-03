/*
 * CommunicationCommand.h
 *
 *  Created on: 2009-12-28
 *      Author: mxx
 */

#ifndef COMMUNICATIONCOMMAND_H_
#define COMMUNICATIONCOMMAND_H_
#include "Packet.h"
namespace bitcomm
{

enum CommunicationCommand
{
	DataRequest,
	CMD_END
};

static const char* cmdWord[]={"ra",""};

class CmdPacket:public virtual Packet
{
public:
	CmdPacket();
	virtual ~CmdPacket();

	void SetCommand(const char* szCmd,unsigned char Machine);

};

}

#endif /* COMMUNICATIONCOMMAND_H_ */
