/*
 * Channel.h
 *
 *  Created on: 2009-12-29
 *      Author: mxx
 */

#ifndef CHANNEL_H_
#define CHANNEL_H_
using namespace std;
#include <exception>
#include <string>
#include <errno.h>
#include <string.h>
namespace bitcomm
{
class ChannelException:public exception
{
public:
	ChannelException() {err=0;bUnConnected=true;};
	ChannelException(int errNo) {err=errNo;bUnConnected=false;};
	virtual ~ChannelException() throw(){};
	virtual const char* what()
			{ return strerror_r(err,cache,128);};

	int err;
	bool bUnConnected;
	char cache[128];
};

class Channel
{
public:
	Channel();
	virtual ~Channel();
	virtual int Open()=0;
	virtual int Read(char* buff,int len)=0;
	virtual int Write(const char* buff,int len)=0;
	virtual void Lock()=0;
	virtual void Unlock()=0;
};
}

#endif /* CHANNEL_H_ */