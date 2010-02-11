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
#include <time.h>
using namespace std;
namespace bitcomm
{
enum CommunicationCommand
{
	DataRequest,
	MPHealthCheck,
	GetCondition,
	SetCondition,
	GetPreAmp,
	GetADCSetting,
	GetSpectrumSetting,
	GetNetworkSetting,
	SetLowHiDoesRateAlarmReset,
	SetTransmitUnitHardwareReset,
	SetTransmitUnitReset,
	SetTransmitUnitMemortClear,
	GetTime,
	SetTime,
	RequestDoseRate,
	ConfirmDoseRate,
	RequestSpectrum,
	ConfirmSpectrum,
	RequestDoseRateAlarm,
	ConfirmDoseRateAlarm,
	Request40KAction,
	Confirm40KAction,
	RequestGPS,
	ConfirmGPS,
	RequestDataCancel,
	DataTerminate,
	SetDataPort,
	SetControlPort,
	ControlPortHealthCheck,
	CMD_END
};

static const char* cmdWord[]={"ra","wa","ca","cA","cb","cc","cd","ce","cF","cH","cI","cJ",
"ck","cK","ha","hA","hc","hC","hd","hD","hf","hF","hg","hG","hy","hz","pA","pB","wb",""};

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
	}__attribute__ ((packed));

	struct GetTimeFrame
	{
		struct FrameHead head;
		char cmd[2];
		unsigned short length;
		struct FrameTime time;
		unsigned char second;
		struct FrameEnd end;
	} __attribute__ ((packed));

	const char* GetData() {return strCache.data();};
	int GetSize() 	{return strCache.size();};
	void Dump();
	void BuildPacket(const char* szContent,int size,unsigned char Machine);
	void SendTo(Channel& port);
	void ReceiveFrameFrom(Channel& port);
	void ReceiveAckFrom(Channel& port);
	bool IsAckNo(unsigned short n);
	bool IsValidStatus(void);
	bool IsValidAck(void);
	bool IsValidFrame(void) { return isFrameCRCOK();};
	bool IsAck(void);
	void SetPacket(const char* szBuf,int n);
	void Ack(bool bAck,char Machine,enum CommunicationCommand eCmd, short nNum=0);
	unsigned short GetDataNo(void);
	unsigned short GetAckNo(void);
	unsigned short GetAssignedPort(void);
	unsigned int GetStatus(void);
	time_t GetMPTime(void);

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


};
}
#endif /* PACKET_H_ */
