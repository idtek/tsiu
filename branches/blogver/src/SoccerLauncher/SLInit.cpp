#include "SLGlobalDef.h"
#include "SLUtility.h"

#define GAME_NAME "SuperSoccerOnline_r.exe"

Engine*						g_poEngine = NULL;

class MyCanvas : public FXCanvas {
	FXDECLARE(MyCanvas)

protected:
	MyCanvas();

public:
	//Construct DX viewer widget
	MyCanvas(FXComposite* p,
		FXObject* tgt = NULL,
		FXSelector sel = 0,
		FXuint opts = 0,
		FXint x = 0,
		FXint y = 0,
		FXint w = 0,
		FXint h = 0);
public:
	long onLaunchGame(FXObject* sender, FXSelector sel, void* ptr);
	long onOpenOutputLogDir(FXObject* sender, FXSelector sel, void* ptr);
	long onEditMiniGame(FXObject* sender, FXSelector sel, void* ptr);
	long onMiniGameChange(FXObject* sender, FXSelector sel, void* ptr);
	long onUserControlModeChange(FXObject* sender, FXSelector sel, void* ptr);
	long onOpenPlayerInfo(FXObject* sender, FXSelector sel, void* ptr);

private:
	void initMinigameList();
	void initOtherUI(const char* minigamename);
	void initDesc();
	void openDataFile(const char* path);
	void generateStartupFile();
	void generateSettingFile();

public:
	enum {
		ID_BTN_LAUNCH = FXMainWindow::ID_LAST,
		ID_BTN_OPENOUTPUTDIR,
		ID_BTN_EDITMINIGAME,
		ID_CB_MINIGAMECHANGED,
		ID_CB_USERCONTROLMODECHANGED,
		ID_LT_OPENPLAYERINFO
	};

private:
	FXComboBox*		m_MiniGame;
	FXList*			m_PlayerList;
	FXComboBox*		m_UserControlMode;
	FXTextField*	m_OutputDir;
	FXText*			m_Description;

	SLData		m_Data;
};
FXDEFMAP(MyCanvas) MyCanvasMap[]={
	FXMAPFUNC(SEL_COMMAND,		MyCanvas::ID_BTN_LAUNCH,					MyCanvas::onLaunchGame),
	FXMAPFUNC(SEL_COMMAND,		MyCanvas::ID_BTN_OPENOUTPUTDIR,				MyCanvas::onOpenOutputLogDir),
	FXMAPFUNC(SEL_COMMAND,		MyCanvas::ID_BTN_EDITMINIGAME,				MyCanvas::onEditMiniGame),
	FXMAPFUNC(SEL_COMMAND,		MyCanvas::ID_CB_MINIGAMECHANGED,			MyCanvas::onMiniGameChange),
	FXMAPFUNC(SEL_COMMAND,		MyCanvas::ID_CB_USERCONTROLMODECHANGED,		MyCanvas::onUserControlModeChange),
	FXMAPFUNC(SEL_DOUBLECLICKED,MyCanvas::ID_LT_OPENPLAYERINFO,				MyCanvas::onOpenPlayerInfo),
};
FXIMPLEMENT(MyCanvas,FXCanvas,MyCanvasMap,ARRAYNUMBER(MyCanvasMap))

MyCanvas::MyCanvas()
{
	flags |= FLAG_ENABLED;
}

MyCanvas::MyCanvas(FX::FXComposite *p, 
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

	FXMatrix* poMatrixMain = new FXMatrix(p, 2, MATRIX_BY_ROWS|LAYOUT_FILL_X|LAYOUT_FILL_Y);

	FXSplitter* poSplitterV		= new FXSplitter(poMatrixMain, LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X|LAYOUT_FILL_Y|SPLITTER_HORIZONTAL);
	FXVerticalFrame* poGroupV1	= new FXVerticalFrame(poSplitterV,FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y);
	FXVerticalFrame* poGroupV2	= new FXVerticalFrame(poSplitterV,FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y);

	//init player list
	m_PlayerList = new FXList(poGroupV1, this, ID_LT_OPENPLAYERINFO, FRAME_THICK|FRAME_RAISED|LAYOUT_FILL_Y|LAYOUT_FIX_WIDTH|LIST_SINGLESELECT,0,0,80,0);
	m_PlayerList->setBackColor(FXRGB(240,240,255));

	FXMatrix* poMatrixV2 = new FXMatrix(poGroupV2, 20, MATRIX_BY_ROWS|LAYOUT_FILL_X|LAYOUT_FILL_Y);

	new FXLabel(poMatrixV2, "Log Output Dir ", NULL, LAYOUT_LEFT|LAYOUT_CENTER_Y);
	m_OutputDir = new FXTextField(poMatrixV2, 1, NULL, 0, FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X|LAYOUT_CENTER_Y|LAYOUT_FILL_COLUMN);
	new FXButton(poMatrixV2, "Open...", NULL, this, ID_BTN_OPENOUTPUTDIR, FRAME_RAISED|FRAME_THICK|LAYOUT_BOTTOM);

	new FXLabel(poMatrixV2, "Select control mode: ", NULL, LAYOUT_LEFT|LAYOUT_CENTER_Y);
	m_UserControlMode = new FXComboBox(poMatrixV2, 25, NULL, 0, COMBOBOX_STATIC|LAYOUT_RIGHT|LAYOUT_FIX_WIDTH|FRAME_SUNKEN|FRAME_THICK, 0,0, 200, 0, 0, 0, 1, 1);
	m_UserControlMode->setNumVisible(4);
	m_UserControlMode->appendItem("Auto");
	m_UserControlMode->appendItem("One Player");
	m_UserControlMode->appendItem("Two Players(1 Team)");
	m_UserControlMode->appendItem("Two Players(2 Teams)");

	new FXLabel(poMatrixV2, "Select mini game: ", NULL, LAYOUT_LEFT|LAYOUT_CENTER_Y);
	m_MiniGame = new FXComboBox(poMatrixV2, 25, this, ID_CB_MINIGAMECHANGED, COMBOBOX_STATIC|LAYOUT_RIGHT|LAYOUT_FIX_WIDTH|FRAME_SUNKEN|FRAME_THICK, 0,0, 200, 0, 0, 0, 1, 1);
	m_MiniGame->setNumVisible(7);
	new FXButton(poMatrixV2, "Edit...", NULL, this, ID_BTN_EDITMINIGAME, FRAME_RAISED|FRAME_THICK|LAYOUT_BOTTOM);

	new FXButton(poMatrixV2, "Launch Game", NULL, this, ID_BTN_LAUNCH, FRAME_RAISED|FRAME_THICK|LAYOUT_BOTTOM|LAYOUT_FIX_WIDTH|LAYOUT_FIX_HEIGHT, 0, 0, 200, 40);

	m_Description = new FXText(poMatrixMain, NULL, 0, TEXT_READONLY|TEXT_WORDWRAP|LAYOUT_FILL_COLUMN|LAYOUT_FILL_ROW|LAYOUT_FILL|TEXT_NO_TABS);
	m_Description->setBackColor(FXRGB(255,255,240));

	//-----------------------------------------------------
	//inti ui from setting files
	//-----------------------------------------------------
	char fullPath[128];
	sprintf(fullPath, "%s\\TestMiniGameSetting.ini", kBasePath);

	int userControlMode = ::GetPrivateProfileInt("Settings", "userControlMode", 0, fullPath);
	D_CHECK(userControlMode >= 0 && userControlMode < 4);

	char logOutputDir[128];
	::GetPrivateProfileString("Settings", "logOutputDir", "C:\\", logOutputDir, 128, fullPath);

	m_UserControlMode->setCurrentItem(userControlMode);
	m_OutputDir->setText(logOutputDir);

	//Init UI data
	initMinigameList();
	initOtherUI(m_MiniGame->getText().text());
	initDesc();
}

long MyCanvas::onLaunchGame(FXObject* sender, FXSelector sel, void* ptr)
{
	generateStartupFile();
	generateSettingFile();
	
	HWND mainHwnd = (HWND)g_poEngine->GetRenderMod()->GetGUIMainWindow()->id();
	HINSTANCE hInst = ::ShellExecute(mainHwnd, "open", GAME_NAME, "-gpptest -cve", "", SW_SHOW);
	if((int)hInst <= 32)
	{
		FXMessageBox::error(this, MBOX_OK, tr("Error"), tr("Cannot launch game: %s"), GAME_NAME);
	}
	return 1;
}

long MyCanvas::onOpenOutputLogDir(FXObject* sender, FXSelector sel, void* ptr)
{
	FXDirDialog open(this,"Open Output Directory");
	open.showFiles(FALSE);
	if(open.execute()){
		m_OutputDir->setText(open.getDirectory().text());
	}
	return 1;
}
long MyCanvas::onEditMiniGame(FXObject* sender, FXSelector sel, void* ptr)
{
	char fullPath[128];
	sprintf(fullPath, "%s\\%s\\%s_Core.lua", kBasePath, m_Data.m_MiniGame.c_str(), m_Data.m_MiniGame.c_str());
	openDataFile(fullPath);
	return 1;
}
long MyCanvas::onOpenPlayerInfo(FXObject* sender, FXSelector sel, void* ptr)
{
	char fullPath[128];
	sprintf(fullPath, "%s\\%s\\%s_%s.lua", kBasePath, m_Data.m_MiniGame.c_str(), m_Data.m_MiniGame.c_str(), m_PlayerList->getItemText(m_PlayerList->getCurrentItem()).text());
	openDataFile(fullPath);
	return 1;
}

long MyCanvas::onMiniGameChange(FXObject* sender, FXSelector sel, void* ptr)
{
	initOtherUI((const char*)ptr);	
	initDesc();
	return 1;
}
long MyCanvas::onUserControlModeChange(FXObject* sender, FXSelector sel, void* ptr)
{
	return 1;
}

void MyCanvas::openDataFile(const char* path)
{
	HWND mainHwnd = (HWND)g_poEngine->GetRenderMod()->GetGUIMainWindow()->id();

	HINSTANCE hInst = ::ShellExecute(mainHwnd,"open", path, "", "", SW_SHOW);
	if((int)hInst <= 32)
	{
		if((int)hInst == ERROR_ACCESS_DENIED)
		{
			hInst = ::ShellExecute(mainHwnd, "open", "notepad.exe", path, "", SW_SHOW);
			if((int)hInst > 32)
			{
				return;
			}
		}
		FXMessageBox::error(this, MBOX_OK, tr("Error"), tr("Cannot open file: %s"), path);
	}
}

void MyCanvas::generateStartupFile()
{
	std::string startup;
	startup += "do\r\n";
	startup += "GameControl.skip_menu = true;\r\n";
	startup += "GameControl.set_startup_by_lua (\"Media/Scripts/AttrTestMinigame/";
	startup += m_Data.m_MiniGame;
	startup += "/";
	startup += m_Data.m_MiniGame;
	startup += "_Core.lua\", GameType.GameType_GENERIC);\r\n";
	startup += "end";

	static const char* kMiniGameStartup = ".\\Media\\scripts\\AttrTestMinigame\\TestMiniGameStartup.lua";

	File* file = FileManager::GetPtr()->OpenFile(kMiniGameStartup, E_FOM_Write|E_FOM_Binary);
	D_CHECK(file != NULL);
	bool ret = file->Write(startup.c_str(), startup.length());
	D_CHECK(ret);
	FileManager::GetPtr()->CloseFile(file);
}
void MyCanvas::generateSettingFile()
{
	static const char* kUsercontrolModeString[] ={
		"ETC_UserControlMode.AUTO",
		"ETC_UserControlMode.SOLO",
		"ETC_UserControlMode.DUEL_TM",
		"ETC_UserControlMode.DUEL_OPP",
	};

	//-----------------------------------------------------
	//Write setting files for game
	//-----------------------------------------------------
	std::string setting;
	setting += "userControlMode = ";
	setting += kUsercontrolModeString[m_UserControlMode->getCurrentItem()];
	setting += ";\r\n";
	setting += "logOutputDir = [[";
	setting += m_OutputDir->getText().text();
	setting += "]];";

	char fullPath[128];
	sprintf(fullPath, "%s\\TestMiniGameSetting.lua", kBasePath);

	File* file = FileManager::GetPtr()->OpenFile(fullPath, E_FOM_Write|E_FOM_Binary);
	D_CHECK(file != NULL);
	bool ret = file->Write(setting.c_str(), setting.length());
	D_CHECK(ret);
	FileManager::GetPtr()->CloseFile(file);

	//-----------------------------------------------------
	//Write setting files for launcher
	//-----------------------------------------------------
	sprintf(fullPath, "%s\\TestMiniGameSetting.ini", kBasePath);

	char usercontrolOption[4];
	itoa(m_UserControlMode->getCurrentItem(), usercontrolOption, 10);

	::WritePrivateProfileString("Settings", "userControlMode", usercontrolOption, fullPath);
	::WritePrivateProfileString("Settings", "logOutputDir", m_OutputDir->getText().text(), fullPath);
}

void MyCanvas::initOtherUI(const char* minigamename)
{
	m_Data.Init(minigamename);

	m_PlayerList->clearItems();
	for(int i = 0; i < m_Data.m_PlayerList.Size(); ++i)
	{
		m_PlayerList->appendItem(m_Data.m_PlayerList[i].c_str());
	}
}
void MyCanvas::initMinigameList()
{
	static const char* kMiniGameList = ".\\Media\\scripts\\AttrTestMinigame\\TestCaseList.txt";

	File* file = FileManager::GetPtr()->OpenFile(kMiniGameList, E_FOM_Read|E_FOM_Binary);
	D_CHECK(file != NULL);
	unsigned int fileSize = file->Size();
	char* buff = new char[fileSize+1];
	bool ret = file->Read(buff, fileSize);
	D_CHECK(ret);
	buff[fileSize]= '\0';
	
	const char* sep = "\r\n";
	char* s = strtok(buff, sep);
	while(s != NULL)
	{
		m_MiniGame->appendItem(s);
		s = strtok(NULL, sep);
	}
	FileManager::GetPtr()->CloseFile(file);
	delete[] buff;
}

void MyCanvas::initDesc()
{
	char fullPath[128];
	sprintf(fullPath, "%s\\%s\\%s_Desc.txt", kBasePath, m_Data.m_MiniGame.c_str(), m_Data.m_MiniGame.c_str());

	File* file = FileManager::GetPtr()->OpenFile(fullPath, E_FOM_Read|E_FOM_Binary);
	D_CHECK(file != NULL);
	unsigned int fileSize = file->Size();
	char* buff = new char[fileSize + 1];
	bool ret = file->Read(buff, fileSize);
	D_CHECK(ret);
	buff[fileSize]= '\0';
	FileManager::GetPtr()->CloseFile(file);

	int buf_len = MultiByteToWideChar(CP_ACP, 0, buff, fileSize, NULL, 0);
	wchar_t *data = new wchar_t[buf_len+1];
	data[0] = TEXT('\0');
	buf_len = MultiByteToWideChar(0, 0, buff, fileSize, data, buf_len);
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

	m_Description->setText(data);

	delete[] buff;
	delete[] data;
}

//-------------------------------------------------------------------------------------------------------------------------
MyCanvas* g_Canvas = NULL;

void MyEngine::DoInit()
{
	FXApp *poApp = g_poEngine->GetRenderMod()->GetGUIApp();
	FXMainWindow *poMain = g_poEngine->GetRenderMod()->GetGUIMainWindow();
	g_Canvas = new MyCanvas(poMain);
}

void MyEngine::DoUnInit()
{
}