//============================================================================
// Name        : device-agent.cpp
// Author      : Meng Xinxin
// Version     :
// Copyright   : No Warranty, No Right reserved
// Description : Hello World in C, Ansi-style
//============================================================================
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <linux/input.h>
#include "DataTask.h"
#include "ControlTask.h"
#include "SerialPort.h"
#include "Modem.h"
#include "Config.h"
#include "DebugLog.h"

using namespace bitcomm;

void InitPort(void)
{
	system("echo 69 > /sys/class/gpio/export;"\
			"echo 72 > /sys/class/gpio/export;"\
			"echo 79 > /sys/class/gpio/export;"\
			"echo 81 > /sys/class/gpio/export;"\
			"echo 85 > /sys/class/gpio/export;"\
			"echo in > /sys/class/gpio/gpio69/direction;"\
			"echo in > /sys/class/gpio/gpio72/direction;"\
			"echo low >/sys/class/gpio/gpio79/direction;"\
			"echo low >/sys/class/gpio/gpio81/direction;"\
			"echo in >/sys/class/gpio/gpio85/direction");
}


int main(void) {
	InitPort();
	INFO("Open configure file agent.conf");
	Config config("./agent.conf");
	Modem exp500;

	Protocol protocol(config.GetServerName().c_str());
	INFO("Set server as: %s",protocol.GetServerName());
	protocol.SetMachine(config.GetMachine());
	INFO("Set machine No. as: %d",protocol.GetMachine());

	protocol.GetMPPort().Open(config.GetMPdev().c_str());

	INFO("Set system clock...");
	time_t Now = protocol.GetMPTime();
	if (Now)
	{
		INFO("Set time to %s",ctime(&Now));
		struct timeval tmNow;
		tmNow.tv_sec = Now;
		tmNow.tv_usec = 0;
		if (settimeofday(&tmNow,0))
		{
			ERRTRACE();
		}

		system("hwclock -w");
	}

	DataTask  dataProcess(protocol,exp500);
	ControlTask controlProcess(protocol,exp500);



	dataProcess.run();
	//controlProcess.doProcess(&controlProcess);

	int fd = ::open("/dev/event0",O_RDONLY );
	struct input_event event;
	while(1)
	{
		sleep(1);
		read(fd,&event,sizeof(event));
		TRACE("value=%d",event.value);
		TRACE("SHUT DOWN\n");
		close(fd);
		system("reboot");
	};

	return EXIT_SUCCESS;
}
