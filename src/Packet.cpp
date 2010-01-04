/*
 * Packet.cpp
 *
 *  Created on: 2009-12-28
 *      Author: mxx
 */

#include "Packet.h"
#include "CRC16.h"
#include "CommunicationCommand.h"
namespace bitcomm
{
Packet::Packet()
{

}

Packet::~Packet()
{

}


void Packet::buildPacket(const char* szContent,int size,unsigned char Machine)
{
	strCache.clear();
	strCache+=SOH;
	strCache.append(1,Machine);
	strCache+=STX;
	strCache.append(szContent,size);
	strCache+=ETX;
	unsigned short crc = CRC16::crc16(~0,
			(const unsigned char*)strCache.data()+3,size+1);
	strCache.append(1,(crc>>8)&0xFF00);
	strCache.append(1,(crc&0x0FF));
	strCache+=EOT;
}

void Packet::SendTo(Channel& port)
{
	port.Write(strCache.c_str(),strCache.size());
}

void Packet::ReceiveAckFrom(Channel& port)
{
	strCache.clear();
	char buff;
	int n = port.Read(&buff,1);

	if (n==0) return;
	do
	{
		//scan frame start
		if (strCache.empty())
		{
			while(n && ( buff!=ACK || buff!=NAK)) {n = port.Read(&buff,1);};
			if (n==0) break;
		}

		do
		{
			strCache.append(&buff,1);
			n = port.Read(&buff,1);
		} while(n);
	}
	while(n);
}

void Packet::ReceiveFrameFrom(Channel& port)
{
	strCache.clear();
	char buff;

	int n = port.Read(&buff,1);
	if (n==0) return;

	do
	{
		//scan frame start
		if (strCache.empty())
		{
			while(buff!=SOH && n) {n = port.Read(&buff,1);};
			if (n==0) break;
		}

		do
		{
			strCache.append(&buff,1);
			if (buff==EOT && isValidFrame()) return;
			n = port.Read(&buff,1);
		} while(n);
	}
	while(n);

	while(!strCache.empty())
	{
		slipToNextStartToken();
		if (isFrameCRCOK()) return;
	}
}

bool Packet::isValidFrame(void)
{
	do
	{
		if (isFrameCRCOK())
			return true;

		if (strCache.size()>4056)
		{
			slipToNextStartToken();
			continue;
		}
	}
	while(false);
	return  false;
}

bool Packet::isFrameCRCOK(void)
{
	if (strCache.size()< 9) return false;
	int tail = strCache.size() - 1;
	if (strCache[tail]==EOT && strCache[tail-3]==ETX)
	{
		unsigned short crc = CRC16::crc16(~0,
				(const unsigned char*)strCache.data()+3,strCache.size()-5);
		return crc == strCache[tail-2]*256 + strCache[tail-1];
	}
	return false;
}

void Packet::slipToNextStartToken(void)
{
	string::size_type pos = strCache.find(SOH,1);

	if (pos!=string::npos)
		strCache = strCache.erase(0,pos);
	else
		strCache.clear();

}

unsigned short Packet::GetDataNo(void)
{
	if (strCache.size()<1) return 0;

	struct DataPacketFrame* pData = (struct DataPacketFrame*) GetData();
	return pData->dataNo;
}

unsigned short Packet::GetAssignedPort(void)
{
	if (strCache.size()<1) return 0;
	struct PortAssignFrame* pData = (struct PortAssignFrame*) GetData();
	return pData->port;
}

int Packet::GetSize()
{
	return strCache.size();
}

bool Packet::IsAckNo(unsigned short No)
{

	return false;
}

bool Packet::IsValidStatus(void)
{
	struct HealthCheckStatusAnswerFrame* pData = (struct HealthCheckStatusAnswerFrame*)GetData();
	if (strCache.size()>0)
	{
		return CmdPacket::IsCommand(pData->cmd,MPHealthCheck);
	}
	return false;
}

unsigned int Packet::GetStatus(void)
{
	struct HealthCheckStatusAnswerFrame* pData = (struct HealthCheckStatusAnswerFrame*)GetData();
	if (strCache.size()>0)
	{
		return pData->nStatus;
	}
	return 0;
}

}//name space end
