//-------------------------------------------------------
// Utility for initializing UI
//-------------------------------------------------------
#include "SLUtility.h"

const char*	kBasePath = ".\\Media\\scripts\\AttrTestMinigame";

void SLData::Init(const char* minigamename)
{
	m_MiniGame = minigamename;

	m_PlayerList.Clear();
	for(int i = 1; i <= kMaxPlayer; ++i)
	{
		char playerName[16];
		sprintf(playerName, "Player%d", i);

		char fullPath[128];
		sprintf(fullPath, "%s\\%s\\%s_%s.lua", kBasePath, minigamename, minigamename, playerName);

		if(FileManager::GetPtr()->IsFileExisted(fullPath))
		{
			m_PlayerList.PushBack(playerName);
		}
	}
}