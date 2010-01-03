/*
 * Protocol.cpp
 *
 *  Created on: 2009-12-30
 *      Author: mxx
 */

#include "Protocol.h"
#include "CommunicationCommand.h"
namespace bitcomm
{
struct PortAssign
{
	unsigned char soh;
	unsigned char machine;
	unsigned char stx;
	unsigned char cmd0;
	unsigned char cmd1;
	unsigned short port;
	unsigned char etx;
	unsigned short crc16;
	unsigned char eot;
} __attribute__ ((packed));

Protocol::Protocol()
{
	// TODO Auto-generated constructor stub

}

Protocol::~Protocol()
{
	// TODO Auto-generated destructor stub
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
				break;

		};
		nPort = negoiateChannel(port,50001);
		retry = 3;
	};
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
				break;

		};
		nPort = negoiateChannel(port,50101);
		retry = 3;
	};
}

int Protocol::negoiateChannel(TCPPort& port,int nStartPort)
{
	int start_port = nStartPort;
	int retry=0;
	int retry_data=0;

	do
	{
		if (port.Open("localhost",start_port)<0)
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

		struct PortAssign* pPortCmd = (struct PortAssign*)cmd.GetData();
		port.Close();
		return pPortCmd->port;

	}while(false);
	return 0;
}


void Protocol::RequestCurrentData(unsigned char Machine,Channel& port,Packet& data)
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
		struct DataPacketFrame& packet=queue.Front();
		port.Write((char*)&packet,sizeof(struct DataPacketFrame));
		Packet ack;
		ack.ReceiveAckFrom(port);
		if (ack.IsAckNo(packet.dataNo))
		{
			queue.Pop();
			retry = 0;
		}
		else
		{
			retry++;
			if (retry>2)
			{
				//TODO:set error
				return;
			}
		}
	};
}

void Protocol::HealthCheck(unsigned char Machine,Channel& dev,Channel& port,Packet& status)
{

}

int Protocol::Sleep(void)
{
	return 0;
}
}
