#include "VMCommand.h"

//-------------------------------------------------------------------------------------
Bool VMCommandCenter::RegisterCommand(StringPtr _strCmdName, VMCommand::VMCommandHandler _pHandler, const VMCommand::ParamList*	_defaultParamList)
{
	if(_strCmdName == NULL || !strcmp(_strCmdName, ""))
	{
		D_Output("[VMCommandCenter::RegisterCommand] invalid command name\n");
		return false;
	}
	VMCommandMapIterator it = m_CommandMap.find(_strCmdName);
	if(it != m_CommandMap.end())
	{
		D_Output("[VMCommandCenter::RegisterCommand] duplicate command name\n");
		return false;
	}
	VMCommand* newCommand = new VMCommand(_pHandler, _defaultParamList);
	m_CommandMap.insert(std::pair<std::string, VMCommand*>(_strCmdName, newCommand));
	return true;
}
//#pragma optimize("", off)
Bool VMCommandCenter::ExecuteFromString(StringPtr _cmd)
{
	Char* token = NULL;
	Char tmp[VMCommand::kMaxCommandLength];
	strncpy(tmp, _cmd, VMCommand::kMaxCommandLength);
	token = strtok(tmp, " ");
	VMCommandMapIterator it = m_CommandMap.find(token);
	if(it == m_CommandMap.end())
	{
		D_Output("[VMCommandCenter::ExecuteFromString] unknown command\n");
		return false;
	}
	VMCommand& rc = *((*it).second);
	VMCommand::ParamList paramList;
	while(1)
	{
		token = strtok(NULL, " ");
		if(!token)
			break;

		VMCommandParamHolder vmParam;
		vmParam.Set(token);
		paramList.PushBack(vmParam);
	}
	s32 iRet = rc.Execute(paramList);
	if(iRet)
	{
		D_Output("[VMCommandCenter::Parse] Error occurs when executing command\n");
		return false;
	}
	return true;
}