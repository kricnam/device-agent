/*
 * CRC16.h
 *
 *  Created on: 2009-12-28
 *      Author: mxx
 */

#ifndef CRC16_H_
#define CRC16_H_

namespace bitcomm
{

class CRC16
{
public:
	static unsigned short crc16(unsigned short start, unsigned char* data,int len);
	static unsigned short crc16(unsigned short start, unsigned char* data,int offset,int len);
	static inline unsigned short crc16_byte(unsigned char data, unsigned short crc)
	{
	    return crc_table_itut[(crc ^ data) & 0x00ff] ^ (crc>>8);
	}
private:
	static  const unsigned short crc_table_itut[];
};

}

#endif /* CRC16_H_ */
