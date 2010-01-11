/*
 * DebugLog.h
 *
 *  Created on: 2010-1-11
 *      Author: mxx
 */
////////////////////////////////////////////////////////
// Defind the common interface for trace and log
// To use trace, must use TraceLogInit() to setup
// message title, IP, and port
// then can use TRACE() to output
// TRACE() is the same format as printf()
// Example:
// ...
// TraceLogInit("TEST","127.0.0.1",1234); //output to localhost:1234
// SETTRACELEVEL(1);       //Only level lower or equal than 1 is out
//...
//
// TRACE(0,"This is a test message"); //output level 0 message
//				      // It's the always output message
// ...
// -------------------------------------------------------
// the output just like:
// 2008-11-06 14:31:58 TEST This is a test message
//
//
// Also can use the class methods to form your own message
// a global variable gTraceLog for class TraceLog exist already
//
// /////////////////////////////////////////////////////

#ifndef DEBUGLOG_H_
#define DEBUGLOG_H_
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <string>
using namespace std;
namespace bitcomm
{
struct Location
{
	const char* szFile;
	const char* szFunc;
	const char* szLine;
};

class TraceLog
{
public:

	TraceLog(void);
	TraceLog(const char* szIP,int nPort);
	TraceLog(const char* szT,const char* szIP,int nPort);
	~TraceLog();

	void Trace(int nLevel, const char* szFmt,...);

	static void Init(const char* Title,const char* szIP, int nPort);
	static inline bool IsInit() {return  sct!=-1;}
	static inline void SetLocalOut(bool bEnable) {bLocal = bEnable;}
	static void SetTraceLevel(int n) { nLevel = n;}
protected:
	static void SetTitle(char* szBuf,int nSize);		 //send out Date,Time and Title message
	void SendOut(const char* szBuf);

	static int nLogPort;
	static struct sockaddr_in sin;
	static int sct;
	static string strTitle;
	static char szLogIP[50];

	static string priorityNames[];
	static int nLevel;
	static bool bLocal;
	static int nCounter;
};

}


#define TraceLogInit(x,y,z)	gTraceLog.Init(x,y,z)
#define TRACE 			gTraceLog.Trace
#define SETLOCALOUT(x)		gTraceLog.SetLocalOut(x)
#define SETTRACELEVEL(x)	gTraceLog.SetTraceLevel(x)
#define ERRTRACE(x)	{char szErrorTraceStrBuffer[128];\
				strerror_r(errno,szErrorTraceStrBuffer,128);\
				gTraceLog.Trace(0,#x":%s",szErrorTraceStrBuffer);}


#endif /* DEBUGLOG_H_ */
