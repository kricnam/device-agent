/*
 * Modem.h
 *
 *  Created on: 2009-12-31
 *      Author: mxx
 */

#ifndef MODEM_H_
#define MODEM_H_

namespace bitcomm
{

class Modem
{
public:
	Modem();
	virtual ~Modem();
	void PowerOff(void);
	void PowerOn(void);
	bool IsPowerOff(void);
protected:
	bool bPowerOff;
};

}

#endif /* MODEM_H_ */
