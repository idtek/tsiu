#ifndef __SLUTILITY_H__
#define __SLUTILITY_H__

#include <string>
#include "SLGlobalDef.h"

extern const char*	kBasePath;

class SLData
{
	static const int	kMaxPlayer = 10;

public:
	void Init(const char* minigamename);

public:
	std::string			m_MiniGame;
	Array<std::string>	m_PlayerList;
};


#endif