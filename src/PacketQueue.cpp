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
#include "DebugLog.h"

namespace bitcomm
{

DataPacketQueue::DataPacketQueue()
{
	if (pthread_mutex_init(&mutex,NULL)!=0)

		ERRTRACE()
}

DataPacketQueue::~DataPacketQueue()
{
	pthread_mutex_destroy(&mutex);
}

void DataPacketQueue::Push(Packet& data)
{
	pthread_mutex_lock(&mutex);
	TRACE("data[%d],size %d",data.GetDataNo(),data.GetSize());
	if (data.GetSize())
	{
		list<Packet>::iterator it;
		for(it = queue.begin();it!=queue.end();it++)
		{
			if ((*it).GetDataNo() == data.GetDataNo())
			{
				pthread_mutex_unlock(&mutex);
				return;
			}
		}
		queue.push_back(data);
	}
	pthread_mutex_unlock(&mutex);
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
	pthread_mutex_lock(&mutex);
	queue.pop_front();
	pthread_mutex_unlock(&mutex);
}

void DataPacketQueue::Save(const char* szFile)
{
	INFO("Save to %s",szFile);
	int fd = open(szFile,O_CREAT|O_WRONLY|O_TRUNC,S_IRWXU);
	if (fd < 0)
	{
		ERRTRACE();
		return;
	}
	pthread_mutex_lock(&mutex);
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
	pthread_mutex_unlock(&mutex);
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
	queue.clear();
	while(!feof(fd))
	{
		int n;
		if (fread(&n,sizeof(n),1,fd)< 1)
			break;

		if (n>0 && n < 5000)
		{
			char* buf = new char[n];
			if (buf && (fread(buf,n,1,fd)== 1))
			{
				Packet packet;
				packet.SetPacket(buf,n);

				if (packet.IsValidFrame()) Push(packet);
				else
				{
					WARNING("Invalid data");
					packet.Dump();
				}
			}
			else
			{
				WARNING("Invalid data,file maybe corrupted.");
			}
			if (buf) delete buf;
		}
		else
		{
			WARNING("Corrupted data, move to next");
		}
	}
	fclose(fd);
	//if (remove(szFile)!=0)
	//	ERRTRACE();
	INFO("Total %d records loaded",queue.size());
}

}
