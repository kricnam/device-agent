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

class Config
{
public:
	Config(const char* szFile);
	virtual ~Config();
	string GetServerName();
	string GetMPdev();
	int GetMachine();
protected:
	string strFileName;
};

}

#endif /* CONFIG_H_ */
