/*
 * CommunicationCommand.h
 *
 *  Created on: 2009-12-28
 *      Author: mxx
 */

#ifndef COMMUNICATIONCOMMAND_H_
#define COMMUNICATIONCOMMAND_H_
#include "Packet.h"
#include <string>
namespace bitcomm
{


class CmdPacket:public virtual Packet
{
public:
	CmdPacket();
	virtual ~CmdPacket();

	void SetCommand(const char* szCmd,unsigned char Machine);
	enum CommunicationCommand CommandType();
	static bool IsCommand(const char* szCmd,enum CommunicationCommand n)
	{
		if (n>CMD_END) return false;
		if (szCmd==NULL) return false;
		return szCmd[0]==cmdWord[n][0] && szCmd[1]==cmdWord[n][1];
	}
};

}

#endif /* COMMUNICATIONCOMMAND_H_ */
