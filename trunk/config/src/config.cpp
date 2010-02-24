//============================================================================
// Name        : config.cpp
// Author      : Meng Xinxin
// Version     :
// Copyright   : No Warrinty, No Right reserved
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../../device-agent/src/Config.h"
using namespace std;
using namespace bitcomm;

string strMsg;
char buf[1024]={0};
Config cDef("");

void outItem(const char* szName,const char* szVal,const char* szDefault,const char* szDesc);
void outItem(const char* szName,int nVal,int nDefault,const char* szDesc);
void save_config(const char* szBuf);

void urldecode(const char *src, const char *last, char *dest)
{
 for(; src != last; src++, dest++)
   if(*src == '+')
     *dest = ' ';
   else if(*src == '%') {
     int code;
     if(sscanf(src+1, "%2x", &code) != 1) code = '?';
     *dest = code;
     src +=2; }
   else
     *dest = *src;
 *dest = '\n';
 *++dest = '\0';
}

void get_key(const char* szKey,string& strVal,const char* szBuf)
{
	strVal.clear();
	const char* szFound = strstr(szBuf,szKey);
	if (szFound)
	{
		char buf[1024]={0};
		const char* end = strstr(szFound,"&");

		if (!end) end = szBuf + strlen(szBuf);
		urldecode(szFound+strlen(szKey)+1,end,buf);
		strVal = buf;
	}
}

int main() {

	long len = 0;
	bool reboot = false;



	char* lenstr = getenv("CONTENT_LENGTH");
	if(lenstr == NULL || sscanf(lenstr,"%ld",&len)!=1 || len > 1024)
	{
		if (lenstr)
		{
			strMsg = "Content length error:";
			strMsg += lenstr;
		}
	}
	else
	{
		fgets(buf, len+1, stdin);
	}

	if (strlen(buf))
	{
		string strReboot;
		get_key("Submit",strReboot,buf);
		if (strstr(buf,"reboot"))
		{
			strMsg = "System will reboot in 5 seconds.";
			reboot = true;

		}
		else
			save_config(buf);
	}


	Config conf("/app/bin/agent.conf");

	cout << "Content-type: text/xml" << endl << endl;
	cout << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << endl;
	cout << "<?xml-stylesheet type=\"text/xsl\" href=\"config.xsl\"?>" << endl;
	cout << "<device_agent_config>" << endl;

	cout << "<prompt>" << strMsg << "</prompt>" << endl;

	outItem("SERVER", conf.GetServerName().c_str(),
			cDef.GetServerName().c_str(), "Server address");
	outItem("DATA_PORT",conf.GetDataPort(),cDef.GetDataPort(), "TCP port for data transmit");
	outItem("COMMAND_PORT",conf.GetCommandPort(),cDef.GetCommandPort(), "TCP port for control command");
	outItem("INTERVAL",conf.GetIntervalSecond(),cDef.GetIntervalSecond(), "communication interval in seconds");

	outItem("MP_PORT", conf.GetMPdev().c_str(), cDef.GetMPdev().c_str(),
			"device file name for MP");
	outItem("MACHINE", conf.GetMachine(), cDef.GetMachine(), "Part No. of  MP");
	outItem("APN", conf.GetAPN().c_str(), cDef.GetAPN().c_str(),
			"satellite access APN");
	outItem("LOCAL_IP", conf.GetIP().c_str(), cDef.GetIP().c_str(),
			"public IP for satellite modem");
	outItem("MODEM_IP", conf.GetModemIP().c_str(), cDef.GetModemIP().c_str(),
			"satellite modem access IP");
	outItem("USERNAME", conf.GetUserName().c_str(), cDef.GetUserName().c_str(),
			"satellite modem user name");
	outItem("PASSWORD", conf.GetPassword().c_str(), cDef.GetPassword().c_str(),
			"access password");
	outItem("MODEM_DELAY", conf.GetModemDelay(), cDef.GetModemDelay(),
			"modem at command response waiting second");
	outItem("MIN_SINGAL", conf.GetMinSignalLevel(), cDef.GetMinSignalLevel(),
			"minimum satellite signal level");
	outItem("POWERON_DELAY", conf.GetPowerOnDelay(), cDef.GetPowerOnDelay(),
				"waiting seconds for MP self test");

	cout << "</device_agent_config>" << endl;

	if (reboot) system("reboot -d 10 &");
	return 0;
}


void print_config(FILE* fp,const char*szKey,const char* szDef,const char* szDesc)
{
	string strVal;
	get_key(szKey,strVal,buf);
	if (strVal.empty()) strVal = szDef;
	fprintf(fp,"#%s\n%s=%s\n",szDesc,szKey,strVal.c_str());
}

void print_config(FILE* fp,const char*szKey,int nDef,const char* szDesc)
{
	string strVal;
	get_key(szKey,strVal,buf);
	if (strVal.empty())
	{
		fprintf(fp,"#%s\n%s=%d\n",szDesc,szKey,nDef);
	}
	else
		fprintf(fp,"#%s\n%s=%s\n",szDesc,szKey,strVal.c_str());
}

void save_config(const char* szBuf)
{
	FILE* fp;
	string strVal;
	fp = fopen("/app/bin/agent.conf","w");
	if (fp)
	{
		print_config(fp,"SERVER",cDef.GetServerName().c_str(), "Server address");
		print_config(fp,"DATA_PORT",cDef.GetDataPort(), "TCP port for data transmit");
		print_config(fp,"COMMAND_PORT",cDef.GetCommandPort(), "TCP port for control command port");
		print_config(fp,"INTERVAL",cDef.GetIntervalSecond(), "communication interval in seconds");
		print_config(fp,"MP_PORT",cDef.GetMPdev().c_str(),"device file name for MP");
		print_config(fp,"MACHINE",cDef.GetMachine(), "Part No. of  MP");
		print_config(fp,"APN", cDef.GetAPN().c_str(), "satellite access APN");
		print_config(fp,"LOCAL_IP", cDef.GetIP().c_str(),"public IP for satellite modem");
		print_config(fp,"MODEM_IP", cDef.GetModemIP().c_str(),"satellite modem access IP");
		print_config(fp,"USERNAME",cDef.GetUserName().c_str(),"satellite modem user name");
		print_config(fp,"PASSWORD", cDef.GetPassword().c_str(),"access password");
		print_config(fp,"MODEM_DELAY",cDef.GetModemDelay(),"modem AT command response waiting second");
		print_config(fp,"MIN_SINGAL",cDef.GetMinSignalLevel(),"minimum satellite signal level");
		print_config(fp,"POWERON_DELAY", cDef.GetPowerOnDelay(),"waiting second for MP self test");
		fclose(fp);
		time_t now;
		time(&now);
		strMsg = "Parameters saved at I/F UNIT local time:";
		strMsg += ctime(&now);
		strMsg += ". Please press reboot button to make them effect ";
	}
	else
	{
		strMsg = "open file error";
	}
}



void outItem(const char* szName,const char* szVal,const char* szDefault,const char* szDesc)
{
	cout << "<item>" << endl;
	cout << "<key>" << szName << "</key>" << endl;
	cout << "<value>" << szVal << "</value>" << endl;
	cout << "<default>" << szDefault << "</default>" << endl;
	cout << "<descript>" << szDesc << "</descript>" << endl;
	cout << "</item>" << endl;
}

void outItem(const char* szName,int nVal,int nDefault,const char* szDesc)
{
	cout << "<item>" << endl;
	cout << "<key>" << szName << "</key>" << endl;
	cout << "<value>" << nVal << "</value>" << endl;
	cout << "<default>" << nDefault << "</default>" << endl;
	cout << "<descript>" << szDesc << "</descript>" << endl;
	cout << "</item>" << endl;
}
