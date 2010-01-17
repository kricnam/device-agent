/*
 * Protocol.cpp
 *
 *  Created on: 2009-12-30
 *      Author: mxx
 */

#include "Protocol.h"
#include "PacketQueue.h"
#include "MPHealthCheckCmdPacket.h"
#include "DebugLog.h"
#include <unistd.h>
#include <sys/time.h>
#include <typeinfo>
#include <stdio.h>

namespace bitcomm
{

Protocol::~Protocol()
{

}

void Protocol::NegoiateDataChannel(TCPPort& port)
{
	int retry = 3;
	int retry_connect = 2;
	int nPort = negoiateChannel(port, 50001);
	while (nPort && retry_connect--)
	{
		while (retry--)
		{
			port.SetRemotePort(nPort);
			port.SetTimeOut(5000000);
			if (port.Connect() > 0)
			{
				bExtCommunicationError = false;
				return;
			}
		};
		nPort = negoiateChannel(port, 50001);
		retry = 3;
	};
	bExtCommunicationError = true;
}

void Protocol::NegoiateControlChannel(TCPPort& port)
{
	int retry = 3;
	int retry_connect = 2;

	int nPort = negoiateChannel(port, 50101);
	TRACE("Get Port %d",nPort);
	struct timeval tmStart, tmNow, tmDiff;
	while (nPort && retry_connect--)
	{
		while (retry--)
		{
			port.SetRemotePort(nPort);
			port.SetTimeOut(5000000);
			gettimeofday(&tmStart, 0);
			if (port.Connect() == 0)
			{
				TRACE("connected");
				bExtCommunicationError = false;
				return;
			}

			//sleep
			gettimeofday(&tmNow, 0);
			timersub(&tmNow,&tmStart,&tmDiff);
			if (tmDiff.tv_usec > 500000)
				tmDiff.tv_sec++;
			if (tmDiff.tv_sec < 5)
				sleep(5 - tmDiff.tv_sec);
		};
		nPort = negoiateChannel(port, 50101);
		retry = 3;
	};
	DEBUG("Set External communication error");
	bExtCommunicationError = true;
}

bool Protocol::IsTimeForSleep(void)
{
	struct timeval tmNow;
	struct timeval tmDiff;
	gettimeofday(&tmNow, 0);
	setLastActionTime(srvData.GetActiveTime());
	setLastActionTime(srvControl.GetActiveTime());
	setLastActionTime(devMP.GetActiveTime());
	timersub(&tmNow,&tmLastActive,&tmDiff);
	INFO("idle time %us [setting:%d]",tmDiff.tv_sec,nIdleTimeSetting);
	return ((unsigned) tmDiff.tv_sec > (unsigned) nIdleTimeSetting);
}

void Protocol::PatrolRest(void)
{
	struct timeval tmNow, tmSleep2, tmSleep1;
	gettimeofday(&tmNow, 0);
	timersub(&tmCurrentDataActive,&tmNow,&tmSleep1);
	timersub(&tmHealthCheckActive,&tmNow,&tmSleep2);
	int n1 = tmSleep1.tv_sec*1000000+tmSleep1.tv_usec;
	int n2 = tmSleep2.tv_sec*1000000+tmSleep2.tv_usec;
	n1 = (n1>n2)?n2:n1;
	INFO("Sleep %d millisecond",n1/1000);
	usleep(n1);
}

void Protocol::SleepForPowerOn(void)
{

	struct timeval tmNow, tmWake, tmSleep;
	gettimeofday(&tmNow, 0);
	tmWake.tv_sec = ((tmNow.tv_sec / nIntervalSetting) + 1) * nIntervalSetting;
	tmWake.tv_usec = 0;
	timersub(&tmWake,&tmNow,&tmSleep);
	TRACE("Sleep %d sec",tmSleep.tv_sec);
	usleep(tmSleep.tv_sec * 1000000 + tmSleep.tv_usec);
}

int Protocol::negoiateChannel(TCPPort& port, int nStartPort)
{
	int start_port = nStartPort;
	int retry = 0;
	int retry_data = 0;

	do
	{
		TRACE("port=%d",start_port);
		if (port.Open(strServerName.c_str(), start_port) < 0)
		{
			retry++;
			TRACE("Retry %d",retry);
			if (retry == 18)
				break;
			if (retry % 6 == 0)
			{
				TRACE("Waiting 30s");
				sleep(30);
				if (start_port == nStartPort)
					start_port++;
				else
					start_port = nStartPort;
			}
			else if (retry % 3 == 0)
			{
				if (start_port == nStartPort)
					start_port++;
				else
					start_port = nStartPort;
			}
			else
			{
				TRACE("Waiting 5s");
				sleep(5);
			}
			TRACE("retry next time");
			continue;
		};

		INFO("port opened, waiting command...");
		CmdPacket cmd;
		port.SetTimeOut(5000000);
		try
		{
			cmd.ReceiveFrameFrom(port);

		} catch (ChannelException& e)
		{
			TRACE("exception %s",e.what());
		}

		retry = 0;
		if (cmd.GetSize() == 0)
		{
			retry_data++;
			if (retry_data == 4)
				break;
			if (start_port == nStartPort)
				start_port++;
			else
				start_port = nStartPort;
			continue;
		}

		try
		{
			Packet ack;
			ack.Ack(true, Machine, cmd.CommandType(), cmd.GetAssignedPort());
			ack.SendTo(port);
		} catch (ChannelException& e)
		{
			WARNING(e.what());
		}

		port.Close();

		return cmd.GetAssignedPort();

	} while (true);

	return 0;
}

void Protocol::RequestCurrentData(Channel& port, Packet& data)
{
	if (!isTimeForAction(tmCurrentDataActive))
		return;
	setReservedTime(tmCurrentDataActive, 10 * 60);
	INFO("request current data");

	CmdPacket request;
	int retry = 3;
	request.SetCommand(cmdWord[DataRequest], Machine);
	port.SetTimeOut(1000000);
	port.Lock();
	do
	{
		try
		{
			request.SendTo(port);
			data.ReceiveFrameFrom(port);
			if (data.GetSize())
				break;
		} catch (ChannelException& e)
		{

		}
	} while (--retry);

	port.Unlock();

	if (!retry && !data.GetSize())
	{
		INFO("set inter communication error");
		bInCommunicationError = true;
	}
	else
		bInCommunicationError = false;

	return;
}

void Protocol::SendCurrentData(Channel& port, DataPacketQueue& queue)
{
	int retry = 0;
	while (queue.GetSize())
	{
		Packet& packet = queue.Front();
		Packet ack;

		try
		{
			port.Write(packet.GetData(), sizeof(struct Packet::DataPacketFrame));
			ack.ReceiveAckFrom(port);
		}
		catch (ChannelException& e)
		{
			WARNING(e.what());
			retry++;
			if (e.bUnConnected)
			{
				if (typeid(port) == typeid(TCPPort))
				{
					NegoiateDataChannel(dynamic_cast<TCPPort&> (port));
				}
				else
					port.Open();
				continue;
			}
		}

		if (ack.IsAck() && ack.IsAckNo(packet.GetDataNo()))
		{
			queue.Pop();
			retry = 0;
		}
		else
		{
			if (retry > 2)
			{
				bExtCommunicationError = true;
				return;
			}
			retry++;
		}
	};
}

void Protocol::HealthCheck(Channel& dev, Packet& status)
{
	if (!isTimeForAction(tmHealthCheckActive))
		return;
	TRACE("");
	setReservedTime(tmHealthCheckActive, 30);
	MPHealthCheckCmdPacket cmd(nLastStatus, Machine,
			bInCommunicationError,bExtCommunicationError);
	cmd.SendTo(dev);
	status.ReceiveFrameFrom(dev);
}

void Protocol::HealthCheckReport(Channel & port, Packet& statusAnswer)
{
	if (statusAnswer.IsValidStatus())
	{
		unsigned int nNewStatus = statusAnswer.GetStatus();
		if (nNewStatus != nLastStatus)
		{
			Packet ack;
			nLastStatus = nNewStatus;
			statusQueue.Push(statusAnswer);
			int  retry=0;
			while(statusQueue.GetSize())
			{

				statusAnswer = statusQueue.Front();
				try
				{
						statusAnswer.SendTo(port);
						ack.ReceiveAckFrom(port);
				}
				catch (ChannelException& e)
				{
					retry++;
					WARNING(e.what());
				}

				if (ack.IsAck() && ack.IsAckNo(statusAnswer.GetDataNo()))
				{
						statusQueue.Pop();
						retry = 0;
				}
				else
				{
					if (retry > 2)
					{
						bExtCommunicationError = true;
						return;
					}
					retry++;
				}
			};
		}
	}
}

enum CommunicationCommand Protocol::GetCommand(Channel& port, CmdPacket& cmd)
{
	INFO("Waiting Command from control channel");
	cmd.ReceiveFrameFrom(port);
	return cmd.CommandType();
}

void Protocol::TransferCmd(Channel& dev, Channel& port, CmdPacket& cmd)
{
	dev.Lock();
	try
	{
		cmd.SendTo(dev);

	} catch (ChannelException& e)
	{
		printf("%s", e.what());
	}
	dev.Unlock();
	try
	{
		Packet answer;
		answer.ReceiveFrameFrom(dev);
		answer.SendTo(port);

	} catch (ChannelException& e)
	{
		printf("%s", e.what());
	}
}

void Protocol::HistoryDataTransfer(Channel& dev, Channel& port,
		HistoryDataRequestCmd& cmd)
{
	int nStart = cmd.GetStartNum();
	int nEnd = cmd.GetEndNum();

	int count = 0;
	DataPacketQueue queue;
	Packet answer;
	while (nStart <= nEnd)
	{
		dev.Lock();
		cmd.SendTo(dev);

		answer.ReceiveFrameFrom(dev);
		dev.Unlock();
		count++;
		queue.Push(answer);
		if (count == 10)
		{
			SendQueueData(queue, port);
		}
		nStart++;
		cmd.SetStartNum(nStart);
	};
}

void Protocol::SendQueueData(DataPacketQueue& queue, Channel& port)
{
	do
	{
		for (int i = 0; i < queue.GetSize(); i++)
		{
			queue.GetAt(i).SendTo(port);

		}
		Packet ack;

		ack.ReceiveAckFrom(port);

		if (ack.IsValidAck() && ack.IsAck())
		{
			int ackNo = ack.GetAckNo();
			while (queue.GetSize() && ackNo >= queue.Front().GetDataNo())
				queue.Pop();
		}
	} while (queue.GetSize());
}

int Protocol::Sleep(void)
{
	return 0;
}

void Protocol::setReservedTime(struct timeval& timer, int n, bool bAlign)
{
	struct timeval tmNow;
	gettimeofday(&tmNow, 0);

	if (bAlign)
	{
		timer.tv_sec = ((tmNow.tv_sec / n) + 1) * n;
		timer.tv_usec = 0;
	}
	else
	{
		timer.tv_sec = tmNow.tv_sec + n;
		timer.tv_usec = tmNow.tv_usec;
	}
}

bool Protocol::isTimeForAction(struct timeval & timer)
{
	struct timeval tmNow;
	gettimeofday(&tmNow, 0);
	TRACE("timer:%u  now:%u",timer.tv_sec,tmNow.tv_sec);
	return !timercmp(&tmNow,&timer,<);
}

}
