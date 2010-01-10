/*
 * CommunicationCommand.h
 *
 *  Created on: 2009-12-28
 *      Author: mxx
 */

#ifndef COMMUNICATIONCOMMAND_H_
#define COMMUNICATIONCOMMAND_H_
#include "Packet.h"
#include <string>
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

class CmdPacket:public virtual Packet
{
public:
	CmdPacket();
	virtual ~CmdPacket();

	void SetCommand(const char* szCmd,unsigned char Machine);
	enum CommunicationCommand CommandType();
	static bool IsCommand(const char* szCmd,enum CommunicationCommand n)
	{
		if (n>CMD_END) return false;
		if (szCmd==NULL) return false;
		return szCmd[0]==cmdWord[n][0] && szCmd[1]==cmdWord[n][1];
	}
};

}

#endif /* COMMUNICATIONCOMMAND_H_ */
