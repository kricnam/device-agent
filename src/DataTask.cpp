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

void DataTask::SaveData()
{
	if (dataQueue.GetSize())
		dataQueue.Save("./Current.data");
}

void DataTask::run(void)
{
	//Start the thread to read and send data
	TRACE("pidTask=%d",pidTask);
	if (pidTask !=(pthread_t) -1) return;
	TRACE("create thread");
	pthread_create(&pidTask,NULL,DataTask::doProcess,this);
}

void* DataTask::doReadData(void* pThis)
{
	INFO("started");
	DataTask& task = *(DataTask*)pThis;

	SerialPort& portMP = task.protocol.GetMPPort();
	Packet currentData;
	while(1)
	{
		task.protocol.RequestCurrentData(portMP,currentData);
		if (currentData.GetSize())
		{
			task.dataQueue.Push(currentData);
			DEBUG("data queue:%d,ID:%d",task.dataQueue.GetSize(),currentData.GetDataNo());
			task.SaveData();
			currentData.Clear();
		}


		task.protocol.HealthCheck(portMP,currentData);
		currentData.Clear();
		task.protocol.PatrolRest();
	};
	return pThis;
}

void* DataTask::doProcess(void* pThis)
{
	INFO("Started...");
	DataTask& task = *(DataTask*)pThis;
	TCPPort& portServer = task.protocol.GetDataPort();

	Packet currentData;
	task.dataQueue.Load("./Current.data");
	pthread_t pid_read;
	pthread_create(&pid_read,NULL,DataTask::doReadData,pThis);

	while(1)
	{

		if (!task.modem.IsPowerOff())
		{
			int nPort = task.protocol.NegoiateDataChannel(task.modem,portServer);

			if (nPort!=0 && nPort!=-1)
			{
				task.protocol.SendCurrentData(task.modem, portServer,
						task.dataQueue);
				task.protocol.HealthCheckReport(portServer);

				if (portServer.IsOpen())
				{
					portServer.Close();
					INFO("Close data port");
				}
			}
		}
		//INFO("Data Thread");
		//task.protocol.SleepForPowerOn();
		while (!task.modem.IsPowerOff())
		{
			sleep(10);
		};
		while(task.modem.IsPowerOff())
		{
			sleep(task.protocol.GetIdleTime()/2);
		}
	};

	return pThis;
}


}
