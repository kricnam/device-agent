/*
 * Channel.h
 *
 *  Created on: 2009-12-29
 *      Author: mxx
 */

#ifndef CHANNEL_H_
#define CHANNEL_H_

namespace bitcomm
{
class Channel
{
public:
	Channel();
	virtual ~Channel();
	virtual int Read(const char* buff,int len)=0;
	virtual int Write(const char* buff,int len)=0;
	virtual void Lock()=0;
	virtual void Unlock()=0;
};
}

#endif /* CHANNEL_H_ */
