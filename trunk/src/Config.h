/*
 * Config.h
 *
 *  Created on: 2010-1-9
 *      Author: mxx
 */

#ifndef CONFIG_H_
#define CONFIG_H_
#include <string>

using namespace std;

namespace bitcomm
{
#define CONF_FILENAME "./agent.conf"

class Config
{
public:
	Config(const char* szFile);
	virtual ~Config();
	string GetServerName();
	string GetMPdev();
	string GetAPN();
	string GetUserName();
	string GetPassword();
	string GetIP();
	string GetModemIP();
	int GetDataPort();
	int GetCommandPort();
	int GetMachine();
	int GetModemDelay();
	int GetTraceLevel();
	int GetPowerOnDelay();
	int GetIntervalSecond();
	int GetIdelSecond();
	float GetMinSignalLevel();
protected:
	string strFileName;
};

}

#endif /* CONFIG_H_ */
