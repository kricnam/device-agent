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
#include "Protocol.h"
#include "pthread.h"
namespace bitcomm
{

class ControlTask
{
public:
	ControlTask(Protocol& p,Modem& m);
	virtual ~ControlTask();
	void run(void);
	static void* doProcess(void* pThis);
protected:

	Protocol& protocol;
	Modem& modem;
	pthread_t pidTask;

};

}

#endif /* CONTROLTASK_H_ */
