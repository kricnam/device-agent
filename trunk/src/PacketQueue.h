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
#include <pthread.h>
namespace bitcomm
{

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
	Packet& Front(void);
	Packet& GetAt(int i);
	void Save(const char* szFile);
	void Load(const char* szFile);

protected:
	list<Packet> queue;
	pthread_mutex_t mutex;
};

}

#endif /* PACKETQUEUE_H_ */
