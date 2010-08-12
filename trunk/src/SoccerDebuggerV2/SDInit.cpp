#include "SDGlobalDef.h"
#include "SDObject.h"
#include "SDNetWork.h"
#include "SDMempool.h"
#include "SDWindowMsgCallBack.h"
#include "fxkeys.h"
//#include "SDGuiCallBack.h"

Engine*						g_poEngine = NULL;
SimpleRenderObjectUtility*	g_poSROU = NULL;
MemPool*					g_poMem = NULL;
HANDLE						g_hRecv;
Bool						g_bStop = false;
ODynamicObj*				g_WatchPlayer = NULL;
u32							g_WindowWidth = kWINDOW_WIDTH;
u32							g_WindowHeight = kWINDOW_HEIGHT;
Bool						g_bIsShowDetailed = false;
Bool						g_bIsWatchOwner = false;
Bool						g_bIsSlowMotion = false;
Bool						g_bIsAlwaysOnTop = false;
#define kMAX_FILENAME_LENGTH	256
Char						g_zLastOpenFile[kMAX_FILENAME_LENGTH];

const u32 kg_NumberOfTab = E_Tab_Num;
const Char* kg_TabDesc[kg_NumberOfTab] = {
	"    Entity    ", "    Attributes    ", "    Statistics    ", "    Watch    "
};

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
	enum {
		ID_SENDCOMMAND = FXMainWindow::ID_LAST,
		ID_TABLE,
	};
public:
	long onCmdSendCommand(FXObject* sender, FXSelector sel, void* ptr);
	long onKeyPress(FXObject* sender, FXSelector sel, void* ptr);
	long onTableSelected(FXObject* sender, FXSelector sel,void* ptr);
	long onTableDeselected(FXObject* sender, FXSelector sel,void* ptr);

	inline FXTable* GetTable(ETabIndex tab) const { return m_TabTable[tab];	}

private:
	FXTextField*		m_Command;
	Array<FXString>		m_CommandHistory;
	s32					m_CurrentCommand;
	FXTable*			m_TabTable[kg_NumberOfTab];
};
FXDEFMAP(MyCanvas) MyCanvasMap[]={
	FXMAPFUNC(SEL_COMMAND,		MyCanvas::ID_SENDCOMMAND,           MyCanvas::onCmdSendCommand),
	FXMAPFUNC(SEL_KEYPRESS,		MyCanvas::ID_SENDCOMMAND,			MyCanvas::onKeyPress),
	FXMAPFUNC(SEL_SELECTED,		MyCanvas::ID_TABLE,					MyCanvas::onTableSelected),
	FXMAPFUNC(SEL_DESELECTED,	MyCanvas::ID_TABLE,					MyCanvas::onTableDeselected),
};
// ButtonApp implementation
FXIMPLEMENT(MyCanvas, FXCanvas, MyCanvasMap, ARRAYNUMBER(MyCanvasMap))

MyCanvas::MyCanvas()
{
	flags |= FLAG_ENABLED;
	m_CurrentCommand = -1;
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
	m_CurrentCommand = -1;

	FXSplitter *poSplitterV		= new FXSplitter(p,LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X|LAYOUT_FILL_Y|SPLITTER_VERTICAL);
	FXVerticalFrame *poGroupV1	= new FXVerticalFrame(poSplitterV,FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y);
	FXVerticalFrame *poGroupV2	= new FXVerticalFrame(poSplitterV,FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y);

	//V1
	FXMatrix* matrix = new FXMatrix(poGroupV1, 3, MATRIX_BY_COLUMNS|LAYOUT_FILL_X);
	new FXLabel(matrix, "Command: ",NULL, JUSTIFY_LEFT|LAYOUT_FILL_X|LAYOUT_CENTER_Y);
	m_Command = new FXTextField(matrix, 1, this, ID_SENDCOMMAND, FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X|LAYOUT_CENTER_Y|LAYOUT_FILL_COLUMN);
	new FXButton(matrix, "Send", NULL, this, ID_SENDCOMMAND, FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X|LAYOUT_CENTER_Y|LAYOUT_FIX_WIDTH, 0, 0, 100);

	//V2
	FXSplitter *poSplitterH			= new FXSplitter(poGroupV2,LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X|LAYOUT_FILL_Y|SPLITTER_HORIZONTAL);
	FXHorizontalFrame *poGroupH1	= new FXHorizontalFrame(poSplitterH,FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0, 0, 900);
	FXHorizontalFrame *poGroupH2	= new FXHorizontalFrame(poSplitterH,FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y);

	//H1
	FXEGDIViewer    *poDXViewer	= new FXEGDIViewer(poGroupH1, NULL, 0, FRAME_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_Y);
	poDXViewer->RegisterListener(new SDWindowMsgCallBack);

	//H2
	FXTabBook* poTabBook = new FXTabBook(poGroupH2, this, 0, PACK_UNIFORM_WIDTH|PACK_UNIFORM_HEIGHT|LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_RIGHT);
	poTabBook->setTabStyle(TABBOOK_BOTTOMTABS);
	FXuint packing_hints = poTabBook->getPackingHints();
	packing_hints |= PACK_UNIFORM_WIDTH;
	poTabBook->setPackingHints(packing_hints);

	for(int i = 0; i < kg_NumberOfTab; ++i)
	{
		FXTabItem* poTab = new FXTabItem(poTabBook, kg_TabDesc[i] ,NULL);
		poTab->setTabOrientation(TAB_BOTTOM);
		FXHorizontalFrame* poTableFrame = new FXHorizontalFrame(poTabBook, FRAME_THICK|FRAME_RAISED);
		FXHorizontalFrame* poBoxframe = new FXHorizontalFrame(poTableFrame,FRAME_THICK|FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0);
		m_TabTable[i] = new FXTable(poBoxframe,this, ID_TABLE, TABLE_COL_SIZABLE|TABLE_ROW_SIZABLE|LAYOUT_FILL_X|LAYOUT_FILL_Y|TABLE_READONLY|TABLE_NO_COLSELECT|TABLE_NO_ROWSELECT,0,0,0,0, 2,2,2,2);
		m_TabTable[i]->setBackColor(FXRGB(255,255,255));
		m_TabTable[i]->setVisibleRows(20);
		m_TabTable[i]->setVisibleColumns(2);
		m_TabTable[i]->setTableSize(0, 2);
		m_TabTable[i]->setCellColor(0,0,FXRGB(255,240,240));
		m_TabTable[i]->setCellColor(1,0,FXRGB(240,255,240));
		m_TabTable[i]->setCellColor(0,1,FXRGB(255,240,240));
		m_TabTable[i]->setCellColor(1,1,FXRGB(240,255,240));
		m_TabTable[i]->setColumnWidth(0, 155);
		m_TabTable[i]->setColumnWidth(1, 155);
		m_TabTable[i]->setRowHeaderWidth(0);
		m_TabTable[i]->setColumnText(0, "Name");
		m_TabTable[i]->setColumnText(1, "Value");
		m_TabTable[i]->setSelBackColor(FXRGB(128,128,128));
	}
}
long MyCanvas::onKeyPress(FXObject* sender, FXSelector sel,void* ptr)
{
	FXEvent* event=(FXEvent*)ptr;
	switch(event->code){
		case KEY_Up:
		case KEY_KP_Up:
			--m_CurrentCommand;
			m_CurrentCommand = Math::Clamp(m_CurrentCommand, 0, (s32)m_CommandHistory.Size() - 1);
			if(m_CurrentCommand >= 0 && m_CurrentCommand < (s32)m_CommandHistory.Size())
			{
				m_Command->setText(m_CommandHistory[m_CurrentCommand]);
				m_Command->selectAll();
			}
			return 1;
		case KEY_Down:
		case KEY_KP_Down:
			++m_CurrentCommand;
			m_CurrentCommand = Math::Clamp(m_CurrentCommand, 0, (s32)m_CommandHistory.Size() - 1);
			if(m_CurrentCommand >= 0 && m_CurrentCommand < m_CommandHistory.Size())
			{
				m_Command->setText(m_CommandHistory[m_CurrentCommand]);
				m_Command->selectAll();
			}
			return 1;
	}
	return 0;
}
long MyCanvas::onCmdSendCommand(FXObject* sender, FXSelector sel,void* ptr)
{
	FXString commandStr = m_Command->getText();
	if(commandStr.empty())
		return 1;

	//printf("%s\n", commandStr.text());
	if(m_CommandHistory.Size() > 0)
	{
		if(m_CommandHistory[m_CommandHistory.Size() - 1] != commandStr)
		{
			m_CommandHistory.PushBack(commandStr);
		}
	}
	else
	{
		m_CommandHistory.PushBack(commandStr);
	}
	m_CurrentCommand = m_CommandHistory.Size();

	UDP_PACK stMsg;
	stMsg.ulFilter = 0;
	stMsg.ulType = MsgType_Command;
	stMsg.bIsHidden = true;
	strncpy(stMsg.unValue._zValue, commandStr.text(), 32); 

	WSABUF stWSABuf;
	stWSABuf.buf = (char*)&stMsg;
	stWSABuf.len = sizeof(stMsg);

	DWORD cbRet = 0;
	WSASendTo(g_hSSock, &stWSABuf,1, &cbRet, 0, (struct sockaddr*)&g_stDestAddr, sizeof(struct sockaddr), NULL, NULL);

	m_Command->selectAll();

	return 1;
}
long MyCanvas::onTableSelected(FXObject* sender, FXSelector sel,void* ptr)
{
	return 1;
}

// Deselected
long MyCanvas::onTableDeselected(FXObject* sender, FXSelector sel,void* ptr)
{
	return 1;
}

//-------------------------------------------------------------------------------------------------------------------------

MyCanvas* g_Canvas = NULL;

namespace Util
{
	typedef std::map<std::string, int>::iterator NameToRowIter;
	std::map<std::string, int> g_NameToRow[E_Tab_Num];
	typedef std::map<std::string, float>::iterator NameToClockIter;
	std::map<std::string, float> g_NameToClock[E_Tab_Num];

	static const float kMaxHighlightingClock = 4.f;

	int GetItemSize(ETabIndex tab)
	{
		FXTable* poTable = g_Canvas->GetTable(tab);
		return poTable->getNumRows();
	}
	void Clear(ETabIndex tab)
	{
		FXTable* poTable = g_Canvas->GetTable(tab);
		int size = poTable->getNumRows();
		for(int i = 0; i < size; ++i)
			poTable->removeRows(0);
		g_NameToRow[tab].clear();
		g_NameToClock[tab].clear();
	}
	int GetRowByName(ETabIndex tab, const Char* name)
	{
		FXTable* poTable = g_Canvas->GetTable(tab);

		NameToRowIter iter = g_NameToRow[tab].find(name);

		int row;
		if(iter == g_NameToRow[tab].end())
		{
			row = poTable->getNumRows();
			poTable->insertRows(row);
			g_NameToRow[tab].insert(std::pair<std::string, int>(name, row));
			g_NameToClock[tab].insert(std::pair<std::string, int>(name, 0));
		}
		else
		{
			row = (*iter).second;
		}
		return row;
	}
	void DrawValue(ETabIndex tab, UDP_PACK* pack, bool shouldHighLight)
	{
		int row = GetRowByName(tab, pack->zName);

		FXTable* poTable = g_Canvas->GetTable(tab);

		poTable->setItemText(row, 0, pack->zName);
		poTable->setItemJustify(row, 0, FXTableItem::LEFT|FXTableItem::CENTER_Y);

		Char zValue[32];
		switch(pack->ulType)
		{
		case MsgType_Float:
			sprintf(zValue, "%.3f", pack->unValue._fValue);
			break;
		case MsgType_Vector:
			sprintf(zValue, "(%.3f, %.3f, %.3f)", pack->unValue._vValue.x, pack->unValue._vValue.y, pack->unValue._vValue.z);
			break;
		case MsgType_Int:
			sprintf(zValue, "%d", pack->unValue._iValue);
			break;
		case MsgType_String:
			strncpy(zValue, pack->unValue._zValue, 32);
			break;
		}
		float clock;

		NameToClockIter iter = g_NameToClock[tab].find(pack->zName);
		D_CHECK(iter != g_NameToClock[tab].end());
		if(!shouldHighLight)
		{
			clock = (*iter).second;
			clock = Math::Max(0.f, clock - g_poEngine->GetClockMod()->GetFrameElapsedSeconds());
		}
		else
		{
			clock = kMaxHighlightingClock;
		}
		(*iter).second = clock;

		if(clock > 0)
		{
			poTable->setItemStipple(row, 1, STIPPLE_WHITE);
		}
		else
		{
			poTable->setItemStipple(row, 1, STIPPLE_BLACK);
		}
		poTable->setItemText(row, 1, zValue);
		poTable->setItemJustify(row, 1, FXTableItem::LEFT|FXTableItem::CENTER_Y);
	}
}

void MyEngine::DoInit()
{
	FXApp *poApp = g_poEngine->GetRenderMod()->GetGUIApp();
	FXMainWindow *poMain = g_poEngine->GetRenderMod()->GetGUIMainWindow();
	g_Canvas = new MyCanvas(poMain);

	NetWorkInit();

	g_poMem = new MemPool;
	g_poMem->SetMaxSize(1000);

	g_poSROU = new WinGDISimpleRenderObjectUtility((WinGDIRenderer*)g_poEngine->GetRenderMod()->GetRenderer());
	//RenderWindowMsg::RegisterMsgListener(new SDWindowMsgCallBack);
	
	g_poEngine->GetSceneMod()->AddObject("ZBall", new OBall);
	g_poEngine->GetSceneMod()->AddObject("Parser", new OParser);
	g_poEngine->GetSceneMod()->AddObject("TeamHome", new OTeam(kHOME_TEAM));
	g_poEngine->GetSceneMod()->AddObject("TeamAway", new OTeam(KAWAY_TEAM));
	g_poEngine->GetSceneMod()->AddObject("Field", new OField);
	g_poEngine->GetSceneMod()->AddObject("Watch", new OWatch);

	g_hRecv = BEGINTHREADEX(0,0,RecvUDPPack,0,0,0);

	memset(g_zLastOpenFile, 0, kMAX_FILENAME_LENGTH);
}

void MyEngine::DoUnInit()
{
	g_bStop = TRUE;
	s32 nRet = WaitForSingleObject(g_hRecv,2000);
	if(nRet == WAIT_TIMEOUT)
	{
		TerminateThread(g_hRecv,0);
	}
	CloseHandle(g_hRecv);
	g_poMem->CleanBuff();

	NetWorkDeinit();

}