#ifndef __LD_GLOBALDEF_H__
#define __LD_GLOBALDEF_H__

#include "TsiU.h"
#include "FXJPGImage.h"
#include <string>
#include <process.h>

using namespace TsiU;

#if PLATFORM_TYPE == PLATFORM_WIN32
#include "TWin32_Header.h"
#endif

extern Engine*						g_poEngine;

class LDImage;

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
	enum {
		ID_STARTSTOP = FXMainWindow::ID_LAST,
		ID_BTN_LOADIMAGE
	};

	void SetImageController(LDImage* _imageController);
	void SetNextImage(FXJPGImage* image);
	void UpdateLoadingProgress(int cur, int max);
	void GetGDIViewerSize(int& width, int& height);

	//event handler
	long onKeyPress(FXObject* sender, FXSelector sel, void *ptr);
	long onLoadImage(FXObject* sender, FXSelector sel, void* ptr);

private:
	wchar_t* _ConvertedString(const char* rawstring, int len);

private:
	FXProgressBar*	m_ProgressBar;
	FXImageView*	m_ImageView;
	LDImage*		m_ImageController;
	bool			m_IsLoadOk;
	FXEGDIViewer*	m_GDIViewer;
};

class GameEngine : public Engine
{
public:
	static GameEngine* GetGameEngine() { return (GameEngine*)g_poEngine;	}

public:
	GameEngine(u32 _width, u32 _height, const Char* _title, Bool _isWindowed);

	virtual void DoInit();
	virtual void DoUnInit();
	virtual void DoPreFrame();

public:
	UIMainPage*					GetMainPage() const					{ return m_pMainPage;			}
	FXJPGImage*					GetImageByIndex(int idx) const		{ return m_pImageList[idx];		}
	void						RemoveImageByIndex(int idx)			{ m_pImageList.Remove(idx);		}
	int							GetImageCount() const				{ return m_pImageList.Size();	}
	SimpleRenderObjectUtility*	GetGDIBrush() const					{ return m_poSROU;				}
	void						LoadImages();

private:
	bool _GetAllImages(const Char* _root, const Char* _ext = NULL,  bool _isRecursion = false);
	bool _LoadImage(const Char* path, FXApp* app);

private:
	UIMainPage*					m_pMainPage;
	SimpleRenderObjectUtility*	m_poSROU;
	List<FXJPGImage*>			m_pImageList;
	Array<std::string>			m_pImagePathList;
};

class LDWindowMsgCallBack : public RenderWindowMsgListener
{
public:
	virtual void OnWindowResize(s32 width, s32 height);
};

#endif