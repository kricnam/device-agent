/*
 * Packet.h
 *
 *  Created on: 2009-12-28
 *      Author: mxx
 */

#ifndef PACKET_H_
#define PACKET_H_
#include <string>
#include "Channel.h"
#include <sys/time.h>
using namespace std;
namespace bitcomm
{
class Packet {
public:
	Packet();
	virtual ~Packet();

	struct FrameHead
	{
		unsigned char SOH;
		unsigned char Machine;
		unsigned char STX;
	}__attribute__ ((packed));

	struct FrameEnd
	{
		unsigned char ETX;
		unsigned short CRC16;
		unsigned char EOT;
	} __attribute__ ((packed));

	struct FrameTime
	{
		unsigned short year;
		unsigned char month;
		unsigned char day;
		unsigned char hour;
		unsigned char minute;
	};

	struct DataPacketFrame
	{
		struct FrameHead head;
		unsigned char cmd[2];
		unsigned short length;
		unsigned short dataNo;
		unsigned char dummy[65];
	}__attribute__ ((packed));

	struct PortAssignFrame
	{
		struct FrameHead head;
		unsigned char cmd[2];
		unsigned short port;
	} __attribute__ ((packed));

	struct MPStatus
	{
		unsigned int nomean:17;
		unsigned int extComErr:1;
		unsigned int intComErr:1;
		unsigned int nouse:13;
	};

	struct HealthCheckStatusAnswerFrame
	{
		struct FrameHead head;
		char cmd[2];
		unsigned short length;
		unsigned short DataNo;
		struct FrameTime time;
		unsigned int nStatus;
		struct FrameEnd end;
	} __attribute__ ((packed));

	struct AcKFrame
	{
		char ack;
		unsigned char Machine;
		char cmd[2];
		unsigned short dataNumber;
	};

	const char* GetData() {return strCache.data();};
	int GetSize() 	{return strCache.size();};

	void buildPacket(const char* szContent,int size,unsigned char Machine);
	void SendTo(Channel& port);
	void ReceiveFrameFrom(Channel& port);
	void ReceiveAckFrom(Channel& port);
	bool IsAckNo(unsigned short n);
	bool IsValidStatus(void);
	bool IsValidAck(void);
	bool IsAck(void);
	unsigned short GetDataNo(void);
	unsigned short GetAckNo(void);
	unsigned short GetAssignedPort(void);
	unsigned int GetStatus(void);
	struct timeval& GetTime(void){ return tmActionTime; }
protected:
	enum Symbol
	{
		SOH = 0x01,
		STX = 0x02,
		ETX = 0x03,
		EOT = 0x04,
		ENQ = 0x05,
		ACK = 0x06,
		NAK = 0x15,
		ETB = 0x17
	};

	string strCache;

	void slipToNextStartToken(void);
	bool isFrameCRCOK(void);
	bool isValidFrame(void);

	struct timeval tmActionTime;
};
}
#endif /* PACKET_H_ */
