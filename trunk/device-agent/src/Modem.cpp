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
	bool bModemOnFirstTime = true;
	bool bOpenContext = true;

	int retry = 3;

	try
	{
		do
		{
			if (UT_ATPort.Open())
			{
				ERROR("fail to connect to Unit SABRE1");
				sleep(2);
				continue;
			}
			INFO("Connected to AT command port");
			break;
		} while (--retry);

		if (retry == 0)
			return;

		sleep(5);
		do
		{
			if (!bModemOpen)
			{
				if (bModemOnFirstTime)
				{
					TRACE("Check AT Port, and disable echo back");
					UT_ATPort.Write("ATE0\r\n", 6);
					WaitATResponse("OK", 5);
				}

				if (!CheckSignalLevel())
				{
					ERROR("Signal level too low");
					break;
				}

				if (bModemOnFirstTime)
				{
					 if (!CheckContext())
					 {
						if (!CheckSignalLevel())
						{
							ERROR("Signal level too low");
							break;
						}
					 }
					 else
					 {
						 bOpenContext = false;
					 }
				}

				if (bOpenContext)
				{
					if (!UT_PowerOn())
					{
						ERROR("fail to active  Unit SABRE1");
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
				}

				if (!CheckSignalLevel())
				{
					ERROR("Signal level too low");
					break;
				}
			}
			else
			{

				if (!CheckContext())
				{
					ERROR("fail to check context:%s",strCache.c_str());
					//UT_ATPort.Close();
					bModemOpen = false;
					bModemOnFirstTime = false;
					INFO("Modem reconnecting...");
					continue;
				}
			}

			sleep(5);
			if (!ConnectIP())
			{
				ERROR("fail to connect:%s ",strCache.c_str());
				break;
			}

			bModemOpen = true;

			UT_ATPort.Close();

			bPowerOff = false;
			return;
		} while (1);

		//ONERROR:
		UT_Reset();
		UT_ATPort.Close();
		bModemOpen = false;
		return;
	} catch (ChannelException& e)
	{
		ERROR(e.what());
		bModemOpen = false;
		UT_ATPort.Close();
		return;
	}
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
	int waitTime = 5;//config.GetModemDelay();

	sleep(1);
	do
	{
		TRACE("Confirm registration information:AT+CGDCONT?");
		UT_ATPort.Write("AT+CGDCONT?\r\n",13);
		strCache.clear();
		if (WaitATResponse("OK",waitTime))
		{
			if (strCache.find(config.GetUserName())!=string::npos
					&& strCache.find(config.GetIP()) != string::npos)
				return 1;
			else
				return 0;
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
		if (strCache.find("ERROR:")!=string::npos)
			return 0;
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

	int retry=5;
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
			int n = UT_ATPort.Read(buf, 256);
			if (n > 0)
			{
				if (n < 255) buf[n]=0;
				TRACE("Expect:%s [R->%s",szWait,buf);
				strCache.append(buf, n);
			}
			if (strCache.find(szWait) != string::npos)
				return 1;
			if (strCache.find("ERROR:")!=string::npos)
				return 0;

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
	int waitTime = 5;//config.GetModemDelay();

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
	Config config("./agent.conf");
	float minSIG = config.GetMinSignalLevel();
	int maxCheckTime = config.GetModemDelay();
	int ret = 0;
    INFO("Checking signal level...");
	if (!OpenSignalLevel())
	{
		return 0;
	}

	struct timeval now, start, diff;
	gettimeofday(&start, 0);

	do
	{
		if (!(GetSignalLevel() < minSIG))
		{
			ret = 1;
			break;
		}
		else
			strCache.clear();

		gettimeofday(&now, 0);
		timersub(&now,&start,&diff);

	}while(diff.tv_sec < maxCheckTime);

	if (!CloseSignalLevel())
	{
		return 0;
	}

	return ret;
}

float Modem::GetSignalLevel(void)
{
	if (WaitATResponse("_ISIG:", 10,false))
	{
		if (WaitATResponse("\n", 1,false))
		{
			string::size_type pos = strCache.find("_ISIG:");
			string::size_type end = strCache.find("\n", pos + 6);
			string Level = strCache.substr(pos + 6, end);
			strCache.erase(0, end);
			INFO("Get Signal level:%s",Level.c_str());
			return atof(Level.c_str());
		}
	}
	return 0.0;
}

}
