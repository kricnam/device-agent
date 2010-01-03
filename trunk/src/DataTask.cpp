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

DataTask::DataTask(SerialPort& port,Modem& m):portMP(port),modem(m)
{
	pidTask=-1;
}

DataTask::~DataTask()
{
	// TODO Auto-generated destructor stub
}

void DataTask::run(void)
{
	//Start the thread to read and send data
	if (pidTask == (unsigned )-1) return;
	pthread_create(&pidTask,NULL,DataTask::doProcess,NULL);
}

void* DataTask::doProcess(void* pThis)
{
	DataTask& task = *(DataTask*)pThis;
	TCPPort portServer;
	Protocol protocol;
	Packet currentData;
	DataPacketQueue dataQueue;

	portServer.Open("127.0.0.1",9999);

	while(1)
	{
		protocol.RequestCurrentData(1,task.portMP,currentData);
		dataQueue.Push(currentData);
		protocol.SendCurrentData(portServer,dataQueue);
		protocol.HealthCheck(1,task.portMP,portServer,currentData);
		protocol.Sleep();
	};

	return pThis;
}


}
