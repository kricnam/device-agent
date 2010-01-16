/*
 * PacketQueue.cpp
 *
 *  Created on: 2009-12-31
 *      Author: mxx
 */

#include "PacketQueue.h"
#include "DebugLog.h"
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
	TRACE("data[%d]",data.GetSize());
	if (data.GetSize())
	{
		list<Packet>::iterator it;
		for(it = queue.begin();it!=queue.end();it++)
		{
			if ((*it).GetDataNo() == data.GetDataNo())
				return;
		}
		queue.push_back(data);
	}
}

Packet& DataPacketQueue::Front(void)
{
	return queue.front();
}
Packet& DataPacketQueue::GetAt(int i)
{
	list<Packet>::iterator it;
	int count = 0;
	for(it = queue.begin();it!=queue.end();it++)
	{
		if (count++== i) break;
	}
	return (*it);
}

void DataPacketQueue::Pop(void)
{
	queue.pop_front();
}

}
