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
#include "DebugLog.h"

namespace bitcomm
{

ControlTask::ControlTask(Protocol& p,Modem& m):protocol(p),modem(m)
{

}

ControlTask::~ControlTask()
{

}

void ControlTask::run(void)
{
	pthread_create(&pidTask,NULL,ControlTask::doProcess,this);
}

void* ControlTask::doProcess(void* pThis)
{
	ControlTask& task = *(ControlTask*)pThis;
	TCPPort& port = task.protocol.GetControlPort();
	SerialPort& portMP = task.protocol.GetMPPort();
	CmdPacket cmd;
	enum CommunicationCommand eCmd;

	while(true)
	{
		if (task.modem.IsPowerOff())
		{
			INFO("Power On Modem");
			task.modem.PowerOn();
			if (task.modem.IsPowerOff())
				task.protocol.SetExtCommunicationError(true);
			else
				task.protocol.SetExtCommunicationError(false);
		}

		if (!task.modem.IsPowerOff())
		{
		try
		{
			eCmd = task.protocol.GetCommand(port,cmd);
			TRACE("Get Command %s",cmdWord[eCmd]);
			switch(eCmd)
			{
			case DataRequest:
			case MPHealthCheck:
				break;
			case GetCondition:
			case SetCondition:
			case GetPreAmp:
			case GetADCSetting:
			case GetSpectrumSetting:
			case SetLowHiDoesRateAlarmReset:
			case GetTime:
			case RequestDoseRate:
			case RequestSpectrum:
			case RequestDoseRateAlarm:
			case Request40KAction:
			case RequestGPS:
				task.protocol.TransferCmd(portMP,port,cmd);
				break;
			case GetNetworkSetting:
				break;
			case SetTransmitUnitHardwareReset:
				break;
			case SetTransmitUnitReset:
				break;
			case SetTransmitUnitMemortClear:
				break;
			case SetTime:
				//TODO:SetLocalTime();
				task.protocol.TransferCmd(portMP,port,cmd);
				break;
			case ConfirmDoseRate:
			case ConfirmSpectrum:
			case ConfirmDoseRateAlarm:
			case Confirm40KAction:
			case ConfirmGPS:
			{
				HistoryDataRequestCmd hcmd(cmd);
				task.protocol.HistoryDataTransfer(portMP,port,hcmd);
				break;
			}
			case RequestDataCancel:
			case DataTerminate:
			case SetDataPort:
			case SetControlPort:
				break;
			case ControlPortHealthCheck:
				//TODO:send ACK frame;
				break;
			case CMD_END:
				TRACE("Got no command");
				break;
			};

		}
		catch (ChannelException& e)
		{
			WARNING("Exception:%s",e.what());
			if (e.bUnConnected)
			{
				task.protocol.NegoiateControlChannel(port);
			}

		}
		}

		if (task.protocol.IsTimeForSleep())
		{
			//Modem Power off
			if (!task.modem.IsPowerOff())
				task.modem.PowerOff();
			task.protocol.SleepForPowerOn();
		}


	};
	return pThis;
}

}
