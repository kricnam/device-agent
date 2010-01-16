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
#include "DebugLog.h"
using namespace bitcomm;

int main(void) {
	//Initiate Serial Port
	TRACE("Init Serial port");

	Modem exp500;

	Config config("./agent.conf");

	Protocol protocol(config.GetServerName().c_str());
	protocol.SetMachine(config.GetMachine());
	DataTask  dataProcess(protocol,exp500);
	ControlTask controlProcess(protocol,exp500);

	dataProcess.run();
	controlProcess.doProcess(&controlProcess);

	return EXIT_SUCCESS;
}
