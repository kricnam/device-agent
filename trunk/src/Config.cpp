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
}
