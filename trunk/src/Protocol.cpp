/*
 * Protocol.cpp
 *
 *  Created on: 2009-12-30
 *      Author: mxx
 */

#include "Protocol.h"

namespace bitcomm
{

Protocol::Protocol()
{
	// TODO Auto-generated constructor stub

}

Protocol::~Protocol()
{
	// TODO Auto-generated destructor stub
}

void Protocol::RequestCurrentData(Channel& port,Packet& data)
{
	Packet request;
	request.setCommand();
	request.Send(port);
	return data.Read(port);
}

}
