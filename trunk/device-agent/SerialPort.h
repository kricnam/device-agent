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
	int Open(const char* szDev);
	void Close();
	void SetCom(void);
	virtual int Open(void) {return Open(strDevName.c_str());};
	virtual int Read(char* buf,int len);
	virtual int Write(const char* buf,int len);
	virtual void Lock(void);
	virtual void Unlock(void);
	virtual bool IsOpen() { return handle == -1;};
	const char* GetPort(void) {return strDevName.c_str();};
	virtual void SetTimeOut(int usec) {timeout= usec;};
protected:
	int handle;
	int timeout;
	string strDevName;
};
}

#endif /* SERIALPORT_H_ */
