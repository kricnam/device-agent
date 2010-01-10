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
#include "Config.h"
using namespace bitcomm;

int main(void) {
	//Initiate Serial Port
	SerialPort portMP;
	Modem exp500;

	Config config("./agent.conf");

	Protocol protocol(config.GetServerName().c_str());
	protocol.setMachine(config.GetMachine());
	DataTask  dataProcess(protocol,portMP,exp500);
	ControlTask controlProcess(portMP,exp500,protocol);

	dataProcess.run();
	controlProcess.doProcess(&controlProcess);

	return EXIT_SUCCESS;
}
