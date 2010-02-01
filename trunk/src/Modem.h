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
protected:
	int UT_PowerOn(void);
	int WaitATResponse(const char* szWait,bool bClearCache=true);
	int AttachNet(void);
	int EstablishIPConnection(void);
	float GetSignalLevel();
	bool bPowerOff;
	string strCache;
	TCPPort UT_ATPort;
};

}

#endif /* MODEM_H_ */
