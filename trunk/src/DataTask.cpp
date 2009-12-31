/*
 * DataTask.cpp
 *
 *  Created on: 2009-12-31
 *      Author: mxx
 */

#include "DataTask.h"
#include <queue>

namespace bitcomm
{

DataTask::DataTask()
{
	// TODO Auto-generated constructor stub

}

DataTask::~DataTask()
{
	// TODO Auto-generated destructor stub
}

void DataTask::run(void)
{
	//Start the thread to read and send data

}

void* DataTask::doProcess(void* pThis)
{
	TCPPort portServer;
	Protocol protocol;
	Packet currentData;
	PacketQueue dataQueue;

	while(1)
	{
		if (protocol.RequestCurrentData(portMP,currentData))
			dataQueue.push(currentData);
		protocol.SendCurrentData(portServer,dataQueue);
		protocol.HealthCheck(portMP,portServer);
		protocol.sleep();
	};

}


}
