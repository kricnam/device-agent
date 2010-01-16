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
#include "pthread.h"
#include <queue>
#include "DebugLog.h"
namespace bitcomm
{

DataTask::DataTask(Protocol& p,Modem& m):protocol(p),modem(m)
{
	pidTask=-1;
}

DataTask::~DataTask()
{

}

void DataTask::run(void)
{
	//Start the thread to read and send data
	TRACE("pidTask=%d",pidTask);
	if (pidTask !=(pthread_t) -1) return;
	TRACE("create thread");
	pthread_create(&pidTask,NULL,DataTask::doProcess,this);
}

void* DataTask::doProcess(void* pThis)
{
	INFO("started");
	DataTask& task = *(DataTask*)pThis;
	TCPPort& portServer = task.protocol.GetDataPort();
	SerialPort& portMP = task.protocol.GetMPPort();
	Packet currentData;
	DataPacketQueue dataQueue;

	while(1)
	{
		task.protocol.RequestCurrentData(portMP,currentData);
		dataQueue.Push(currentData);
		if (!task.modem.IsPowerOff())
		{
			task.protocol.SendCurrentData(portServer,dataQueue);
		}

		task.protocol.HealthCheck(portMP,currentData);
		if (!task.modem.IsPowerOff())
		{
			task.protocol.HealthCheckReport(portServer,currentData);
		}

		task.protocol.PatrolRest();
	};

	return pThis;
}


}
