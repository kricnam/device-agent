/*
 * Protocol.cpp
 *
 *  Created on: 2009-12-30
 *      Author: mxx
 */

#include "Protocol.h"
#include "PacketQueue.h"
#include "Packet.h"
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

int Protocol::NegoiateDataChannel(Modem& modem,TCPPort& port)
{
	int retry = 3;
	int retry_connect = 1;
	struct timeval tmStart, tmNow, tmDiff;
	INFO("Waiting for control channel connected...");
	while (!bControlDone)
	{
		sleep(1);
		if (modem.IsPowerOff()) return 0;
	};
	int nPort = negoiateChannel(port, nDataPort);
	INFO("Get Port %d",nPort);
	while (nPort && retry_connect--)
	{
		while (retry--)
		{
			port.SetRemotePort(nPort);
			port.SetTimeOut(nNegotialTimeOut*1000000);
			gettimeofday(&tmStart, 0);
			if (port.Connect() == 0)
			{
				INFO("connected");
				bExtCommunicationError = false;
				return nPort;
			}
			//sleep
			if (modem.IsPowerOff()) return 0;
			gettimeofday(&tmNow, 0);
			timersub(&tmNow,&tmStart,&tmDiff);
			if (tmDiff.tv_sec * 1000000 + tmDiff.tv_usec < nNegotialTimeOut*1000000)
			{
				INFO("Sleep for next try");
				usleep(nNegotialTimeOut*1000000 - tmDiff.tv_sec * 1000000 - tmDiff.tv_usec);
			}
		};
		if (modem.IsPowerOff()) return 0;
		nPort = negoiateChannel(port, nDataPort);
		retry = 3;
	};
	if (!modem.IsPowerOff())
		bExtCommunicationError = true;

	return -1;
}

void Protocol::NegoiateControlChannel(TCPPort& port)
{
	int retry = 3;
	int retry_connect = 1;
	int nPort = negoiateChannel(port, nCommandPort);
	INFO("Get Port %d",nPort);
	struct timeval tmStart, tmNow, tmDiff;
	while (nPort && retry_connect--)
	{
		while (retry--)
		{
			port.SetRemotePort(nPort);
			port.SetTimeOut(nNegotialTimeOut*1000000);
			gettimeofday(&tmStart, 0);
			if (port.Connect() == 0)
			{
				INFO("connected");
				bExtCommunicationError = false;
				bControlDone = true;
				return;
			}

			//sleep
			gettimeofday(&tmNow, 0);
			timersub(&tmNow,&tmStart,&tmDiff);
			if (tmDiff.tv_sec*1000000+tmDiff.tv_usec < nNegotialTimeOut*1000000 )
			{
				INFO("Sleep for next try");
				usleep(nNegotialTimeOut*1000000 - tmDiff.tv_sec*1000000-tmDiff.tv_usec);
			}
		};
		nPort = negoiateChannel(port, nCommandPort);
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

	TRACE("Last ActiveTime %u.%06u",tmLastActive.tv_sec,tmLastActive.tv_usec);
	setLastActionTime(srvData.GetActiveTime());
	setLastActionTime(srvControl.GetActiveTime());
	TRACE("Last ActiveTime %u.%06u",tmLastActive.tv_sec,tmLastActive.tv_usec);
	TRACE("Now time is %u ",tmNow.tv_sec);
	timersub(&tmNow,&tmLastActive,&tmDiff);
	INFO("idle time %u s [setting:%d s]",tmDiff.tv_sec,nIdleTimeSetting);
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
	if (n1 < 0) n1 = 0;
	if (n2 < 0) n2 = 0;
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
	struct timeval tmStart,tmNow,tmDiff;

	do
	{
		TRACE("port=%d",start_port);
		gettimeofday(&tmStart,0);
		if (port.Open(strServerName.c_str(), start_port) < 0)
		{
			gettimeofday(&tmNow,0);
			retry++;
			TRACE("Retry %d",retry);
			if (retry == 6)
				break;

			if (retry % 3 == 0)
			{
				if (start_port == nStartPort)
					start_port++;
				else
					start_port = nStartPort;
			}

			timersub(&tmNow,&tmStart,&tmDiff);
			int uSleep = tmDiff.tv_sec*1000000 + tmDiff.tv_usec;
			if (uSleep>0) usleep(uSleep);
			TRACE("retry next time");
			continue;
		};

		INFO("port opened, waiting command...");
		CmdPacket cmd;
		port.SetTimeOut(20000000);
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

	time_t nowMP = GetMPTime();
	time_t nowLocal;

	if (nowMP && time(&nowLocal)!=-1)
	{
		int dt;
		if (nowMP > nowLocal)
			dt = nowMP -nowLocal;
		else
			dt = nowLocal - nowMP;
		if (dt > 5)
		{
			struct timeval tmNow;
			tmNow.tv_sec = nowMP;
			tmNow.tv_usec = 0;
			if (settimeofday(&tmNow,0))
			{
				ERRTRACE();
			}
		}
	}

	if (nowMP && nowMP < tmCurrentDataActive.tv_sec)
	{
		int dt = tmCurrentDataActive.tv_sec - nowMP;
		if (dt > 0 && dt < 10 ) sleep(dt+1);
	}

	int nIntervalSecond = GetMPIntervalSecond();
	DEBUG("Set data interval %d",nIntervalSecond);
	if (nIntervalSecond==0) nIntervalSecond = 120;
	setReservedTime(tmCurrentDataActive,nIntervalSecond);

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

				//avoid the former answer messed with  this one
				// if not the right answer ,read again
				if (data.GetData() && data.FrameCommandType()!= DataRequest)
				{
					data.ReceiveFrameFrom(port);
				}

				if (data.GetSize())
					break;
			}
			catch (ChannelException& e)
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

	DEBUG("Get Data: No.%d",data.GetDataNo());
	return;
}

void Protocol::SendCurrentData(Modem& modem,Channel& port, DataPacketQueue& queue)
{
	int retry = 0;
	setReservedTime(tmSendDataActive,nIntervalSetting);
	while (queue.GetSize())
	{
		Packet& packet = queue.Front();
		Packet ack;

		INFO("Send current data[%d]",packet.GetDataNo());

		if (packet.FrameCommandType()!=DataRequest)
		{
			ERROR("not a [ra]  data,droped");
			queue.Pop();
			continue;
		}

		try
		{
			port.Write(packet.GetData(), sizeof(struct Packet::DataPacketFrame));
			ack.ReceiveAckFrom(port);
		}
		catch (ChannelException& e)
		{
			WARNING(e.what());

			if (e.bUnConnected)
			{
				if (typeid(port) == typeid(TCPPort))
				{
					if (NegoiateDataChannel(modem,dynamic_cast<TCPPort&> (port))==0)
						return;
				}
				else
					port.Open();

				if (bExtCommunicationError) return;
				if (modem.IsPowerOff()) return;
				else continue;
			}

		}

		if (modem.IsPowerOff()) return;
		ack.Dump();
		if (ack.IsAck() && ack.IsAckNo(packet.GetDataNo()))
		{
			INFO("Get ACK[%d]",ack.GetAckNo());
			queue.Pop();
			retry = 0;
		}
		else
		{
			if (retry > 3)
			{
				if (!modem.IsPowerOff())
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
	INFO("");
	setReservedTime(tmHealthCheckActive, 30);
	MPHealthCheckCmdPacket cmd(nLastStatus, Machine,
			bInCommunicationError,bExtCommunicationError);
	cmd.SendTo(dev);
	status.ReceiveFrameFrom(dev);
	DEBUG("Get Status = 0x%08X, DataNo.%d",status.GetStatus(),status.GetDataNo());
	if (status.IsValidStatus())
	{
		unsigned int nNewStatus = status.GetStatus();
		if (nNewStatus != nLastStatus)
		{
			nLastStatus = nNewStatus;
			statusQueue.Push(status);
		}
	}
}

void Protocol::HealthCheckReport(Channel & port)
{
	Packet ack;
	int retry = 0;
	if (bExtCommunicationError || !port.IsOpen())
				INFO("Can not send status now.");
	else
	{
		while (statusQueue.GetSize())
				{
					Packet statusAnswer = statusQueue.Front();
					INFO("Report status 0x%08X",statusAnswer.GetStatus());
					ack.Clear();
					try
					{
						statusAnswer.SendTo(port);
						ack.ReceiveAckFrom(port);
					} catch (ChannelException& e)
					{
						retry++;
						WARNING(e.what());
					}
					ack.Dump();
					if (ack.IsAck() && ack.IsAckNo(statusAnswer.GetDataNo()))
					{
						INFO("status accepted");
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

enum CommunicationCommand Protocol::GetCommand(Channel& port, CmdPacket& cmd)
{
	INFO("Waiting Command from control channel");
	cmd.ReceiveFrameFrom(port);
	return cmd.CommandType();
}

void Protocol::TransferCmd(Channel& dev, Channel& port, CmdPacket& cmd)
{
	int retry=0;
	Packet answer;
	dev.Lock();
	dev.SetTimeOut(5000000);
		do
		{
			if(!answer.GetSize())
			{
				try
				{
					cmd.SendTo(dev);
				}
				catch (ChannelException& e)
				{
					WARNING("%s", e.what());
				}
			}

			try
			{
				if (!answer.GetSize())
				{
					answer.ReceiveFrameFrom(dev);
				}

				if (answer.GetSize())
				{
					answer.SendTo(port);
					retry=0;
					break;
				}
				else
				{
					retry++;
					continue;
				}
			}
			catch (ChannelException& e)
			{
						WARNING("%s", e.what());
						retry++;
			}
		} while (retry < 3);

		dev.Unlock();

		if (answer.GetSize()==0)
		{
			bInCommunicationError = true;
		}
		else if(retry==3)
		{
			bExtCommunicationError = true;
		}
		else
		{
			bExtCommunicationError = false;
			bInCommunicationError = false;
		}

}

void Protocol::HistoryDataTransfer(Channel&
		dev, Channel& port,
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

time_t Protocol::GetMPTime(void)
{
	CmdPacket cmdGetTime;
	Packet timeAnswer;
	cmdGetTime.SetCommand(cmdWord[GetTime],Machine);

	cmdGetTime.SendTo(devMP);
	devMP.SetTimeOut(300000);
	timeAnswer.ReceiveFrameFrom(devMP);

	if (timeAnswer.GetSize())
	{
		return timeAnswer.GetMPTime();
	}
	else
	{
		ERROR("Can not get time from Monitor Post");
		return 0;
	}
}

int Protocol::GetMPIntervalSecond(void)
{
	CmdPacket cmdGetTime;
	Packet timeAnswer;
	cmdGetTime.SetCommand(cmdWord[GetCondition],Machine);

	cmdGetTime.SendTo(devMP);
	devMP.SetTimeOut(10000000);
	timeAnswer.ReceiveFrameFrom(devMP);

	if (timeAnswer.GetSize())
	{
		return timeAnswer.GetInterval()*60;
	}
	else
	{
		ERROR("Can not get time from Monitor Post");
		return 120;
	}
}

}
