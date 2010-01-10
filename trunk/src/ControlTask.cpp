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

ControlTask::ControlTask(SerialPort& port,Modem& m,Protocol& p):portMP(port),modem(m),protocol(p)
{

}

ControlTask::~ControlTask()
{

}

void ControlTask::run(void)
{

}

void* ControlTask::doProcess(void* pThis)
{
	ControlTask& task = *(ControlTask*)pThis;
	TCPPort port;
	CmdPacket cmd;
	enum CommunicationCommand eCmd;

	while(true)
	{
		if (task.modem.IsPowerOff()) task.modem.PowerOn();
		eCmd = task.protocol.GetCommand(port,cmd);
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
			task.protocol.TransferCmd(task.portMP,port,cmd);
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
			task.protocol.TransferCmd(task.portMP,port,cmd);
			break;
		case ConfirmDoseRate:
		case ConfirmSpectrum:
		case ConfirmDoseRateAlarm:
		case Confirm40KAction:
		case ConfirmGPS:
		{
			HistoryDataRequestCmd hcmd(cmd);
			task.protocol.HistoryDataTransfer(task.portMP,port,hcmd);
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
			if (task.protocol.IsTimeForSleep())
			{
				//Modem Power off
				task.modem.PowerOff();
				task.protocol.SleepForPowerOn();
			}
			break;
		};
	}
	return pThis;
}

}
