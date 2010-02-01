/*
 * Modem.cpp
 *
 *  Created on: 2009-12-31
 *      Author: mxx
 */

#include "Modem.h"
#include "DebugLog.h"
#include "TCPPort.h"
#include "DebugLog.h"
#include <sys/time.h>
#include <stdlib.h>
/* MODEM_H_ */
//send out Date,Time and Title message
namespace bitcomm
{
Modem::Modem()
{
	bPowerOff = true;
	UT_ATPort.SetRemoteHost("192.168.1.35");
	UT_ATPort.SetRemotePort(9998);
}

Modem::~Modem()
{
}

void Modem::PowerOn(void)
{
	bPowerOff = false;
	do
	{
		if (system("udhcpc -n -q") == -1)
		{
			ERROR("fail to run DHCP client");
			break;
		}

		try
		{
		if (UT_ATPort.Open() < 0)
		{
			ERROR("fail to connect to UT");
			break;
		}

		if (!UT_PowerOn())
		{
			ERROR("fail to active  UT");
			break;
		}

		if (!AttachNet())
		{
			ERROR("Attach fail:%s",strCache.c_str());
			break;
		}

		if (!EstablishIPConnection())
		{
			ERROR("fail %s",strCache.c_str());
			break;
		}

		system("udhcpc -n -q");
		return;
		}
		catch(ChannelException& e)
		{
			ERROR(e.what());
		}
	} while (0);
	PowerOff();
	return;
}

int Modem::EstablishIPConnection(void)
{
	UT_ATPort.Write("AT+CGDCONT=10,\"IP\",\"bgan.inmarsat.com\"\n",39);
	WaitATResponse("OK");
	UT_ATPort.Write("AT_IAPPPCONNECT=1,10\n",21);
	return WaitATResponse("OK");
}

int Modem::UT_PowerOn()
{
	int ret = 0;
	struct timeval start, now, diff;
	UT_ATPort.Write("AT_ISIG=1\n", 10);
	if (!WaitATResponse("OK"))
	{
		ERROR("UT response time out");
		return 0;
	}
	gettimeofday(&start, 0);
	do
	{
		int n = GetSignalLevel();
		if (n > 40)
		{
			UT_ATPort.Write("AT_IPOINT=1\n",13);
			WaitATResponse("OK");
			ret = 1;
			break;
		}
		gettimeofday(&now, 0);
		timersub(&now,&start,&diff);
		if (diff.tv_sec > 10)
			break;
	} while (1);
	UT_ATPort.Write("AT_ISIG=0\n", 10);
	WaitATResponse("OK");
	return ret;
}

bool Modem::IsPowerOff(void)
{
	return bPowerOff;
}

void Modem::PowerOff(void)
{
	INFO("Power off");
	bPowerOff = true;
	do
	{
		try
		{
			UT_ATPort.Open();
			UT_ATPort.Write("AT_IAPPPCONNECT=0\n",19);
			WaitATResponse("OK");
			break;
		} catch (ChannelException& e)
		{
			ERROR(e.what());
		}
	} while (1);

	system("udhcpc -n -q");
	system("ifconfig eth0 down");
}

int Modem::WaitATResponse(const char *szWait, bool bClear)
{
	char buf[256];
	if (bClear)
		strCache.clear();

	struct timeval now, start, diff;
	gettimeofday(&start, 0);
	do
	{
		try
		{
			int n = UT_ATPort.Read(buf, 256);
			if (n > 0)
				strCache.append(buf, n);
			if (strCache.find(szWait) != string::npos)
				return 1;
		} catch (ChannelException& e)
		{
			ERROR("%s",e.what());
			return 0;
		}
		gettimeofday(&now, 0);
		timersub(&now,&start,&diff);
		if (diff.tv_sec > 3)
			return 0;
	} while (1);
}

int Modem::AttachNet()
{
	UT_ATPort.Write("AT+CGATT?\n",10);
	return WaitATResponse("+CGATT: 1,3");
}

float Modem::GetSignalLevel(void)
{
	if (WaitATResponse("_ISIG:", false))
	{
		string::size_type pos = strCache.find("_ISIG:");
		string::size_type end = strCache.find("\n", pos + 6);
		string Level = strCache.substr(pos + 6, end);
		strCache.erase(0, end);
		return atof(Level.c_str());
	}
	return 0.0;
}

}
