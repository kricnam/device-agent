/*
 * Packet.cpp
 *
 *  Created on: 2009-12-28
 *      Author: mxx
 */

#include "Packet.h"
namespace bitcomm
{
Packet::Packet()
{

}

Packet::~Packet()
{
	// TODO Auto-generated destructor stub
}


void Packet::buildPacket(const char* szContent,int size,unsigned char Machine)
{
	strCache.clear();
	strCache+=SOH;
	strCache.append(1,Machine);
	strCache+=STX;
	strCache.append(szContent,size);
	strCache+=ETX;
	unsigned short crc = CRC16::crc16(~0,strCache.c_str()+3,size+1);
	strCache.append(1,(crc>>8)&0xFF00);
	strCache.append(1,(crc&0x0FF));
	strCache+=EOT;
}

void Packet::send(Channel& port)
{
	port.Write(strCache.c_str(),strCache.size());
}

void Packet::read()
{

}

}
