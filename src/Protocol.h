/*
 * Protocol.h
 *
 *  Created on: 2009-12-30
 *      Author: mxx
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_
#include "PacketQueue.h"
#include "Packet.h"
#include "TCPPort.h"
namespace bitcomm
{

class Protocol
{
public:
	Protocol();
	virtual ~Protocol();
	void RequestCurrentData(unsigned char Machine,Channel& port,Packet& data);
	void SendCurrentData(Channel& port, DataPacketQueue& queue);
	void NegoiateDataChannel(TCPPort& port);
	void NegoiateControlChannel(TCPPort& port);
	int Sleep();
	void HealthCheck(unsigned char Machine,Channel& dev,Channel& port,Packet& status);
protected:
	int negoiateChannel(TCPPort& port,int nStartPort);
};

}

#endif /* PROTOCOL_H_ */
