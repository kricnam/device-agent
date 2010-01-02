/*
 * Protocol.cpp
 *
 *  Created on: 2009-12-30
 *      Author: mxx
 */

#include "Protocol.h"
#include "CommunicationCommand.h"
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
	port.Lock();
	CommunicationCommand request;

	request.SetCommand(request.DataRequest,Machine);
	request.SendTo(port);

	data.ReceiveFrameFrom(port);
	port.Unlock();
	return;
}

void Protocol::SendCurrentData(Channel& port, DataPacketQueue& queue)
{
	if (queue.GetSize()>0)
	{
		struct DataPacketFrame& packet=queue.Front();
		port.Write((char*)&packet,sizeof(struct DataPacketFrame));
	}
}

void Protocol::HealthCheck(unsigned char Machine,Channel& dev,Channel& port,Packet& status)
{

}

int Protocol::Sleep(void)
{
	return 0;
}
}
