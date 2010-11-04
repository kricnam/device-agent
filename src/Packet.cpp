/*
 * Packet.cpp
 *
 *  Created on: 2009-12-28
 *      Author: mxx
 */
#include "CommunicationCommand.h"
#include "DebugLog.h"
#include "Packet.h"
#include "CRC16.h"
#include <string>
#include <typeinfo>
#include <stdio.h>
#include <arpa/inet.h>

using namespace std;
namespace bitcomm
{

char* cmdWord[]={"ra","wa","ca","cA","cb","cc","cd","ce","cF","cH","cI","cJ",
"ck","cK","ha","hA","hc","hC","hd","hD","hf","hF","hg","hG","hy","hz","pA","pB","wb",""};

Packet::Packet()
{

}

Packet::~Packet()
{

}

void Packet::BuildPacket(const char* szContent, int size, unsigned char Machine)
{
	strCache.clear();
	strCache += SOH;
	strCache.append(1, Machine);
	strCache += STX;
	strCache.append(szContent, size);
	strCache += ETX;
	unsigned short crc = CRC16::crc16(~0,
			(const unsigned char*) (strCache.data()) + 3, size + 1);
	strCache.append(1, (crc >> 8) & 0x00FF);
	strCache.append(1, (crc & 0x0FF));
	strCache += EOT;
}

void Packet::SendTo(Channel & port)
{
	port.Write(strCache.data(), strCache.size());
}

void Packet::ReceiveAckFrom(Channel & port)
{
	strCache.clear();
	char buff;
	int n = port.Read(&buff, 1);
	if (n == 0)
		return;
	port.SetTimeOut(10000000);
	do
	{
		//scan frame start
		if (strCache.empty())
		{
			while (n && (buff != ACK && buff != NAK))
			{
				n = port.Read(&buff, 1);
			};
			if (n == 0)
				break;

		}
		do
		{
			strCache.append(&buff, 1);
			n = port.Read(&buff, 1);
		} while (n);
	} while (n);
}

void Packet::ReceiveFrameFrom(Channel & port)
{
	TRACE("Receive from %s",typeid(port).name());
	strCache.clear();
	char buff;
	int n = port.Read(&buff, 1);
	if (n == 0)
		return;

	do
	{
		//scan frame start
		if (strCache.empty())
		{
			while (buff != SOH && n)
			{
				n = port.Read(&buff, 1);
			};
			if (n == 0)
			{
				break;
			}

		}
		do
		{
			strCache.append(&buff, 1);
			if (buff == EOT && isValidFrame())
				return;
			n = port.Read(&buff, 1);

		} while (n);
	} while (n);
	TRACE("Data reach end");
	Dump();
	while (!strCache.empty())
	{
		slipToNextStartToken();
		if (isFrameCRCOK())
			return;
	}
}

void Packet::Dump()
{
	char buf[1024]={0};
	int n=0;
	for(unsigned int i=0;i < strCache.size();i++)
	{
		sprintf(buf+n,"%02hhX ",strCache[i]);
		n = strlen(buf);
		if (n>1000) break;
	}
	DEBUG(buf);
}

void Packet::Ack(bool bAck,char Machine ,enum CommunicationCommand eCmd, short nNum)
{
	strCache.clear();
	if (bAck) strCache.append(1,(char)ACK);
	else strCache.append(1,(char)NAK);

	strCache.append(1,Machine);
	strCache.append(cmdWord[eCmd],2);
	if (nNum)
	{
		strCache.append(1,((nNum>>8) & 0x0FF));
		strCache.append(1,(nNum & 0x0FF));
	}
	Dump();
}

bool Packet::isValidFrame(void)
{
	do
	{
		if (isFrameCRCOK())
			return true;
		TRACE("CRC fail");
		if (strCache.size() > 4056)
		{
			slipToNextStartToken();
			continue;
		}
	} while (false);
	return false;
}

bool Packet::isFrameCRCOK(void)
{
	if (strCache.size() < 9)
		return false;

	int tail = strCache.size() - 1;
	if (strCache[tail] == EOT && strCache[tail - 3] == ETX)
	{
		unsigned short crc = CRC16::crc16(~0,
				(const unsigned char*) (strCache.data()) + 3, strCache.size()
						- 6);
		unsigned short CRC = (((strCache[tail - 2] << 8) & 0xFF00) | (0x00FF & strCache[tail - 1]));
		TRACE("CRC[%04x] calculated = %04x",CRC,crc);

		return crc == CRC;
	}
	return false;
}

void Packet::slipToNextStartToken(void)
{
	string::size_type pos = strCache.find(SOH, 1);
	if (pos != string::npos)
		strCache = strCache.erase(0, pos);

	else
		strCache.clear();

}
unsigned short Packet::GetDataNo(void)
{
	if (strCache.size() < 1)
		return 0;

	struct DataPacketFrame *pData = (struct DataPacketFrame*) (GetData());
	return ntohs(pData->dataNo);
}

int Packet::GetInterval(void)
{
	if (strCache.size() < 1)
		return 0;

	struct ConditionFrame *pData = (struct ConditionFrame*) (GetData());
	return (int)(pData->nInterval);
}

unsigned short Packet::GetAssignedPort(void)
{
	if (strCache.size() < 1)
		return 0;

	struct PortAssignFrame *pData = (struct PortAssignFrame*) (GetData());
	return ntohs(pData->port);
}
bool Packet::IsAckNo(unsigned short No)
{
	struct AcKFrame *p = (struct AcKFrame*) (GetData());
	//return (strCache.size() == sizeof(struct AcKFrame) && p->ack == ACK
	//		&& p->dataNumber == htons(No));
	return (p->ack == ACK && p->dataNumber == htons(No));
}

bool Packet::IsValidStatus(void)
{
	struct HealthCheckStatusAnswerFrame *pData =
			(struct HealthCheckStatusAnswerFrame*) (GetData());
	if (strCache.size() > 0)
	{
		return CmdPacket::IsCommand(pData->cmd, MPHealthCheck);
	}
	return false;
}

unsigned int Packet::GetStatus(void)
{
	struct HealthCheckStatusAnswerFrame *pData =
			(struct HealthCheckStatusAnswerFrame*) (GetData());
	if (strCache.size() > 0)
	{
		return ntohl(pData->nStatus);
	}
	return 0;
}

unsigned short Packet::GetAckNo(void)
{
	if (strCache.size() == 4)
		return 0;

	struct AcKFrame *p = (struct AcKFrame*) (GetData());
	return ntohs(p->dataNumber);
}

time_t Packet::GetMPTime(void)
{
	if (!strCache.size() )	return 0;

	struct GetTimeFrame *p = (struct GetTimeFrame*) (GetData());
	if (p->cmd[0] != cmdWord[GetTime][0]||
		p->cmd[1] != cmdWord[GetTime][1])
	{
		ERROR("Not a time frame");
		return 0;
	}
	Dump();
	struct tm now ={0};
	now.tm_year = ntohs(p->time.year) - 1900;
	now.tm_mon = p->time.month - 1;
	now.tm_mday = p->time.day;
	now.tm_hour = p->time.hour;
	now.tm_min = p->time.minute;
	now.tm_sec = p->second;
	return mktime(&now);
}

bool Packet::IsValidAck(void)
{
	return (!strCache.empty()) && (strCache[0] == ACK || strCache[0] == NAK);
}

bool Packet::IsAck(void)
{
	return strCache[0] == ACK;
}

void Packet::SetPacket(const char* szBuf,int n)
{
	strCache.clear();
	strCache.append(szBuf,n);
}

enum CommunicationCommand Packet::FrameCommandType()
{
	int eCmd;
	const char *pCmd = strCache.data()+3;
	if (GetSize() > 4)
	{
		for (eCmd = 0; eCmd < CMD_END; eCmd++)
		{
			if (pCmd[0] == cmdWord[eCmd][0] && pCmd[1] == cmdWord[eCmd][1])
				return (enum CommunicationCommand) eCmd;
		}
	}
	return CMD_END;
}


}//name space end
