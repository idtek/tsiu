#ifndef __TGUI_EVENT__
#define __TGUI_EVENT__

#include "TCore_Types.h"
#include <deque>

namespace TsiU
{
	struct ka_msg_s32_tag{};
	struct ka_msg_u32_tag{};
	struct ka_msg_f32_tag{};

	union GuiEventParam
	{
		s32		lParam;
		u32		ulParam;
		f32		fParam;
	};

	class GuiEventParamObject
	{
	public:
		void SetValue(const GuiEventParam& p_Param){ m_Param = p_Param; };

		virtual	operator s32()		{ return m_Param.lParam;	}
		virtual	operator u32()		{ return m_Param.ulParam;	}
		virtual	operator f32()		{ return m_Param.fParam;	}

	private:
		GuiEventParam m_Param;
	};

	template<typename T>
	class GuiEventParamObjectFactory{
	public:
		static GuiEventParamObject* Create(T value){
			GuiEventParamObject* obj = new GuiEventParamObject();
			obj->SetValue(*((GuiEventParam*)&value));
			return obj;
		};
	};
	
	class GuiEvent
	{
	public:
		GuiEvent(){};
		~GuiEvent()
		{
			for(s32 i = 0; i < CountParam(); ++i)
			{
				if(m_arParamList[i])
					delete m_arParamList[i];
			}
			m_arParamList.clear();
		};

		GuiEvent* AddParam(s32 value){ m_arParamList.push_back(GuiEventParamObjectFactory<s32>::Create(value)); return this; };
		GuiEvent* AddParam(u32 value){ m_arParamList.push_back(GuiEventParamObjectFactory<u32>::Create(value)); return this; };
		GuiEvent* AddParam(f32 value){ m_arParamList.push_back(GuiEventParamObjectFactory<f32>::Create(value)); return this; };

		s32	GetParam(s32 idx, ka_msg_s32_tag)	{ return (s32)*m_arParamList[idx]; };
		u32	GetParam(s32 idx, ka_msg_u32_tag)	{ return (u32)*m_arParamList[idx]; };
		f32	GetParam(s32 idx, ka_msg_f32_tag)	{ return (f32)*m_arParamList[idx]; };

		void GetPareToVar(s32 idx, s32& val)	{ val = GetParam(idx, ka_msg_s32_tag()); };
		void GetPareToVar(s32 idx, u32& val)	{ val = GetParam(idx, ka_msg_u32_tag()); };
		void GetPareToVar(s32 idx, f32& val)	{ val = GetParam(idx, ka_msg_f32_tag()); };

		s32	CountParam() { return static_cast<s32>(m_arParamList.size()); };	

	private:
		std::deque<GuiEventParamObject*> m_arParamList;
	};
}

#endif