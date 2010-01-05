/*
 * HistoryDataRequestCmd.h
 *
 *  Created on: 2010-1-5
 *      Author: mxx
 */

#ifndef HISTORYDATAREQUESTCMD_H_
#define HISTORYDATAREQUESTCMD_H_

#include "CommunicationCommand.h"

namespace bitcomm
{

class HistoryDataRequestCmd: public virtual bitcomm::CmdPacket
{
public:
	struct HistoryDataRequestFrame
	{
		struct FrameHead head;
		char cmd[2];
		unsigned short length;
		unsigned short startNo;
		unsigned short endNo;
		unsigned short dataNum;
		struct FrameEnd end;
	} __attribute__ ((packed));

	HistoryDataRequestCmd();
	virtual ~HistoryDataRequestCmd();
	int GetStartNum(void)
	{
		struct HistoryDataRequestFrame* p =(struct HistoryDataRequestFrame*)GetData();
		return p->startNo;
	};
	int GetEndNum(void)
	{
		struct HistoryDataRequestFrame* p =(struct HistoryDataRequestFrame*)GetData();
		return p->endNo;
	};
	int GetDataNumber(void)
	{
		struct HistoryDataRequestFrame* p =(struct HistoryDataRequestFrame*)GetData();
		return p->dataNum;
	};
	void SetStartNum(unsigned short n)
	{
		struct HistoryDataRequestFrame* p =(struct HistoryDataRequestFrame*)GetData();
		p->startNo = n;
	};

};

}

#endif /* HISTORYDATAREQUESTCMD_H_ */
