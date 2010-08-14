#include "GSGlobalDef.h"

//-----------------------------------------------------------------------------------------------------
Engine*						g_poEngine	= NULL;

//-----------------------------------------------------------------------------------------------------
FXDEFMAP(UIMainPage) UIMainPageMap[]={
	FXMAPFUNC(SEL_KEYPRESS,				UIMainPage::ID_LAST,		UIMainPage::onKeyPress),
};
// ButtonApp implementation
FXIMPLEMENT(UIMainPage, FXCanvas, UIMainPageMap, ARRAYNUMBER(UIMainPageMap))

UIMainPage::UIMainPage()
{
	flags |= FLAG_ENABLED;
}

UIMainPage::UIMainPage(FX::FXComposite *p, 
					   FX::FXObject *tgt, 
					   FX::FXSelector sel, 
					   FX::FXuint opts, 
					   FX::FXint x, 
					   FX::FXint y, 
					   FX::FXint w, 
					   FX::FXint h)
					   :FXCanvas(p,tgt,sel,opts,x,y,w,h)
{
	flags |= FLAG_ENABLED;

	FXVerticalFrame *poFrame	= new FXVerticalFrame(p, FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y);
	m_GDIViewer = new FXEGDIViewer(poFrame, NULL, 0, FRAME_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_FILL_COLUMN|LAYOUT_FILL_ROW);
	m_GDIViewer->RegisterListener(new GSWindowMsgCallBack);
}

long UIMainPage::onKeyPress(FXObject* sender, FXSelector sel, void *ptr)
{
	return 1;
}

wchar_t* UIMainPage::_ConvertedString(const char* rawstring, int len)
{
	int buf_len = MultiByteToWideChar(CP_ACP, 0, rawstring, len, NULL, 0);
	wchar_t *data = new wchar_t[buf_len+1];
	data[0] = TEXT('\0');
	buf_len = MultiByteToWideChar(0, 0, rawstring, len, data, buf_len);
	data[buf_len] = TEXT('\0');
	FXint c, i, j, n;
	for(i = j = 0; j < buf_len; j++)
	{
		c = data[j];
		if(c != TEXT('\r'))
		{
			data[i++] = c;
		}
	}
	n = i;
	data[n] = TEXT('\0');
	return data;
}

//--------------------------------------------------------------------------------------------------------------
GameEngine::GameEngine(u32 _width, u32 _height, const Char* _title, Bool _isWindowed)
	:Engine(_width, _height, _title, _isWindowed)
	,m_pMainPage(NULL)
{
}

void GameEngine::DoInit()
{
	FXMainWindow* poMain = g_poEngine->GetRenderMod()->GetGUIMainWindow();
	m_pMainPage = new UIMainPage(poMain);
}

//---------------------------------------------------------------------------------------
void GSWindowMsgCallBack::OnWindowResize(s32 width, s32 height)
{
	WinGDIRenderer* pRender = (WinGDIRenderer*)GameEngine::GetGameEngine()->GetRenderMod()->GetRenderer();
	if(pRender && pRender->GetMainWindow())
		pRender->OnWindowResize(width, height);
}