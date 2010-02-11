/*
 * PacketQueue.cpp
 *
 *  Created on: 2009-12-31
 *      Author: mxx
 */

#include "PacketQueue.h"
#include "DebugLog.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
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

void DataPacketQueue::Save(const char* szFile)
{
	INFO("Save to %s",szFile);
	int fd = open(szFile,O_CREAT|O_WRONLY|O_TRUNC);
	if (fd < 0)
	{
		ERRTRACE();
		return;
	}
	list<Packet>::iterator it;
	for(it = queue.begin();it!=queue.end();it++)
	{
		int n = (*it).GetSize();
		if (n)
		{
			if (write(fd,&n,sizeof(n))!=sizeof(n))
			{
				ERRTRACE();
				break;
			}
			if (n!=write(fd,(*it).GetData(),n))
			{
				ERRTRACE();
				break;
			}
		}
	}
	close(fd);
}

void DataPacketQueue::Load(const char* szFile)
{
	INFO("Load from %s",szFile);
	FILE* fd = fopen(szFile,"rb");
	if (fd == NULL)
	{
		ERRTRACE();
		return;
	}

	while(!feof(fd))
	{
		int n;
		if (fread(&n,sizeof(n),1,fd)< sizeof(n))
			break;

		if (n>0)
		{
			char* buf = new char[n];
			if (buf && (fread(buf,n,1,fd)== (unsigned)n))
			{
				Packet packet;
				packet.SetPacket(buf,n);
				if (packet.IsValidFrame()) Push(packet);
			}
			if (buf) delete buf;
		}
	}
	fclose(fd);
}

}
