/*
 * ControlTask.cpp
 *
 *  Created on: 2009-12-31
 *      Author: mxx
 */

#include "ControlTask.h"
#include "Modem.h"
#include "Protocol.h"
#include "TCPPort.h"

namespace bitcomm
{

ControlTask::ControlTask(SerialPort& port,Modem& m):portMP(port),modem(m)
{

}

ControlTask::~ControlTask()
{
	// TODO Auto-generated destructor stub
}

void ControlTask::run(void)
{

}

void* ControlTask::doProcess(void* pThis)
{
	Protocol protocol;
	//TCPPort portServer;

	while(true)
	{
		//if (protocol.getCommand())
		//	protocol.processCommand();
		//else
		{
			//modem.powerOff();
			//modem.waitPowerOn();
		}
	}
	return pThis;
}

}
