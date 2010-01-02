/*
 * PacketQueue.h
 *
 *  Created on: 2009-12-31
 *      Author: mxx
 */

#ifndef PACKETQUEUE_H_
#define PACKETQUEUE_H_
#include <list>
#include "Packet.h"
namespace bitcomm
{


struct DataPacketFrame
{
	unsigned char SOH;
	unsigned char Machine;
	unsigned char STX;
	unsigned char cmd[2];
	unsigned short length;
	unsigned short dataNo;
	unsigned char dummy[65];
};

class DataPacketQueue
{
public:
	DataPacketQueue();
	virtual ~DataPacketQueue();
	void Push(Packet& data);
	void Pop(void);
	int GetSize(void)
	{
		return queue.size();
	};
	struct DataPacketFrame& Front(void);

protected:
	list<struct DataPacketFrame> queue;
};

}

#endif /* PACKETQUEUE_H_ */
