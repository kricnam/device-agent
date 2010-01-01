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

void Protocol::RequestCurrentData(unsigned char Machine,Channel& port,Packet& data)
{
	CommunicationCommand request;
	request.setCommand(request.DataRequest,Machine);
	request.send(port);
	return data.Read(port);
}

}
