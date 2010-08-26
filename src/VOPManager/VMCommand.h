#ifndef __VM_COMMAND_H__
#define __VM_COMMAND_H__

#include "VMGlobalDef.h"
#include <string>
#include <map>

class VMCommandParamHolder
{
	typedef union
	{
		s32		iVal;
		f32		fVal;
		Char*	sVal;
	} ParamValue;

public:
	VMCommandParamHolder()
	{
		memset(&m_Param, 0, sizeof(ParamValue));
	}

	s32			ToInt()		const	{ return m_Param.iVal;	}
	f32			ToFloat()	const	{ return m_Param.fVal;	}
	Char*		ToString()	const	{ return m_Param.sVal;	}

	template<typename T>
	void Set(const T& _val){
		m_Param = *(ParamValue*)&_val;
	}

public:
	ParamValue m_Param;
};

class VMCommand
{
public:
	static const s32 kMaxParameterCount = 2;

	typedef s32 (*VMCommandHandler)(const VMCommandParamHolder& _p1, const VMCommandParamHolder& _p2);
	enum EParamType
	{
		EParamType_None,
		EParamType_Int,
		EParamType_Float,
		EParamType_String,
	};

public:
	VMCommand(VMCommandHandler _pHandler, EParamType _param1 = EParamType_None, EParamType _param2 = EParamType_None)
		:m_pHandler(_pHandler)
	{
		m_ParamType[0] = _param1;
		m_ParamType[1] = _param2;
	}
	s32 Execute(const VMCommandParamHolder& _p1, const VMCommandParamHolder& _p2)
	{
		if(m_pHandler)
		{
			return (*m_pHandler)(_p1, _p2);
		}
		return 0;
	}
	EParamType GetParamType(s32 _idx) const
	{
		D_CHECK(_idx >= 0 && _idx < kMaxParameterCount);
		return m_ParamType[_idx];
	}
private:
	EParamType			m_ParamType[kMaxParameterCount];
	VMCommandHandler	m_pHandler;
};

class VMCommandCenter : public Singleton<VMCommandCenter>
{
	typedef std::map<std::string, VMCommand*>				VMCommandMap;
	typedef std::map<std::string, VMCommand*>::iterator		VMCommandMapIterator;

public:
	VMCommandCenter(){}

	Bool RegisterCommand(StringPtr						_strCmdName, 
						 VMCommand::VMCommandHandler	_pHandler, 
						 VMCommand::EParamType			_param1 = VMCommand::EParamType_None, 
						 VMCommand::EParamType			_param2 = VMCommand::EParamType_None);
	Bool ExecuteFromString(StringPtr _cmd);

private:
	VMCommandMap m_CommandMap;
};

#endif