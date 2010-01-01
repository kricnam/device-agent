/*
 * Packet.h
 *
 *  Created on: 2009-12-28
 *      Author: mxx
 */

#ifndef PACKET_H_
#define PACKET_H_
#include <string>
using namespace std;
namespace bitcomm
{
class Packet {
public:
	Packet();
	virtual ~Packet();
	void setCommand(const char szCmd);
	void buildPacket(const char* szContent,int size,unsigned char Machine);
	void send(Channel& port);
protected:
	enum Symbol
	{
		SOH = 0x01,
		STX = 0x02,
		ETX = 0x03,
		EOT = 0x04,
		ENQ = 0x05,
		ACK = 0x06,
		NAK = 0x15,
		ETB = 0x17
	};

	string strCache;
};
}
#endif /* PACKET_H_ */
