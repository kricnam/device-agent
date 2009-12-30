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
};
}
#endif /* TCPPORT_H_ */
