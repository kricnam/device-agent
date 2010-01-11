/*
 * DebugLog.cpp
 *
 *  Created on: 2010-1-11
 *      Author: mxx
 */

#include "DebugLog.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <iostream>
#include <string>
#include <sys/time.h>
#include <unistd.h>
#define MAXBUFSIZE 1024

namespace bitcomm
{
TraceLog gTraceLog;

static const string priorityNames[] =
	{ "Zero Priority", "TRACE", "DEBUG", "INFO", "NOTICE", "WARNING", "ERROR",
			"CRITICAL ERROR", "ALERT", "EMERGENCY" };

int TraceLog::sct = -1;
int TraceLog::nLevel = 0;
bool TraceLog::bLocal = true;
int TraceLog::nCounter = 0;
int TraceLog::nLogPort = 0;
struct sockaddr_in TraceLog::sin = {0};
char TraceLog::szLogIP[50]={0};
string TraceLog::strTitle = "";

TraceLog::TraceLog()
{
}
TraceLog::~TraceLog()
{
	if (sct > 0)
		shutdown(sct, SHUT_RD);
}

TraceLog::TraceLog(const char* szIP, int nPort)
{
	sct = -1;
	nLevel = 0;
	bLocal = true;
	nCounter = 0;
	Init("", szIP, nPort);
}

TraceLog::TraceLog(const char* szTitle, const char* szIP, int nPort)
{
	sct = -1;
	nLevel = 0;
	bLocal = true;
	nCounter = 0;
	Init(szTitle, szIP, nPort);
}

void  TraceLog::Init(const char* Title, const char* szIP, int nPort)
{
	nLogPort = nPort;
	strncpy(szLogIP, szIP, 49);

	strTitle = Title;
	if (sct != -1)
		close( sct);
	sct = socket(AF_INET, SOCK_DGRAM, 0);
	if (socket < 0)
	{
		perror("TraceLog::Init on socket");
	}
	memset(&sin, 0x0, sizeof(sin));

	sin.sin_family = AF_INET;
	sin.sin_port = htons(nLogPort);
	sin.sin_addr.s_addr = inet_addr(szLogIP);
	TRACE(0, "[%s] Log to %s:%d", Title, szLogIP, nLogPort);
}

void TraceLog::SetTitle(char* Buf, int nSize)
{
	struct tm Time =
	{ 0 };
	struct timeval tv =
	{ 0 };
	struct timezone tz =
	{ 0 };

	gettimeofday(&tv, &tz);

	localtime_r(&tv.tv_sec, &Time);
	snprintf(Buf, nSize, "\n%02d-%02d %02d:%02d:%02d.%03d %s %04d ",
			Time.tm_mon + 1, Time.tm_mday, Time.tm_hour, Time.tm_min,
			Time.tm_sec, (int)tv.tv_usec / 1000, strTitle.c_str(), nCounter);
	nCounter = (nCounter > 9998) ? 0 : nCounter + 1;
}

void TraceLog::SendOut(const char* szBuf)
{
	register int n = 0;
	if (bLocal)
	{
		if (puts(szBuf + 1) == EOF || fflush(stdout))
		{
			perror("TraceLog::SendOut:puts");
			clearerr(stdout);
		}
	}
	if (sct < 0 && !bLocal)
	{
		if (fputs(szBuf, stderr) == EOF || fflush(stderr))
		{
			perror("TraceLog::SendOut:fputs");
			clearerr(stderr);
		}
	}
	else
	{
		n = sendto(sct, szBuf, strlen(szBuf), MSG_DONTWAIT,
				(struct sockaddr *) & sin, sizeof(sin));
		if (-1 == n)
		{
			perror("\nTraceLog::SendOut:sendto");
			fputs(szBuf + 1, stderr);
			fputs("\n", stderr);
		}
	}
}

void TraceLog::Trace(int nLev, const char* szFmt, ...)
{
	if (!IsInit())
	{
		fputs("TraceLog Object not initiated.\n"
			"Please call TraceLogInit(Title,IP,Port) to initiate first.\n"
			"look tracelog.h for further reference.", stderr);
		exit(1);
	}
	if (nLev > nLevel)
		return;
	va_list ap;
	int n;
	int size = 256;
	char* pBuf = NULL;
	char szBuf[80] =
	{ 0 };
	if ((pBuf = (char*) malloc(size)) == NULL)
	{
		pBuf = szBuf;
		size = 80;
	}

	SetTitle(pBuf, size);
	int used = strlen(pBuf);
	while (1)
	{
		/* Try to print in the allocated space. */
		va_start(ap, szFmt);
		n = vsnprintf(pBuf + used, size - used, szFmt, ap);
		va_end(ap);

		/* If that worked, return the string. */
		if (80 == size || (n > -1 && n < size - used) || size == MAXBUFSIZE)
		{
			SendOut(pBuf);
			if (80 != size)
				free(pBuf);
			return;
		}
		/* Else try again with more space. */
		char* ptmp = pBuf;
		if (n > -1) /* glibc 2.1 */
			size = min(used + n + 1, MAXBUFSIZE); /* precisely what is needed */
		else
			/* glibc 2.0 */
			size = min(size + 80, MAXBUFSIZE); /* each time increase 80 bytes */
		if ((pBuf = (char*) realloc(pBuf, size)) == NULL)
		{
			//if failed extending, just send already ones
			SendOut(ptmp);
			free(ptmp);
			return;
		}
	}
}

}
