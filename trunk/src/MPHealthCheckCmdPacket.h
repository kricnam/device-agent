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

#define INT_COMM_ERR (1<<17)
#define EXT_COMM_ERR (1<<18)

class MPHealthCheckCmdPacket: public virtual bitcomm::Packet
{
public:
	MPHealthCheckCmdPacket(unsigned nState,unsigned char Machine,bool bIntErr,bool bExtErr);
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
