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
#include "Protocol.h"
#include <string>
using namespace std;
namespace bitcomm
{

class DataTask
{
public:
	DataTask(Protocol& p,Modem& modem);
	virtual ~DataTask();
	void run(void);
	static void* doProcess(void* pThis);
	static void* doReadData(void* pThis);
	void SaveData();
	Protocol& protocol;
	DataPacketQueue dataQueue;
protected:
	Modem& modem;
	pthread_t pidTask;
	string strServer;

};

}

#endif /* DATATASK_H_ */
