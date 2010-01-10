/*
 * Protocol.h
 *
 *  Created on: 2009-12-30
 *      Author: mxx
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_
#include "PacketQueue.h"
#include "Packet.h"
#include "TCPPort.h"
#include "CommunicationCommand.h"
#include "HistoryDataRequestCmd.h"
#include <string>
#include <list>
#include <sys/time.h>
namespace bitcomm
{

class Protocol
{
public:
	Protocol(const char* szServer)
	{
		strServerName = szServer;
		nIdleTimeSetting = 60;
		nIntervalSetting = 600;
	};
	virtual ~Protocol();
	void RequestCurrentData(Channel& port,Packet& data);
	void SendCurrentData(Channel& port, DataPacketQueue& queue);
	void NegoiateDataChannel(TCPPort& port);
	void NegoiateControlChannel(TCPPort& port);
	int Sleep();
	void HealthCheck(Channel& dev,Packet& status);
	void HealthCheckReport(Channel& port,Packet& status);
	enum CommunicationCommand GetCommand(Channel& port,CmdPacket& cmd);
	void TransferCmd(Channel& dev,Channel& port,CmdPacket& cmd);
	void HistoryDataTransfer(Channel& dev,Channel& port,HistoryDataRequestCmd& cmd);
	void SendQueueData(DataPacketQueue& queue,Channel& port);
	bool IsTimeForSleep(void);
	void SleepForPowerOn(void);
	void PatrolRest(void);
    bool getExtCommunicationError() const
    {
        return bExtCommunicationError;
    }

    unsigned char getMachine() const
    {
        return Machine;
    }

    void setExtCommunicationError(bool bExtCommunicationError)
    {
        this->bExtCommunicationError = bExtCommunicationError;
    }

    void setMachine(unsigned char Machine)
    {
        this->Machine = Machine;
    }

protected:

	int negoiateChannel(TCPPort& port,int nStartPort);
	void setReservedTime(struct timeval& timer,int sec, bool bAlign=true);
	bool isTimeForAction(struct timeval& timer);
	void setLastActionTime(struct timeval& tm)
	{
		if (timercmp(&tm,&tmLastActive,>))
				tmLastActive = tm;
	};
	bool bExtCommunicationError;
	bool bInCommunicationError;
	unsigned char Machine;
	string strServerName;
	unsigned int nLastStatus;
	int nIdleTimeSetting;
	int nIntervalSetting;
	struct timeval tmLastActive;
	struct timeval tmHealthCheckActive;
	struct timeval tmCurrentDataActive;
};

}

#endif /* PROTOCOL_H_ */
