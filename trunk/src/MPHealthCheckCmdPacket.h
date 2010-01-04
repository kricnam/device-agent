/*
 * MPHealthCheckCmdPacket.h
 *
 *  Created on: 2010-1-4
 *      Author: mxx
 */

#ifndef MPHEALTHCHECKCMDPACKET_H_
#define MPHEALTHCHECKCMDPACKET_H_

#include "Packet.h"

namespace bitcomm
{

class MPHealthCheckCmdPacket: public virtual bitcomm::Packet
{
public:
	MPHealthCheckCmdPacket(unsigned nState,unsigned char Machine);
	virtual ~MPHealthCheckCmdPacket();
protected:
	struct MPHealthCheckCmdContent
	{
		char cmd[2];
		unsigned short length;
		unsigned int nStatus;
	} __attribute__ ((packed));
};

}

#endif /* MPHEALTHCHECKCMDPACKET_H_ */
