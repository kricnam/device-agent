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
#include <string>
namespace bitcomm
{

class Protocol
{
public:
	Protocol();
	virtual ~Protocol();
	void RequestCurrentData(Channel& port,Packet& data);
	void SendCurrentData(Channel& port, DataPacketQueue& queue);
	void NegoiateDataChannel(TCPPort& port);
	void NegoiateControlChannel(TCPPort& port);
	int Sleep();
	void HealthCheck(Channel& dev,Channel& port,Packet& status);
protected:
	int negoiateChannel(TCPPort& port,int nStartPort);
	bool bExtCommunicationError;
	bool bInCommunicationError;
	unsigned char Machine;
	string strServerName;
	unsigned int nLastStatus;
};

}

#endif /* PROTOCOL_H_ */
