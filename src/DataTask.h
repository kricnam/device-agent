/*
 * DataTask.h
 *
 *  Created on: 2009-12-31
 *      Author: mxx
 */

#ifndef DATATASK_H_
#define DATATASK_H_
#include "pthread.h"
#include "SerialPort.h"
#include "Modem.h"
namespace bitcomm
{

class DataTask
{
public:
	DataTask(SerialPort& port,Modem& m);
	virtual ~DataTask();
	void run(void);
	static void* doProcess(void* pThis);
protected:
	SerialPort& portMP;
	Modem& modem;
	pthread_t pidTask;
};

}

#endif /* DATATASK_H_ */
