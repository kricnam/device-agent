/*
 * PacketQueue.cpp
 *
 *  Created on: 2009-12-31
 *      Author: mxx
 */

#include "PacketQueue.h"

namespace bitcomm
{

DataPacketQueue::DataPacketQueue()
{

}

DataPacketQueue::~DataPacketQueue()
{

}

void DataPacketQueue::Push(Packet& data)
{
	list<Packet>::iterator it;

	for(it = queue.begin();it!=queue.end();it++)
	{
		if ((*it).GetDataNo() == data.GetDataNo())
			return;
	}

	queue.push_back(data);
}

Packet& DataPacketQueue::Front(void)
{
	return queue.front();
}

void DataPacketQueue::Pop(void)
{
	queue.pop_front();
}

}