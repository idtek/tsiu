#include "VMCommand.h"

//-------------------------------------------------------------------------------------
Bool VMCommandCenter::RegisterCommand(StringPtr _strCmdName, VMCommand::VMCommandHandler _pHandler, VMCommand::EParamType _param1, VMCommand::EParamType _param2)
{
	if(_strCmdName == NULL || !strcmp(_strCmdName, ""))
	{
		D_Output("[VMCommandCenter::RegisterCommand] invalid command name\n");
		return false;
	}
	if(_param2 != VMCommand::EParamType_None && _param1 == VMCommand::EParamType_None)
	{
		D_Output("[VMCommandCenter::RegisterCommand] invalid parameter type\n");
		return false;
	}
	VMCommandMapIterator it = m_CommandMap.find(_strCmdName);
	if(it != m_CommandMap.end())
	{
		D_Output("[VMCommandCenter::RegisterCommand] duplicate command name\n");
		return false;
	}
	VMCommand* newCommand = new VMCommand(_pHandler, _param1, _param2);
	m_CommandMap.insert(std::pair<std::string, VMCommand*>(_strCmdName, newCommand));
	return true;
}

Bool VMCommandCenter::ExecuteFromString(StringPtr _cmd)
{
	VMCommandParamHolder param[VMCommand::kMaxParameterCount];
	Char* token = NULL;
	Char tmp[512];
	strncpy(tmp, _cmd, 512);
	token = strtok(tmp, " ");
	if(token != NULL)
	{
		VMCommandMapIterator it = m_CommandMap.find(token);
		if(it == m_CommandMap.end())
		{
			D_Output("[VMCommandCenter::ExecuteFromString] unknown command\n");
			return false;
		}
		VMCommand& rc = *((*it).second);
		for(s32 i = 0; i < VMCommand::kMaxParameterCount; ++i)
		{
			if(rc.GetParamType(i) != VMCommand::EParamType_None)
			{
				token = strtok(NULL, " ");
				if(token != NULL)
				{
					if(rc.GetParamType(i) == VMCommand::EParamType_Int)
						param[i].Set<s32>(atoi(token));
					else if(rc.GetParamType(i) == VMCommand::EParamType_Float)
						param[i].Set<f32>((f32)atof(token));
					else if(rc.GetParamType(i) == VMCommand::EParamType_String)
						param[i].Set<Char*>(token);
				}
				else
				{
					D_Output("[VMCommandCenter::Parse] invalid param count\n");
					return false;
				}
			}
			else{
				param[i].Set<s32>(0);
			}
		}
		s32 iRet = rc.Execute(param[0], param[1]);
		if(iRet)
		{
			D_Output("[VMCommandCenter::Parse] Error occurs when executing command\n");
			return false;
		}
		return true;
	}
	return false;
}