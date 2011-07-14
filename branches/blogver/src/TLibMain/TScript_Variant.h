#ifndef __TSCRIPT_SCRIPTVARIANT__
#define __TSCRIPT_SCRIPTVARIANT__



namespace TsiU
{
	enum LuaVariantType_t
	{
		E_LVT_NIL	= 0,
		E_LVT_STR,	
		E_LVT_NUM,	
		E_LVT_INTEGER,	
		E_LVT_BOOL,
		E_LVT_POINTER,
		E_LVT_TABLE,
		E_LVT_MAX,
	};

	template<typename T, LuaVariantType_t E>
	class ScriptVariant
	{
	public:
		ScriptVariant(T _Value)
		{
			m_Value		= _Value;
			m_ValueType	= E;
		}
		T				 GetValue()		{ return m_Value;	 }	
		LuaVariantType_t GetValueType() { return m_ValueType }

	private:
		T					m_Value;
		LuaVariantType_t	m_ValueType;
	}
}

#endif