/*
 * DataTask.cpp
 *
 *  Created on: 2009-12-31
 *      Author: mxx
 */

#include "DataTask.h"
#include "TCPPort.h"
#include "SerialPort.h"
#include "PacketQueue.h"
#include "Protocol.h"
#include <queue>

namespace bitcomm
{

DataTask::DataTask(Protocol& p,SerialPort& port,Modem& m):protocol(p),portMP(port),modem(m)
{
	pidTask=-1;
}

DataTask::~DataTask()
{

}

void DataTask::run(void)
{
	//Start the thread to read and send data
	if (pidTask != (unsigned )-1) return;
	pthread_create(&pidTask,NULL,DataTask::doProcess,NULL);
}

void* DataTask::doProcess(void* pThis)
{
	DataTask& task = *(DataTask*)pThis;
	TCPPort portServer;

	Packet currentData;
	DataPacketQueue dataQueue;

	while(1)
	{
		task.protocol.RequestCurrentData(task.portMP,currentData);
		dataQueue.Push(currentData);
		task.protocol.SendCurrentData(portServer,dataQueue);
		task.protocol.HealthCheck(task.portMP,portServer,currentData);
		task.protocol.Sleep();
	};

	return pThis;
}


}
