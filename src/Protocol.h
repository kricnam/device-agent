/*
 * Protocol.h
 *
 *  Created on: 2009-12-30
 *      Author: mxx
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

namespace bitcomm
{

class Protocol
{
public:
	Protocol();
	virtual ~Protocol();
	void RequestCurrentData(unsigned char Machine,Channel& port,Packet& data);
};

}

#endif /* PROTOCOL_H_ */
