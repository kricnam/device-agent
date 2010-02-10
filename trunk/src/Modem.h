/*
 * Modem.h
 *
 *  Created on: 2009-12-31
 *      Author: mxx
 */

#ifndef MODEM_H_
#define MODEM_H_
#include "TCPPort.h"
namespace bitcomm
{

class Modem
{
public:
	Modem();
	virtual ~Modem();
	void PowerOff(void);
	void PowerOn(void);
	bool IsPowerOff(void);
	string strAPN;
	string strUserName;
	string strIP;
	string strPassword;
protected:
	int UT_PowerOn(void);
	int WaitATResponse(const char* szWait,int timeout=5, bool bClearCache=true);
	int AttachNet(void);
	int LogonIP(void);
	int CheckContext(void);
	int ConnectIP(void);
	float GetSignalLevel(void);
	bool bPowerOff;
	string strCache;
	TCPPort UT_ATPort;

};

}

#endif /* MODEM_H_ */
