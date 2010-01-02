/*
 * SerialPort.h
 *
 *  Created on: 2009-12-28
 *      Author: mxx
 */

#ifndef SERIALPORT_H_
#define SERIALPORT_H_
#include "Channel.h"
#include <string>
using namespace std;
namespace bitcomm
{
class SerialPort :public virtual Channel{
public:
	SerialPort();
	virtual ~SerialPort();
	void Open(const char* szDev);
	void SetCom(void);
	virtual int Read(const char* buf,int len);
	virtual int Write(const char* buf,int len);
	virtual void Lock(void);
	virtual void Unlock(void);
protected:
	int handle;
	int timeout;
	string strDevName;
};
}

#endif /* SERIALPORT_H_ */
