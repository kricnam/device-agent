/*
 * Modem.cpp
 *
 *  Created on: 2009-12-31
 *      Author: mxx
 */

#include "Modem.h"
#include "DebugLog.h"
namespace bitcomm
{

Modem::Modem()
{
	bPowerOff = true;
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
	INFO("Power off");
	bPowerOff = true;
}

}
