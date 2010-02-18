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
	bModemOpen = false;
}

Modem::~Modem()
{
}

void Modem::PowerOn(void)
{
	///////////////////////////////
	bPowerOff = true;

	do
	{
		int retry = 3;
		do
		{
			if (UT_ATPort.Open())
			{
				ERROR("fail to connect to UT");
				continue;
			}
			INFO("Connected to AT command port");
			break;
		} while (--retry);

		if (retry == 0)
			break;

		try
		{
			if (!bModemOpen)
			{
				sleep(5);
				TRACE("Check AT Port, and disable echo back");
				UT_ATPort.Write("ATE0\r\n", 6);
				WaitATResponse("OK", 10);

				if (!CheckSignalLevel())
				{
					ERROR("Signal level too low");
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
				bModemOpen = true;
			}
			else
			{
				if (!CheckSignalLevel())
				{
					ERROR("Signal level too low");
					break;
				}

				if (!CheckContext())
				{
					ERROR("fail to check context:%s",strCache.c_str());
					UT_ATPort.Close();
					bModemOpen = false;
					continue;
				}
			}

			if (!ConnectIP())
			{
				ERROR("fail to connect:%s ",strCache.c_str());
				break;
			}

			UT_ATPort.Close();
			sleep(5);
			bPowerOff = false;
			return;
		}
		catch (ChannelException& e)
		{
			ERROR(e.what());
			bModemOpen = false;
			UT_ATPort.Close();
		}
		break;
	} while (1);

	if (strCache.find("CME ERROR")!=string::npos)
	{
		UT_Reset();
		bModemOpen = false;
		return;
	}

	PowerOff();
	return;
}

void Modem::UT_Reset()
{
	int ret = 0;
	UT_ATPort.Write("AT+CFUN=1,1\r\n",13);
	ret = WaitATResponse("OK",5);
}

int Modem::LogonIP(void)
{
	char buf[256]={0};
	Config config(CONF_FILENAME);
	sleep(1);
	snprintf(buf,256,"AT+CGDCONT=10,\"IP\",\"%s\",\"%s\",0,0,\"%s\",\"%s\"\r\n",
			config.GetAPN().c_str(),config.GetIP().c_str(),
			config.GetUserName().c_str(),
			config.GetPassword().c_str());


	int retry=3;
	do
	{
		INFO("Registration:%s",buf);
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
	int waitTime = config.GetModemDelay();

	sleep(1);
	do
	{
		TRACE("Confirm registration information:AT+CGDCONT?");
		UT_ATPort.Write("AT+CGDCONT?\r\n",13);
		strCache.clear();
		if (WaitATResponse("OK",waitTime))
		{
			if (strCache.find(config.GetAPN())!=string::npos
					&& strCache.find(config.GetIP())!=string::npos)
			return 1;
		}
	}while(--retry);
	return 0;
}

int Modem::ConnectIP(void)
{
	int retry=3;
	Config config(CONF_FILENAME);
	strCache.clear();
	do
	{
		INFO("Line connect:AT_IAPPPCONNECT=1,10");
		UT_ATPort.Write("AT_IAPPPCONNECT=1,10\r\n",22);
		if (WaitATResponse("OK",config.GetModemDelay(),true))
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
		INFO("AT_IPOINT=1");
		UT_ATPort.Write("AT_IPOINT=1\r\n",14);
		ret = WaitATResponse("OK",waitTime);
		if (ret) break;
	} while (--retry);
	return ret;
}

bool Modem::IsPowerOff(void)
{
	return bPowerOff;
}

void Modem::PowerOff(void)
{
	INFO("Modem disconnect");
	bPowerOff = true;
//return;
	int retry=3;
	do
	{
		try
		{
			UT_ATPort.Open();
			do
			{
				TRACE("AT_IAPPPCONNECT=0");
				UT_ATPort.Write("AT_IAPPPCONNECT=0\r\n",20);
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
	UT_ATPort.Close();
}

int Modem::WaitATResponse(const char *szWait, int timeout,bool bClear)
{
	char buf[256];
	if (bClear)
		strCache.clear();

	UT_ATPort.SetTimeOut(timeout*1000000);
	struct timeval now, start, diff;
	gettimeofday(&start, 0);
	do
	{
		try
		{
			int n = UT_ATPort.Read(buf, 256);
			if (n > 0)
			{
				if (n < 255) buf[n]=0;
				TRACE("Expect:%s [R->%s",szWait,buf);
				strCache.append(buf, n);
			}
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
	Config config("./agent.conf");
	int waitTime = config.GetModemDelay();

	do
	{
		UT_ATPort.Write("AT+CGATT?\r\n",11);
		if (WaitATResponse("OK",waitTime))
		{
			if (strCache.find("+CGATT: 1,3")!=string::npos)
				return 1;
		}
		sleep(20);
	}while(--retry);
	return 0;
}

int Modem::OpenSignalLevel()
{
	int retry=3;
	do
	{
		INFO("AT_ISIG=1");
		UT_ATPort.Write("AT_ISIG=1\r\n",11);
		if (WaitATResponse("OK",5))
		{
				return 1;
		}
	}while(--retry);
	return 0;
}

int Modem::CloseSignalLevel()
{
	int retry=3;
	do
	{
		INFO("AT_ISIG=0");
		UT_ATPort.Write("AT_ISIG=0\r\n",11);
		if (WaitATResponse("OK",5))
		{
			return 1;
		}
	}while(--retry);
	return 0;

}

int Modem::CheckSignalLevel()
{
	int retry=3;
	Config config("./agent.conf");
	float minSIG = config.GetMinSignalLevel();
	int ret = 0;

	if (!OpenSignalLevel())
	{
		return 0;
	}

	do
	{
		if (!(GetSignalLevel() < minSIG) &&
				!(GetSignalLevel() < minSIG) &&
				!(GetSignalLevel() < minSIG))
		{
			ret = 1;
			break;
		}
		else
			strCache.clear();
	}while(--retry);


	if (!CloseSignalLevel())
	{
		return 0;
	}

	return ret;
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