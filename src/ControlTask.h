/*
 * ControlTask.h
 *
 *  Created on: 2009-12-31
 *      Author: mxx
 */

#ifndef CONTROLTASK_H_
#define CONTROLTASK_H_
#include "Modem.h"
#include "SerialPort.h"

namespace bitcomm
{

class ControlTask
{
public:
	ControlTask(SerialPort& port,Modem& m);
	virtual ~ControlTask();
	void run(void);
	static void* doProcess(void* pThis);
protected:
	SerialPort& portMP;
	Modem& modem;
};

}

#endif /* CONTROLTASK_H_ */
