#include "TScript_Machine.h"
#include "TScript_API.h"
#include "TScript_Function.h"
#include "TEngine_ScriptModule.h"
#include "TFS_FileManager.h"

namespace TsiU
{
	ScriptModule::ScriptModule()
	{
		m_poMachine = new ScriptMachine;
	}

	ScriptModule::~ScriptModule()
	{
		D_SafeDelete(m_poMachine);
	}

	void ScriptModule::Init()
	{
		m_poMachine->Init();

		ScriptFuncInfo tmpSFI;
		ScriptFunction* poSF = NULL;
		//Register Function
		s32 iIdx = 0;
		while(1)
		{
			tmpSFI = g_API_Def[iIdx];
			if(tmpSFI.iFuncIdx < 0)
				break;
			else if(tmpSFI.iFuncIdx == 0)
				continue;

			poSF = new ScriptFunction(m_poMachine, &tmpSFI);
			if(poSF->RegisterFunction())
			{
				m_aFunctionList.insert(std::pair<std::string, ScriptFunction*>(tmpSFI.strFuncName, poSF));
			}
			else
			{
				D_Output("<Script> Register %s Failed\n", tmpSFI.strFuncName.c_str());
			}
			iIdx++;
		}

		/*File *poFile = FileManager::GetPtr()->CreateFile(".\\Script\\main.lua", E_FOM_Read|E_FOM_Text);
		m_uiBufferSize = poFile->Size();
		m_poMainBuffer = new Char[m_uiBufferSize];
		poFile->Read(m_poMainBuffer, m_uiBufferSize);
		FileManager::GetPtr()->DestroyFile(poFile);*/
	}

	void ScriptModule::RunOneFrame(float _fDeltaTime)
	{
		//m_poMachine->DoBuffer(m_poMainBuffer, m_uiBufferSize, "Main");
	}
	void ScriptModule::UnInit()
	{
		m_poMachine->Uninit();
	}
}