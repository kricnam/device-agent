/*
 * TCPPort.h
 *
 *  Created on: 2009-12-30
 *      Author: mxx
 */

#ifndef TCPPORT_H_
#define TCPPORT_H_

#include "Channel.h"
namespace bitcomm
{
class TCPPort: public virtual Channel
{
public:
	TCPPort();
	virtual ~TCPPort();
	int Open(const char* szServer);
	virtual int Read(const char* buf,int len);
	virtual int Write(const char* buf,int len);
	virtual void Lock(void);
	virtual void Unlock(void);
};
}
#endif /* TCPPORT_H_ */
