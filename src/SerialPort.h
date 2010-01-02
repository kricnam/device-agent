/*
 * SerialPort.h
 *
 *  Created on: 2009-12-28
 *      Author: mxx
 */

#ifndef SERIALPORT_H_
#define SERIALPORT_H_
#include "Channel.h"
namespace bitcomm
{
class SerialPort :public virtual Channel{
public:
	SerialPort();
	virtual ~SerialPort();
	void Open(const char* szDev);
	virtual int Read(const char* buf,int len);
	virtual int Write(const char* buf,int len);
	virtual void Lock(void);
	virtual void Unlock(void);
};
}

#endif /* SERIALPORT_H_ */
