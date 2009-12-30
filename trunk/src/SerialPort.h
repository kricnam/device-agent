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
};
}

#endif /* SERIALPORT_H_ */
