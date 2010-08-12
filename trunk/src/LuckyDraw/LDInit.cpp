#include "LDGlobalDef.h"
#include "LDImage.h"
#include "fxkeys.h"

Engine*						g_poEngine	= NULL;

FXDEFMAP(UIMainPage) UIMainPageMap[]={
	FXMAPFUNC(SEL_KEYPRESS,				UIMainPage::ID_STARTSTOP,		UIMainPage::onKeyPress),
	FXMAPFUNC(SEL_COMMAND,				UIMainPage::ID_BTN_LOADIMAGE,	UIMainPage::onLoadImage)
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
 ,m_ImageController(NULL)
 ,m_IsLoadOk(false)
{
	//setTarget(this);
	flags |= FLAG_ENABLED;

	FXSplitter *poSplitterV		= new FXSplitter(p,LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X|LAYOUT_FILL_Y|SPLITTER_VERTICAL);
	FXVerticalFrame *poGroupV1	= new FXVerticalFrame(poSplitterV,FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0, 0, 800, 630);
	FXVerticalFrame *poGroupV2	= new FXVerticalFrame(poSplitterV,FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y);

	m_ImageView = new FXImageView(poGroupV1, NULL, this, ID_STARTSTOP, FRAME_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_Y);
	m_ImageView->setFocus();

	FXMatrix* poMatrixH1 = new FXMatrix(poGroupV2, 2, MATRIX_BY_ROWS|LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_FILL_COLUMN);
	m_ProgressBar = new FXProgressBar(poMatrixH1, NULL, 0, LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_FILL_COLUMN|FRAME_SUNKEN|FRAME_THICK);

	FXMatrix* poMatrixV1 = new FXMatrix(poMatrixH1, 2, MATRIX_BY_COLUMNS|LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_FILL_COLUMN|LAYOUT_FILL_ROW);
	
	wchar_t* btnName = _ConvertedString("¼ÓÔØÍ¼Æ¬", 8);	
	new FXButton(poMatrixV1, btnName, NULL, this, ID_BTN_LOADIMAGE, FRAME_RAISED|FRAME_THICK|LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_FILL_ROW);

	m_GDIViewer = new FXEGDIViewer(poMatrixV1, NULL, 0, FRAME_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_FILL_COLUMN|LAYOUT_FILL_ROW);
	m_GDIViewer->RegisterListener(new LDWindowMsgCallBack);
}

void UIMainPage::SetImageController(LDImage* _imageController)
{
	m_ImageController = _imageController;
}

void UIMainPage::SetNextImage(FXJPGImage* image)
{
	FXImage* lastImage = m_ImageView->getImage();
	if(lastImage)
	{
		lastImage->destroy();
	}

	int width = m_ImageView->getWidth();
	int height = m_ImageView->getHeight();

	if(int(width * 0.75f) < height)
		image->scale(width, int(width * 0.75f));
	else
		image->scale(int(height / 0.75f), height);
	
	image->create();
	m_ImageView->setImage(image);
}

void UIMainPage::UpdateLoadingProgress(int cur, int max)
{
	int dataProgress = (int)((float)cur / max * 100);
	m_ProgressBar->setProgress(dataProgress);
}

void UIMainPage::GetGDIViewerSize(int& width, int& height)
{
	width = m_GDIViewer->getWidth();
	height = m_GDIViewer->getHeight();
}

long UIMainPage::onKeyPress(FXObject* sender, FXSelector sel, void *ptr)
{
	if(!m_IsLoadOk)
		return 1;

	FXEvent* event=(FXEvent*)ptr;
	switch(event->code)
	{
	case KEY_space:
		m_ImageController->TriggerStartStop();
		break;
	}
	return 1;
}

long UIMainPage::onLoadImage(FXObject* sender, FXSelector sel, void* ptr)
{
	GameEngine::GetGameEngine()->LoadImages();
	m_ImageView->setFocus();
	m_IsLoadOk = true;

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

//----------------------------------------------------------------------------------------------
GameEngine::GameEngine(u32 _width, u32 _height, const Char* _title, Bool _isWindowed)
	:Engine(_width, _height, _title, _isWindowed)
	,m_pMainPage(NULL)
{
}

void GameEngine::DoInit()
{
	FXMainWindow* poMain = g_poEngine->GetRenderMod()->GetGUIMainWindow();
	//poMain->setDecorations(DECOR_MINIMIZE | DECOR_CLOSE | DECOR_TITLE | DECOR_BORDER);
	m_pMainPage = new UIMainPage(poMain);

	LDImage* imageController = new LDImage();
	m_pMainPage->SetImageController(imageController);

	m_poSROU = new WinGDISimpleRenderObjectUtility((WinGDIRenderer*)g_poEngine->GetRenderMod()->GetRenderer());

	GetSceneMod()->AddObject("ImageChange", imageController);
}

void GameEngine::LoadImages()
{
	if(!_GetAllImages(".", "jpg"))
	{
		D_FatalError("failed to load image files");
	}

	m_pImageList.Clear();

	FXApp* poApp = g_poEngine->GetRenderMod()->GetGUIApp();
	for(int i = 0; i < m_pImagePathList.Size(); ++i)
	{
		bool isOk = _LoadImage(m_pImagePathList[i].c_str(), poApp);
		D_CHECK(isOk);
		m_pMainPage->UpdateLoadingProgress(i + 1, m_pImagePathList.Size());
	}
	//Set first image
	//if(m_pImageList.Size() > 1)
	//	m_pMainPage->SetNextImage(m_pImageList[0]);
}

bool GameEngine::_LoadImage(const Char* _path, FXApp* _app)
{
	File* file = FileManager::GetPtr()->OpenFile(_path, E_FOM_Read | E_FOM_Binary);
	D_CHECK(file);
	u32 fileSize = file->Size();
	u8* data = new u8[fileSize];
	bool ret = file->Read(data, fileSize);
	D_CHECK(ret);
	FXJPGImage* poImage = new FXJPGImage(_app, (const void*)data, IMAGE_KEEP);
	FileManager::GetPtr()->CloseFile(file);
	delete[] data;
	m_pImageList.AddTail(poImage);

	return true;
}

bool GameEngine::_GetAllImages(const Char* _root, const Char* _ext,  bool _isRecursion)
{
	m_pImagePathList.Clear();

	TCHAR szFind[MAX_PATH];
	lstrcpy(szFind, _root);
	lstrcat(szFind, "\\");
	lstrcat(szFind, "*.*");

	WIN32_FIND_DATA findFileData;
	HANDLE hFind = FindFirstFile(szFind, &findFileData);
	if(hFind == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	else
	{
		do 
		{
			//ignore ./..
			if (findFileData.cFileName[0] == '.')
				continue;

			//ignore dir
			if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				continue;

			//filter ext
			if (_ext != NULL)
			{
				std::string fullName = strlwr(findFileData.cFileName);
				std::string::size_type findPos = fullName.rfind(".");
				if(findPos != std::string::npos)
				{
					std::string ext = fullName.substr(findPos + 1);
					if(ext != std::string(_ext))
					{
						continue;
					}
				}
			}

			TCHAR szFullPath[MAX_PATH];
			lstrcpy(szFullPath, _root);
			lstrcat(szFullPath, "\\");
			lstrcat(szFullPath, findFileData.cFileName);
			m_pImagePathList.PushBack(std::string(szFullPath));

		}while(FindNextFile(hFind, &findFileData));
	}
	return true;
}

void GameEngine::DoPreFrame()
{
}
void GameEngine::DoUnInit()
{
}

//---------------------------------------------------------------------------------------
void LDWindowMsgCallBack::OnWindowResize(s32 width, s32 height)
{
	WinGDIRenderer* pRender = (WinGDIRenderer*)GameEngine::GetGameEngine()->GetRenderMod()->GetRenderer();
	if(pRender && pRender->GetMainWindow())
		pRender->OnWindowResize(width, height);
}