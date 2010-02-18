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
#include "SerialPort.h"
#include "CommunicationCommand.h"
#include "HistoryDataRequestCmd.h"
#include "Modem.h"
#include <string>
#include <list>
#include <sys/time.h>
#include <time.h>

namespace bitcomm
{

class Protocol
{
public:
	Protocol(const char* szServer)
	{
		strServerName = szServer;
		nIdleTimeSetting = 20;
		nIntervalSetting = 600;
		gettimeofday(&tmCurrentDataActive,0);
		tmHealthCheckActive = tmCurrentDataActive;
		tmLastActive = tmCurrentDataActive;
		tmSendDataActive = tmCurrentDataActive;
		nLastStatus = 0;
		bExtCommunicationError = false;
		bInCommunicationError = false;
		nCommandPort = 50101;
		nDataPort = 50001;
		nNegotialTimeOut = 20; //seconds

		bControlDone =false;
	};
	virtual ~Protocol();
	void RequestCurrentData(Channel& port,Packet& data);
	void SendCurrentData(Modem& modem,Channel& port, DataPacketQueue& queue);
	int NegoiateDataChannel(Modem& modem,TCPPort& port);
	void NegoiateControlChannel(TCPPort& port);
	int Sleep();
	void HealthCheck(Channel& dev,Packet& status);
	void HealthCheckReport(Channel& port);
	enum CommunicationCommand GetCommand(Channel& port,CmdPacket& cmd);
	void TransferCmd(Channel& dev,Channel& port,CmdPacket& cmd);
	void HistoryDataTransfer(Channel& dev,Channel& port,HistoryDataRequestCmd& cmd);
	void SendQueueData(DataPacketQueue& queue,Channel& port);
	bool IsTimeForSleep(void);
	void SleepForPowerOn(void);
	void PatrolRest(void);
	time_t GetMPTime(void);
	int GetMPIntervalSecond(void);
	void SaveStatus() { if (statusQueue.GetSize()) statusQueue.Save("./status.data");};
	void LoadStatus() { statusQueue.Load("./status.data");};
    void SetCommandPort(int nCommandPort)
    {
        this->nCommandPort = nCommandPort;
    }

    void SetDataPort(int nDataPort)
    {
        this->nDataPort = nDataPort;
    }

    bool GetExtCommunicationError()
    {
        return bExtCommunicationError;
    };

    unsigned char GetMachine()
    {
        return Machine;
    };

    void SetExtCommunicationError(bool bExtCommunicationError)
    {
        this->bExtCommunicationError = bExtCommunicationError;
    };

    void SetMachine(unsigned char Machine)
    {
        this->Machine = Machine;
    };

    void SetIdleTimeSecond(int n)
    {
    	nIdleTimeSetting = n;
    };

    void SetIntervalSecond(int n)
    {
    	nIntervalSetting = n;
    }

    TCPPort& GetControlPort()
    {
    	return srvControl;
    };

    TCPPort& GetDataPort()
    {
    	return srvData;
    };

    SerialPort& GetMPPort()
    {
    	return devMP;
    };

    const char* GetServerName()
	{
    	return strServerName.c_str();
	};

    int GetIdleTime()
    {
    	return nIdleTimeSetting;
    };

    void SetControlDone(bool b)
    {
    	bControlDone = b;
    };
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
	bool bControlPre;
	bool bControlDone;
	unsigned char Machine;
	string strServerName;
	unsigned int nLastStatus;
	int nIdleTimeSetting;
	int nIntervalSetting;
	struct timeval tmLastActive;
	struct timeval tmHealthCheckActive;
	struct timeval tmCurrentDataActive;
	struct timeval tmSendDataActive;
	SerialPort devMP;
	TCPPort srvData;
	TCPPort srvControl;
	DataPacketQueue statusQueue;
	int nCommandPort;
	int nDataPort;
	int nNegotialTimeOut;
};

}

#endif /* PROTOCOL_H_ */
