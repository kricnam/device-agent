/*
 * Modem.cpp
 *
 *  Created on: 2009-12-31
 *      Author: mxx
 */

#include "Modem.h"

namespace bitcomm
{

Modem::Modem()
{
}

Modem::~Modem()
{
}

void Modem::PowerOn(void)
{
	bPowerOff = false;
}

bool Modem::IsPowerOff(void)
{
	return bPowerOff;
}

void Modem::PowerOff(void)
{
	bPowerOff = true;
}

}
