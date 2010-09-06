#ifndef __VM_GLOBALDEF_H__
#define __VM_GLOBALDEF_H__

#include "TsiU.h"

using namespace TsiU;

#if PLATFORM_TYPE == PLATFORM_WIN32
#include "TWin32_Header.h"
#endif

#define USE_UDT_LIB

//-------------------------------------------------------------------------
extern Engine*		g_poEngine;

//-------------------------------------------------------------------------
enum{
	E_ET_UIUpdateList = E_ET_User,
	E_ET_UIUpdateSummay,
};
//--------------------------------------------------------------------------
struct WatchedInfos
{	
	struct WatchedInfoValue
	{
		WatchedInfoValue()
			: m_Visible(false)
			, m_ShowingName("")
			, m_Value("")
		{}
		std::string m_ShowingName;
		std::string m_Value;
		Bool		m_Visible;
	};
	void Init();
	void UpdateValue(StringPtr _strName, s32 _iValue, Bool _bHideSummay = false);
	void UpdateValue(StringPtr _strName, StringPtr _strValue, Bool _bHideSummay = false);
	void UpdateValue(StringPtr _strName, f32 _fValue, Bool _bHideSummay = false);

	typedef std::map<std::string, WatchedInfoValue>				WatchedValueMap;
	typedef std::map<std::string, WatchedInfoValue>::iterator	WatchedValueMapInterator;

	WatchedValueMap m_Values;
};
//--------------------------------------------------------------------------
class MyCanvas : public FXCanvas {
	FXDECLARE(MyCanvas)
protected:
	MyCanvas();

public:
	MyCanvas(FXComposite* p,
		FXObject* tgt = NULL,
		FXSelector sel = 0,
		FXuint opts = 0,
		FXint x = 0,
		FXint y = 0,
		FXint w = 0,
		FXint h = 0);

private:
	void onUpdateList(const Event* _poEvent);
	void onUpdateSummay(const Event* _poEvent);

public:
	long onCmdSendCommand(FXObject* sender, FXSelector sel, void* ptr);
	long onKeyPress(FXObject* sender, FXSelector sel, void* ptr);

public:
	enum {
		ID_SENDCOMMAND = FXMainWindow::ID_LAST,
		ID_TABLE
	};

private:
	FXTextField*		m_Command;
	Array<FXString>		m_CommandHistory;
	s32					m_CurrentCommand;

	FXTable*			m_VUPTable;
	FXTable*			m_Summary;
};
//---------------------------------------------------------------------------------------
class GameEngine : public Engine
{
public:
	static GameEngine* GetGameEngine() { return (GameEngine*)g_poEngine;	}

	GameEngine(u32 _uiWidth, u32 _uiHeight, const Char* _strTitle, Bool _bIsWindow);

	virtual void DoInit();
	virtual void DoUnInit();
};

#endif