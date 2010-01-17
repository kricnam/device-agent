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
	INFO("Open configure file agent.conf");
	Config config("./agent.conf");
	Modem exp500;

	Protocol protocol(config.GetServerName().c_str());
	INFO("Set server as: %s",protocol.GetServerName());
	protocol.SetMachine(config.GetMachine());
	INFO("Set machine No. as: %d",protocol.GetMachine());

	protocol.GetMPPort().Open(config.GetMPdev().c_str());

	DataTask  dataProcess(protocol,exp500);
	ControlTask controlProcess(protocol,exp500);



	//dataProcess.run();
	//while(1){sleep(1000);};
	controlProcess.doProcess(&controlProcess);

	return EXIT_SUCCESS;
}
