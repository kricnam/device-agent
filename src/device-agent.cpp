//============================================================================
// Name        : device-agent.cpp
// Author      : Meng Xinxin
// Version     :
// Copyright   : No Warranty, No Right reserved
// Description : Hello World in C, Ansi-style
//============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "DataTask.h"
#include "ControlTask.h"
#include "SerialPort.h"
#include "Modem.h"

using namespace bitcomm;

int main(void) {
	//Initiate Serial Port
	SerialPort portMP;

	//Power on the EXP500,Make sure got a valid IP
	Modem exp500;

	//Negotiate DATA Channel
	//Negotiate CONTROL Channel
	//Get Current data
	//Send out data
	//Get status
	//send out status if necessary
	//Check CONTROL command
	//Process command if necessary
	//Power off EXP500 if necessary
	//read and store data and status, until EXP500 power on

	DataTask  dataProcess(portMP,exp500);
	ControlTask controlProcess(portMP,exp500);

	dataProcess.run();
	controlProcess.run();

	while(1) sleep(60);
	return EXIT_SUCCESS;
}
