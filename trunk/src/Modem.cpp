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
#include "Config.h"
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
/* MODEM_H_ */
//send out Date,Time and Title message
namespace bitcomm
{
Modem::Modem()
{
	bPowerOff = true;
	Config config(CONF_FILENAME);
	UT_ATPort.SetRemoteHost(config.GetModemIP().c_str());
	UT_ATPort.SetRemotePort(9998);
}

Modem::~Modem()
{
}

void Modem::PowerOn(void)
{
	//Modemless test
	bPowerOff = false;
	return;
	///////////////////////////////
	bPowerOff = true;
	do
	{
		system("ifconfig eth0 up;sleep 1");
		//if (system("udhcpc -n -q") == -1)
		//{
		//	ERROR("fail to run DHCP client");
		//	break;
		//}

		try
		{
			int retry=3;
			do
			{
				UT_ATPort.SetTimeOut(5000000);

				if (UT_ATPort.Open())
				{
					ERROR("fail to connect to UT");
					continue;
				}
				INFO("Connected to AT command port");
				break;
			} while(--retry);
			if (retry==0) break;

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

			if (!LogonIP())
			{
				ERROR("fail to set context:%s",strCache.c_str());
				break;
			}

			if (!CheckContext())
			{
				ERROR("fail to check context:%s",strCache.c_str());
				break;
			}

			if (!ConnectIP())
			{
				ERROR("fail to connect:%s ",strCache.c_str());
				break;
			}
			//system("udhcpc -n -q");
			bPowerOff = false;
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

int Modem::LogonIP(void)
{
	char buf[256]={0};
	Config config(CONF_FILENAME);
	snprintf(buf,256,"AT+CGDCONT=10,\"IP\",\"%s\",\"%s\",0,0,\"%s\",\"%s\"\n",
			config.GetAPN().c_str(),config.GetIP().c_str(),
			config.GetUserName().c_str(),
			config.GetPassword().c_str());

	int retry=3;
	do
	{
		UT_ATPort.Write(buf,strlen(buf));
	    if (WaitATResponse("OK"))
	    	return 1;
	}
	while(--retry);
	return 0;
}

int Modem::CheckContext(void)
{
	int retry=3;
	Config config(CONF_FILENAME);
	do
	{
		UT_ATPort.Write("AT+CGDCONT?\n",12);
		if (WaitATResponse("OK"))
		{
			if (strCache.find(config.GetAPN())!=string::npos
					&& strCache.find(config.GetIP())!=string::npos)
			return 1;
		}
	}while(--retry);
	return 0;

	UT_ATPort.Write("AT_IAPPPCONNECT=1,10\n",21);
	return WaitATResponse("OK");

}

int Modem::ConnectIP(void)
{
	int retry=3;
	Config config(CONF_FILENAME);
	do
	{
		UT_ATPort.Write("AT_IAPPPCONNECT=1,10\n",21);
		if (WaitATResponse("OK",config.GetModemDelay()))
			return 1;
	}while(--retry);
	return 0;
}


int Modem::UT_PowerOn()
{
	int ret = 0;
	int retry=3;
	Config config("./agent.conf");
	int waitTime = config.GetModemDelay();
	do
	{
		UT_ATPort.Write("AT_IPOINT=1\n",13);
		ret = WaitATResponse("OK",waitTime);
	} while (--retry);
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

	int retry=3;
	do
	{
		try
		{
			//UT_ATPort.Open();
			do
			{
				UT_ATPort.Write("AT_IAPPPCONNECT=0\n",19);
				if (WaitATResponse("OK"))
					break;
			} while(--retry);
			break;
		}
		catch (ChannelException& e)
		{
			ERROR(e.what());
		}
	} while (--retry);

	//system("udhcpc -n -q");
	if (system("ifconfig eth0 down"))
	{
		ERRTRACE();
	}
}

int Modem::WaitATResponse(const char *szWait, int timeout,bool bClear)
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
		if (diff.tv_sec > timeout)
			return 0;
	} while (1);
}

int Modem::AttachNet()
{
	int retry=3;
	do
	{
		UT_ATPort.Write("AT+CGATT?\n",10);
		if (WaitATResponse("+CGATT:1,3"))
			return 1;
	}while(--retry);
	return 0;
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
