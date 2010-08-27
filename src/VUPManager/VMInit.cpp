#include "VMGlobalDef.h"
#include "VMCommand.h"
#include "VMVupManager.h"
#include "VMVup.h"
#include <fxkeys.h>

//--------------------------------------------------------------------------------
Engine*		g_poEngine	= NULL;
//--------------------------------------------------------------------------------------
FXDEFMAP(MyCanvas) MyCanvasMap[]={
	FXMAPFUNC(SEL_COMMAND,		MyCanvas::ID_SENDCOMMAND,					MyCanvas::onCmdSendCommand),
	FXMAPFUNC(SEL_KEYPRESS,		MyCanvas::ID_SENDCOMMAND,					MyCanvas::onKeyPress)
};
FXIMPLEMENT(MyCanvas,FXCanvas,MyCanvasMap,ARRAYNUMBER(MyCanvasMap))
//--------------------------------------------------------------------------------------------
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

	FXHorizontalFrame* poBoxframe = new FXHorizontalFrame(poGroupV2,FRAME_THICK|FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0);
	FXTable* table = new FXTable(poBoxframe,this, ID_TABLE, TABLE_COL_SIZABLE|TABLE_ROW_SIZABLE|LAYOUT_FILL_X|LAYOUT_FILL_Y|TABLE_READONLY|TABLE_NO_COLSELECT|TABLE_NO_ROWSELECT,0,0,0,0, 2,2,2,2);
	table->setBackColor(FXRGB(255,255,255));
	table->setVisibleRows(20);
	table->setVisibleColumns(4);
	table->setTableSize(0, 4);
	table->setCellColor(0,0,FXRGB(255,240,240));
	table->setCellColor(1,0,FXRGB(240,255,240));
	table->setCellColor(0,1,FXRGB(255,240,240));
	table->setCellColor(1,1,FXRGB(240,255,240));
	table->setColumnWidth(0, 155);
	table->setColumnWidth(1, 155);
	table->setRowHeaderWidth(0);
	table->setColumnText(0, "ID");
	table->setColumnText(1, "Status");
	table->setSelBackColor(FXRGB(128,128,128));

	m_VUPTable = table;

	//Register event handler
	GameEngine::GetGameEngine()->GetEventMod()->RegisterHandler(
		(EventType_t)(E_ET_UIUpdateList), 
		new MEventHandler<MyCanvas>(this, &MyCanvas::onUpdateList));
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

	VMCommandCenter* vmCenter = VMCommandCenter::GetPtr();
	if(vmCenter)
	{
		Bool bRet = vmCenter->ExecuteFromString(commandStr.text());
		if(!bRet)
		{
		}
	}
	m_Command->selectAll();

	return 1;
}

void MyCanvas::onUpdateList(const Event* _poEvent)
{
	VMVupManager* pManager = static_cast<VMVupManager*>(_poEvent->GetParam<void*>(0));

	s32 iRow = 0;
	Char zValue[32] = {0};
	VMVupManager::VUPMapConstIterator it = pManager->m_poVupMap.begin();
	for(;it != pManager->m_poVupMap.end(); ++it)
	{
		if(iRow >= m_VUPTable->getNumRows())
		{
			m_VUPTable->insertRows(iRow);
		}
		const VMVup& vup = *((*it).second);

		sprintf(zValue, "%d", vup.GetUniqueID());
		m_VUPTable->setItemText(iRow, 0, zValue);
		m_VUPTable->setItemJustify(iRow, 0, FXTableItem::LEFT|FXTableItem::CENTER_Y);

		m_VUPTable->setItemText(iRow, 1, VMVup::kStatus[vup.GetCurrentStatus()].GetName());
		m_VUPTable->setItemJustify(iRow, 1, FXTableItem::LEFT|FXTableItem::CENTER_Y);

		iRow++;
	}
	if(iRow < m_VUPTable->getNumRows())
	{
		m_VUPTable->removeRows(iRow, m_VUPTable->getNumRows() - iRow);
	}
}

//-------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------
MemPool::MemPool()
{
	size = 0;
	loss = 0;
	maxsize = 100000;
};

MemPool::~MemPool()
{
	mempool.clear();
};

int MemPool::GetUDPData(UDP_PACK *buf, int cnt /* = 1 */)
{
	if(!buf)
		return 0;
	if( size < cnt)
		cnt = size;
	s.Lock();
	std::deque<UDP_PACK>::iterator itstart,itend;
	itstart = itend = mempool.begin();
	for(int i = 0; i < cnt; ++i)
	{
		memcpy(buf + i, &(*itend), sizeof(UDP_PACK));
		itend++;
	}
	mempool.erase(itstart, itend);
	size -= cnt;
	s.UnLock();
	return cnt;
};

void MemPool::InsertUDPData(const UDP_PACK& up)
{
	s.Lock();
	if( size < maxsize )
	{
		mempool.push_back(up);
		size++;
	}
	else
	{
		loss++;
	}
	s.UnLock();
};

void MemPool::CleanBuff()
{
	s.Lock();
	size = 0;
	loss = 0;
	mempool.clear();
	s.UnLock();
}
//---------------------------------------------------------------------------------------------
GameEngine::GameEngine(u32 _uiWidth, u32 _uiHeight, const Char* _strTitle, Bool _bIsWindow)
	:Engine(_uiWidth, _uiHeight, _strTitle, _bIsWindow, 60)
{
	VMCommandCenter::Create();
}
//-------------------------------------------------------------------------------------------------------------------------------
void GameEngine::DoInit()
{
	//Init UI
	FXApp *poApp = g_poEngine->GetRenderMod()->GetGUIApp();
	FXMainWindow *poMain = g_poEngine->GetRenderMod()->GetGUIMainWindow();
	new MyCanvas(poMain);

	//Add Object
	GameEngine::GetGameEngine()->GetSceneMod()->AddObject("VUMMan", new VMVupManager);
}

void GameEngine::DoUnInit()
{
	VMCommandCenter::Destroy();
}