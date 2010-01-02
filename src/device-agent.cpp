//============================================================================
// Name        : device-agent.cpp
// Author      : Meng Xinxin
// Version     :
// Copyright   : No Warrinty, No Right reserved
// Description : Hello World in C, Ansi-style
//============================================================================

#include <stdio.h>
#include <stdlib.h>

#include "DataTask.h"
#include "ControlTask.h"
#include "SerialPort.h"
#include "Modem.h"

using namespace bitcomm;

int main(void) {
	//TODO: Initiate Serial Port
	//TODO: Power on the EXP500
	//TODO: Make sure got a valid IP
	//TODO: Negotiate DATA Channel
	//TODO: Negotiate CONTROL Channel
	//TODO: Get Current data
	//TODO: Send out data
	//TODO: Get status
	//TODO: send out status if necessary
	//TODO: Check CONTROL command
	//TODO: Process command if necessary
	//TODO: Power off EXP500 if necessary
	//TODO: read and store data and status, until EXP500 power on
	SerialPort portMP;
	Modem sat;
	DataTask  dataProcess(portMP,sat);
	ControlTask controlProcess(portMP,sat);

	dataProcess.run();
	controlProcess.run();

	return EXIT_SUCCESS;
}
