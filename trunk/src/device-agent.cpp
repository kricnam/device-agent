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
#include <sys/wait.h>
#include "DataTask.h"
#include "ControlTask.h"
#include "SerialPort.h"
#include "Modem.h"
#include "Config.h"
#include "DebugLog.h"

using namespace bitcomm;

void InitPort(void)
{
	//echo 72 > /sys/class/gpio/export;"
	//"echo in > /sys/class/gpio/gpio72/direction;"
	int n  = system("echo 79 > /sys/class/gpio/export;"\
			"echo 81 > /sys/class/gpio/export;"\
			"echo 85 > /sys/class/gpio/export;"\
			"echo low >/sys/class/gpio/gpio79/direction;"\
			"echo low >/sys/class/gpio/gpio81/direction;"\
			"echo in > /sys/class/gpio/gpio85/direction");
	DEBUG("system return %d",n);
}

int check_shutdown()
{
	int fd = open("/sys/class/gpio/gpio72/value",O_RDONLY );
	if (fd<0) return 0;
	char c[2]={0};
	if (read(fd,c,1)<1)
	{
		ERRTRACE();
	}
	close(fd);
	INFO("read shutdown value: %s",c);
	return atoi(c);
}

int main_work(void) {
	INFO("Open configure file agent.conf");
	Config config("./agent.conf");
	int n = config.GetTraceLevel();

	SETTRACELEVEL(n);
	int nDelay = config.GetPowerOnDelay();
	InitPort();
	system("echo high >/sys/class/gpio/gpio81/direction;");//Enable Power off

	system("ifconfig eth0 up;sleep 1;route add default gw 192.168.1.35 ");

	Modem exp500;

	exp500.strAPN = config.GetAPN();
	exp500.strIP = config.GetIP();
	exp500.strUserName = config.GetUserName();
	exp500.strPassword = config.GetPassword();

	Protocol protocol(config.GetServerName().c_str());
	INFO("Set server as: %s",protocol.GetServerName());
	protocol.SetMachine(config.GetMachine());
	INFO("Set machine No. as: %d",protocol.GetMachine());
	n = config.GetDataPort();
	INFO("Set Data Port as: %d",n);
	protocol.SetDataPort(n);
	n = config.GetCommandPort();
	INFO("Set Command Port as: %d",n);
	protocol.SetCommandPort(n);

	n = config.GetIntervalSecond();
	INFO("Set interval time as: %d second",n);
	protocol.SetIntervalSecond(n);

	protocol.LoadStatus();
	protocol.GetMPPort().Open(config.GetMPdev().c_str());

	INFO("Set system clock...");
	time_t Now = protocol.GetMPTime();
	if (!Now)
	{
		INFO("Waiting %d seconds for Monitoring Post self check over...",nDelay);
		sleep(nDelay);
		Now = protocol.GetMPTime();//try again
	}

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
	system("echo low >/sys/class/gpio/gpio81/direction;");//Disable Power off
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
		INFO("Monitoring Power Off signal...");
		if (read(fd, &event, sizeof(event)) < 0)
		{
				ERRTRACE();
				INFO("fail safe, set Power Off OK, return %d",
								system("echo high >/sys/class/gpio/gpio81/direction;"));
				close(fd);
				while(1)
						{
							sleep(60);
						};
		}
		DEBUG("event %d,%d,%d",event.type,event.code,event.value);
		if (event.type == EV_KEY && event.code == BTN_0 && event.value == 0)
		{
			TRACE("SHUT DOWN\n");
			close(fd);
			dataProcess.SaveData();
			if (system("reboot"))
			{
				ERRTRACE();
				return -1;
			}
			break;
		}
	};

	return EXIT_SUCCESS;
}

int main()
{
    while (1)
	{
		pid_t child = fork();

		if (child == 0)
		{
			return main_work();
		}

		if (child == -1)
			ERROR("fail to create working child" );
		else
		{
			waitpid(child, NULL, 0);
			INFO("App Working process exit,Restart after 10 second");
			system("reboot");
			return 0;
		}
	}
	return -1;
}

