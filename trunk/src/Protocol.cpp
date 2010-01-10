/*
 * Protocol.cpp
 *
 *  Created on: 2009-12-30
 *      Author: mxx
 */

#include "Protocol.h"
#include "PacketQueue.h"
#include "MPHealthCheckCmdPacket.h"
namespace bitcomm
{
Protocol::~Protocol()
{

}

void Protocol::NegoiateDataChannel(TCPPort& port)
{
	int retry = 3;
	int retry_connect=2;
	int nPort = negoiateChannel(port,50001);
	while(nPort && retry_connect--)
	{
		while(retry--)
		{
			port.SetRemotePort(nPort);
			port.SetTimeOut(5000000);
			if (port.Connect()>0)
			{
				bExtCommunicationError=false;
				return;
			}
		};
		nPort = negoiateChannel(port,50001);
		retry = 3;
	};
	bExtCommunicationError=true;
}

void Protocol::NegoiateControlChannel(TCPPort& port)
{
	int retry = 3;
	int retry_connect=2;
	int nPort = negoiateChannel(port,50101);
	while(nPort && retry_connect--)
	{
		while(retry--)
		{
			port.SetRemotePort(nPort);
			port.SetTimeOut(5000000);
			if (port.Connect()>0)
			{
				bExtCommunicationError=false;
				return;
			}
		};
		nPort = negoiateChannel(port,50101);
		retry = 3;
	};
	bExtCommunicationError=true;
}

int Protocol::negoiateChannel(TCPPort& port,int nStartPort)
{
	int start_port = nStartPort;
	int retry=0;
	int retry_data=0;

	do
	{
		if (port.Open(strServerName.c_str(),start_port)<0)
		{
			retry++;
			if (retry==12) break;
			if (retry%3==0)	start_port++;
			if (retry%6==0)
			{
				sleep(30);
				start_port=nStartPort;
			}
			else
				sleep(5);
			continue;
		}

		retry=0;
		CmdPacket cmd;
		port.SetTimeOut(5000000);
		cmd.ReceiveFrameFrom(port);
		if (cmd.GetSize()==0)
		{
			retry_data++;
			if (retry_data==4) break;
			if (start_port==nStartPort) start_port++;
			else start_port=nStartPort;
			continue;
		}

		port.Close();
		return cmd.GetAssignedPort();

	}while(false);
	return 0;
}


void Protocol::RequestCurrentData(Channel& port,Packet& data)
{
	port.Lock();
	CmdPacket request;
	request.SetCommand(cmdWord[DataRequest],Machine);
	request.SendTo(port);
	data.ReceiveFrameFrom(port);
	port.Unlock();
	return;
}

void Protocol::SendCurrentData(Channel& port, DataPacketQueue& queue)
{
	int retry =0;
	while(queue.GetSize())
	{
		Packet& packet=queue.Front();
		port.Write(packet.GetData(),sizeof(struct  Packet::DataPacketFrame));
		Packet ack;
		ack.ReceiveAckFrom(port);
		if (ack.IsAckNo(packet.GetDataNo()))
		{
			queue.Pop();
			retry = 0;
		}
		else
		{
			retry++;
			if (retry>2)
			{
				bExtCommunicationError=true;
				return;
			}
		}
	};
}

void Protocol::HealthCheck(Channel& dev,Channel& port,Packet& status)
{
	MPHealthCheckCmdPacket cmd(nLastStatus,Machine);
	cmd.SendTo(dev);
	Packet statusAnswer;
	statusAnswer.ReceiveFrameFrom(dev);
	if (statusAnswer.IsValidStatus())
	{
		unsigned int nNewStatus = statusAnswer.GetStatus();
		if (nNewStatus!=nLastStatus)
		{
			nLastStatus = nNewStatus;
			statusAnswer.SendTo(port);
			Packet ack;
			ack.ReceiveAckFrom(port);
		}
	}
}

enum CommunicationCommand Protocol::GetCommand(Channel& port,CmdPacket& cmd)
{
	cmd.ReceiveFrameFrom(port);
	return cmd.CommandType();
}

void Protocol::TransferCmd(Channel& dev,Channel& port,CmdPacket& cmd)
{
	dev.Lock();
	cmd.SendTo(dev);
	Packet answer;
	answer.ReceiveFrameFrom(dev);
	dev.Unlock();
	answer.SendTo(port);
}

void Protocol::HistoryDataTransfer(Channel& dev,Channel& port,HistoryDataRequestCmd& cmd)
{
	int nStart = cmd.GetStartNum();
	int nEnd = cmd.GetEndNum();

	int count=0;
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
		if (count==10)
		{
			SendQueueData(queue,port);
		}
		nStart++;
		cmd.SetStartNum(nStart);
	};
}

void Protocol::SendQueueData(DataPacketQueue& queue,Channel& port)
{
	do
	{
		for(int i=0;i<queue.GetSize();i++)
		{
			queue.GetAt(i).SendTo(port);
		}
		Packet ack;

		ack.ReceiveAckFrom(port);

		if (ack.IsValidAck() && ack.IsAck())
		{
			int ackNo =  ack.GetAckNo();
			while(queue.GetSize()&&	ackNo >= queue.Front().GetDataNo())
				queue.Pop();
		}
	}while(queue.GetSize());
}


int Protocol::Sleep(void)
{
	return 0;
}

}
