#ifndef __GS_GLOBALDEF_H__
#define __GS_GLOBALDEF_H__

#include "TsiU.h"

using namespace TsiU;

#if PLATFORM_TYPE == PLATFORM_WIN32
#include "TWin32_Header.h"
#endif

extern Engine*						g_poEngine;

class UIMainPage : public FXCanvas {
	FXDECLARE(UIMainPage)

protected:
	UIMainPage();

public:
	//Construct DX viewer widget
	UIMainPage(FXComposite* p,
		FXObject* tgt = NULL,
		FXSelector sel = 0,
		FXuint opts = 0,
		FXint x = 0,
		FXint y = 0,
		FXint w = 0,
		FXint h = 0);

public:
	//enum {
	//	ID_STARTSTOP = FXMainWindow::ID_LAST,
	//};

	//event handler
	long onKeyPress(FXObject* sender, FXSelector sel, void *ptr);

private:
	wchar_t* _ConvertedString(const char* rawstring, int len);

private:
	FXEGDIViewer*	m_GDIViewer;
};

class GameEngine : public Engine
{
public:
	static GameEngine* GetGameEngine() { return (GameEngine*)g_poEngine;	}

public:
	GameEngine(u32 _width, u32 _height, const Char* _title, Bool _isWindowed);

	virtual void DoInit();
	virtual void DoUnInit(){};
	virtual void DoPreFrame(){};

private:
	UIMainPage*		m_pMainPage;
};

class GSWindowMsgCallBack : public RenderWindowMsgListener
{
public:
	virtual void OnWindowResize(s32 width, s32 height);
};

#endif