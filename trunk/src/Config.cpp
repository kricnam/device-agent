/*
 * Config.cpp
 *
 *  Created on: 2010-1-9
 *      Author: mxx
 */

#include "Config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "DebugLog.h"
namespace bitcomm
{
int GetValue(const char* szFile,const char *szKey, char *pBuf, int nLen)
{
	FILE *confFile;
	int nRet,nLine,nCmp;
	int nszKey = strlen(szKey);
	char line[1024]={0};
	char szKeyCmp[1024]={0};
	char* moveEof=NULL;

	int nSpace=0;
	int nEnd=0;
	int i;

	pBuf[0]=0;

	if( (confFile  = fopen((const char *)szFile, "r" )) == NULL )
		nRet = -1;
	else
	{
		nRet = -2;
		while (!feof (confFile))
		{
			memset(line,0,1024);
			memset(szKeyCmp,0,1024);
			fgets(line, 1024,confFile);

			if (line[0]=='#')
				continue;

			nLine = strlen(line);
			if(line[nLine-1]=='\n')
				nEnd = 1;
			else
				nEnd=0;
			moveEof=strtok(line,"\n");
			if(moveEof!=NULL)
			{
				while(*moveEof == ' ')
				{
					nSpace++;
					moveEof--;
				}
			}
			if(nSpace!=0)
				nLine = nLine-nSpace-1;
			strcpy(szKeyCmp,szKey);
			strcat(szKeyCmp,"=");
			nCmp=strncmp(szKeyCmp,line,nszKey+1);
			if(!nCmp)
			{
				int temp = nLine-nszKey;
				if(nLen<temp)
					nRet = temp;
				else
				{
					if(nEnd>0)
					{
						for(i=0;i<temp-1;i++)
							pBuf[i]=line[i+nszKey+1];
					}
					else
					{
						for(i=0;i<temp;i++)
							pBuf[i]=line[i+nszKey+1];
					}
					nRet = 0;
				}
				break;
			}
		}
		fclose (confFile);
	}
	return nRet;
}

Config::Config(const char* szFileName)
{
	strFileName = szFileName;
}

Config::~Config()
{

}

string Config::GetServerName()
{
	char tmp[1024]={0};
	if (GetValue(strFileName.c_str(),"SERVER",tmp,1024)==0)
		return tmp;
	return "";
}

string Config::GetMPdev()
{
	char tmp[1024]={0};
	if (GetValue(strFileName.c_str(),"MP_PORT",tmp,1024)==0)
		return tmp;
	return "";

}

int Config::GetMachine()
{
	char tmp[1024]={0};
	if (GetValue(strFileName.c_str(),"MACHINE",tmp,1024)==0)
	{
		return atoi(tmp);
	}
	return 1;
}

int Config::GetModemDelay()
{
	char tmp[1024]={0};
	if (GetValue(strFileName.c_str(),"MODEM_DELAY",tmp,1024)==0)
	{
		return atoi(tmp);
	}
	return 60;
}

int Config::GetPowerOnDelay()
{
	char tmp[1024]={0};
	if (GetValue(strFileName.c_str(),"POWERON_DELAY",tmp,1024)==0)
	{
		return atoi(tmp);
	}
	return 90;
}

int Config::GetDataPort()
{
	char tmp[1024]={0};
	if (GetValue(strFileName.c_str(),"DATA_PORT",tmp,1024)==0)
	{
		return atoi(tmp);
	}
	return 50001;
}

int Config::GetTraceLevel(void)
{
	char tmp[1024]={0};
	if (GetValue(strFileName.c_str(),"LOG_LEVEL",tmp,1024)==0)
	{
		return atoi(tmp);
	}
	return LP_INFO;
}


int Config::GetCommandPort()
{
	char tmp[1024]={0};
	if (GetValue(strFileName.c_str(),"COMMAND_PORT",tmp,1024)==0)
	{
		return atoi(tmp);
	}
	return 50101;
}

string Config::GetAPN(void)
{
	char tmp[1024]={0};
	if (GetValue(strFileName.c_str(),"APN",tmp,1024)==0)
	{
		return tmp;
	}
	return "bgan.inmarsat.com";
}


string Config::GetIP(void)
{
	char tmp[1024]={0};
	if (GetValue(strFileName.c_str(),"LOCAL_IP",tmp,1024)==0)
	{
		return tmp;
	}
	return "";
}

string Config::GetModemIP(void)
{
	char tmp[1024]={0};
	if (GetValue(strFileName.c_str(),"MODEM_IP",tmp,1024)==0)
	{
		return tmp;
	}
	return "192.168.1.35";
}

string Config::GetUserName(void)
{
	char tmp[1024]={0};
	if (GetValue(strFileName.c_str(),"USERNAME",tmp,1024)==0)
	{
		return tmp;
	}
	return "";
}

float Config::GetMinSignalLevel()
{
	char tmp[1024]={0};
	if (GetValue(strFileName.c_str(),"MIN_SIGNAL",tmp,1024)==0)
	{
		return (float)atoi(tmp);
	}
	return (float)45;
}

int Config::GetIntervalSecond()
{
	char tmp[1024]={0};
	if (GetValue(strFileName.c_str(),"INTERVAL",tmp,1024)==0)
	{
		return atoi(tmp);
	}
	return 600;
}

string Config::GetPassword(void)
{
	char tmp[1024]={0};
	if (GetValue(strFileName.c_str(),"PASSWORD",tmp,1024)==0)
	{
		return tmp;
	}
	return "";
}

}
