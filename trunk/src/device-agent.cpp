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
	int n  = system("echo 72 > /sys/class/gpio/export;"\
			"echo 79 > /sys/class/gpio/export;"\
			"echo 81 > /sys/class/gpio/export;"\
			"echo 85 > /sys/class/gpio/export;"\
			"echo in > /sys/class/gpio/gpio72/direction;"\
			"echo low >/sys/class/gpio/gpio79/direction;"\
			"echo low >/sys/class/gpio/gpio81/direction;"\
			"echo in > /sys/class/gpio/gpio85/direction");
	DEBUG("system return %d",n);
}


int main(void) {
	InitPort();
	INFO("Open configure file agent.conf");
	Config config("./agent.conf");
	Modem exp500;

	exp500.strAPN = config.GetAPN();
	exp500.strIP = config.GetIP();
	exp500.strUserName = config.GetUserName();
	exp500.strPassword = config.GetPassword();

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

		DEBUG("hwclock return %d",system("hwclock -w"));
	}

	DataTask  dataProcess(protocol,exp500);
	ControlTask controlProcess(protocol,exp500);

	dataProcess.run();
	controlProcess.run();

	int fd = open("/dev/event0",O_RDONLY );
	if (fd<0)
	{
		ERRTRACE();
		INFO("fail safe, set Power Off OK, return %d",
				system("echo high >/sys/class/gpio/gpio81/direction;"));
		while(1)
		{
			sleep(60);
		};
	}

	struct input_event event;
	while(1)
	{
		sleep(1);
		if (read(fd,&event,sizeof(event))<0)
			ERRTRACE();
		TRACE("value=%d",event.value);
		TRACE("SHUT DOWN\n");
		close(fd);
		if (system("reboot"))
		{
			ERRTRACE();
		}
	};

	return EXIT_SUCCESS;
}
