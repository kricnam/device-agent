/*
 * PacketQueue.h
 *
 *  Created on: 2009-12-31
 *      Author: mxx
 */

#ifndef PACKETQUEUE_H_
#define PACKETQUEUE_H_

namespace bitcomm
{

class PacketQueue
{
public:
	PacketQueue();
	virtual ~PacketQueue();
	void push(Packet& data);
protected:

};

}

#endif /* PACKETQUEUE_H_ */
