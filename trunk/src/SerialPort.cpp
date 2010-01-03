/*
 * SerialPort.cpp
 *
 *  Created on: 2009-12-28
 *      Author: mxx
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "SerialPort.h"
#include "unistd.h"
#include "stdio.h"
#include "termios.h"
using namespace std;
namespace bitcomm
{

SerialPort::SerialPort()
{
	handle = -1;
	timeout = 20000;
}

SerialPort::~SerialPort()
{
	if (handle>0)
		close(handle);
}

void SerialPort::Open(const char* szDev)
{
	//not reopen here
	if (handle>0) return;

	if (strDevName!=szDev)
		strDevName = szDev;

	handle = open(strDevName.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (handle<0)
		perror("SerialPort::Open");
	SetCom();
	return;
}

void SerialPort::SetCom(void)
{
	if (handle<0) return;

	struct termios newtio;
	tcflush(handle, TCIFLUSH);

	tcgetattr(handle, &newtio); /* save current port settings */
	/* set new port settings for canonical input processing */
	newtio.c_cflag = 0;
	newtio.c_cflag &= ~(CSIZE | PARENB);
	newtio.c_cflag |= B19200 | CS8 | CLOCAL | CREAD;

	newtio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG |IEXTEN);

	newtio.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);

	newtio.c_oflag &= ~OPOST;

	/* set input mode (non-canonical, no echo,...) */
	newtio.c_cc[VMIN] = 0;
	newtio.c_cc[VTIME] = 0;

	int n = tcsetattr(handle, TCSANOW, &newtio);
	if (n<0)
		perror("SerialPort::SetCom");
}

void SerialPort::Lock(void)
{

}

void SerialPort::Unlock(void)
{

}

int SerialPort::Read(char* buf,int len)
{
	if (len==0) return 0;
	if (handle==-1)
		Open(strDevName.c_str());
	int try_again = 2;
	int n;
	do
	{
		n = read(handle,buf,len);
		if (n>0) return n;
		if (n==-1)
		{
			perror("SerialPort::Read");
			return 0;
		}
		if (try_again--)
		{
			usleep(timeout);
			continue;
		}
	}while(false);
	return n;
}

int SerialPort::Write(const char* buf,int len)
{
	if (len==0) return 0;
	if (handle==-1)
		Open(strDevName.c_str());

	int n = write(handle,buf,len);
	tcdrain(handle);
	if (n>0) return n;
	if (n==-1)
		perror("SerialPort::Write");
	return 0;
}
}
