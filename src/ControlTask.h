/*
 * ControlTask.h
 *
 *  Created on: 2009-12-31
 *      Author: mxx
 */

#ifndef CONTROLTASK_H_
#define CONTROLTASK_H_

namespace bitcomm
{

class ControlTask
{
public:
	ControlTask();
	virtual ~ControlTask();
	void run(void);
	static void* doProcess(void* pThis);
protected:
	SerialPort& portMP;
	Modem& modem;
};

}

#endif /* CONTROLTASK_H_ */
