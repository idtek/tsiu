#ifndef __VM_COMMAND_H__
#define __VM_COMMAND_H__

#include "VMGlobalDef.h"
#include <string>
#include <map>

class VMCommandParamHolder
{
	typedef union
	{
		Char*	sVal;
	} ParamValue;

public:
	VMCommandParamHolder()
	{
		memset(&m_Param, 0, sizeof(ParamValue));
	}
	VMCommandParamHolder(Char* _val)
	{
		Set(_val);
	}

	s32			ToInt()		const	{ return atoi(m_Param.sVal);	}
	f32			ToFloat()	const	{ return atof(m_Param.sVal);	}
	Char*		ToString()	const	{ return m_Param.sVal;			}

	void Set(Char* _val){
		m_Param = *(ParamValue*)&_val;
	}

public:
	ParamValue m_Param;
};

class VMCommand
{
public:
	static const s32 kMaxCommandLength = 128;

	typedef Array<VMCommandParamHolder> ParamList;
	typedef s32 (*VMCommandHandler)(const ParamList& _paramList);

public:
	VMCommand(VMCommandHandler _pHandler, const ParamList* _defaultParamList)
		:m_pHandler(_pHandler)
	{
		if(_defaultParamList)
			m_DefaultParamList = *_defaultParamList;
		else
			m_DefaultParamList.Clear();
	}
	s32 Execute(ParamList& _paramList)
	{
		if(m_pHandler)
		{
			if(_paramList.Size() < m_DefaultParamList.Size())
			{
				for(s32 i = _paramList.Size(); i < m_DefaultParamList.Size(); ++i)
				{
					_paramList.PushBack(m_DefaultParamList[i]);
				}
			}
			return (*m_pHandler)(_paramList);
		}
		return 0;
	}
private:
	VMCommandHandler	m_pHandler;
	ParamList			m_DefaultParamList;
};

class VMCommandCenter : public Singleton<VMCommandCenter>
{
	typedef std::map<std::string, VMCommand*>				VMCommandMap;
	typedef std::map<std::string, VMCommand*>::iterator		VMCommandMapIterator;

public:
	VMCommandCenter(){}

	Bool RegisterCommand(StringPtr							_strCmdName, 
						 VMCommand::VMCommandHandler		_pHandler,
						 const VMCommand::ParamList*		_defaultParamList = NULL);
	Bool ExecuteFromString(StringPtr _cmd);

private:
	VMCommandMap m_CommandMap;
};

#endif