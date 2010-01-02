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
	list<struct DataPacketFrame>::iterator it;
	struct DataPacketFrame* pData = (struct DataPacketFrame*)data.GetData();
	for(it = queue.begin();it!=queue.end();it++)
	{
		if ((*it).dataNo == pData->dataNo)
			return;
	}

	queue.push_back(*pData);
}

struct DataPacketFrame& DataPacketQueue::Front(void)
{
	return queue.front();
}

void DataPacketQueue::Pop(void)
{
	queue.pop_front();
}

}
