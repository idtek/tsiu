#include "VMGlobalDef.h"
#include "VMCommand.h"
#include "VMVupManager.h"
#include "VMVup.h"
#include <fxkeys.h>
#include <sstream>
//--------------------------------------------------------------------------------
Engine*		g_poEngine	= NULL;
//-----------------------------------------------------------------------------------------------
void WatchedInfos::Init()
{
#define ADD_WATCHVALUE(name, init) { \
	WatchedInfoValue wiValue;\
	wiValue.m_ShowingName = name;\
	wiValue.m_Value = init;\
	m_Values.insert(std::pair<std::string, WatchedInfoValue>(name, wiValue));\
	}\

	ADD_WATCHVALUE("0.Total VUPs", "0");
	for(s32 i = 0; i < EVupStatus_Num; ++i)
	{
		ADD_WATCHVALUE(VMVup::kStatus[i].GetName(), "0");
	}
	for(s32 i = 0; i < ETestPhase_Num; ++i)
	{
		ADD_WATCHVALUE(VMVup::kTestPhase[i].GetName(), "0");
	}
}
void WatchedInfos::UpdateValue(StringPtr _strName, s32 _iValue, Bool _bHideSummay)
{	
	std::stringstream str;
	str << _iValue;

	WatchedValueMapInterator it = m_Values.find(_strName);
	if(it != m_Values.end())
	{
		(*it).second.m_Value = str.str();
		(*it).second.m_Visible = _bHideSummay ? (_iValue != 0) : true;
	}
	else
	{
		if(_iValue != 0)
		{
			WatchedInfoValue wiValue;
			wiValue.m_ShowingName = _strName;
			wiValue.m_Value = str.str();
			wiValue.m_Visible = _bHideSummay ? (_iValue != 0) : true;
			m_Values.insert(std::pair<std::string, WatchedInfoValue>(_strName, wiValue));
		}
	}
}
void WatchedInfos::UpdateValue(StringPtr _strName, StringPtr _strValue, Bool _bHideSummay)
{
	WatchedValueMapInterator it = m_Values.find(_strName);
	if(it != m_Values.end())
	{
		(*it).second.m_Value = _strValue;
		(*it).second.m_Visible = true;
	}
	else
	{
		WatchedInfoValue wiValue;
		wiValue.m_ShowingName = _strName;
		wiValue.m_Value = _strValue;
		wiValue.m_Visible = true;
		m_Values.insert(std::pair<std::string, WatchedInfoValue>(_strName, wiValue));
	}
}
void WatchedInfos::UpdateValue(StringPtr _strName, f32 _fValue, Bool _bHideSummay)
{
	std::stringstream str;
	str << _fValue;

	WatchedValueMapInterator it = m_Values.find(_strName);
	if(it != m_Values.end())
	{
		(*it).second.m_Value = str.str();
		(*it).second.m_Visible = _bHideSummay ? (_fValue > 0) : true;
	}
	else
	{
		if(_fValue > 0)
		{
			WatchedInfoValue wiValue;
			wiValue.m_ShowingName = _strName;
			wiValue.m_Value = str.str();
			wiValue.m_Visible = _bHideSummay ? (_fValue > 0) : true;
			m_Values.insert(std::pair<std::string, WatchedInfoValue>(_strName, wiValue));
		}
	}
}
//---------------------------------------------------------------------------------------
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
						  \
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

	FXSplitter *poSplitterH		= new FXSplitter(poGroupV2,LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X|LAYOUT_FILL_Y|SPLITTER_REVERSED);
	FXVerticalFrame *poGroupV21	= new FXVerticalFrame(poSplitterH,FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y);
	FXVerticalFrame *poGroupV22	= new FXVerticalFrame(poSplitterH,FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y);
	{
		FXHorizontalFrame* poBoxframe = new FXHorizontalFrame(poGroupV21,FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0);
		FXTable* table = new FXTable(poBoxframe,this, ID_TABLE, TABLE_COL_SIZABLE|TABLE_ROW_SIZABLE|LAYOUT_FILL_X|LAYOUT_FILL_Y|TABLE_READONLY,0,0,0,0, 2,2,2,2);
		table->setBackColor(FXRGB(255,255,255));
		table->setVisibleRows(20);
		table->setVisibleColumns(7);
		table->setTableSize(0, 7);
		table->setCellColor(0,0,FXRGB(255,240,240));
		table->setCellColor(1,0,FXRGB(240,255,240));
		table->setCellColor(0,1,FXRGB(255,240,240));
		table->setCellColor(1,1,FXRGB(240,255,240));
		for(s32 i = 0; i < 7; ++i)
		{
			table->setColumnWidth(i, 100);
		}
		table->setRowHeaderWidth(0);
		table->setColumnText(0, "ID");
		table->setColumnText(1, "Group");
		table->setColumnWidth(1, 60);
		table->setColumnText(2, "Current Phase");
		table->setColumnWidth(2, 120);
		table->setColumnText(3, "Last Result");
		table->setColumnText(4, "Last Phase");
		table->setColumnWidth(4, 120);
		table->setColumnText(5, "IP");
		table->setColumnWidth(5, 160);
		table->setColumnText(6, "Port");
		table->setSelBackColor(FXRGB(128,128,128));

		m_VUPTable = table;
	}

	{
		FXHorizontalFrame* poBoxframe = new FXHorizontalFrame(poGroupV22, FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0);
		FXTable* table = new FXTable(poBoxframe,this, ID_TABLE, TABLE_COL_SIZABLE|TABLE_ROW_SIZABLE|LAYOUT_FILL_X|LAYOUT_FILL_Y|TABLE_READONLY|LAYOUT_FIX_WIDTH,0,0, 260,0, 2,2,2,2);
		table->setBackColor(FXRGB(255,255,255));
		table->setVisibleRows(20);
		table->setVisibleColumns(2);
		table->setTableSize(0, 2);
		table->setCellColor(0,0,FXRGB(220,220,220));
		table->setCellColor(1,0,FXRGB(255,255,255));
		table->setCellColor(0,1,FXRGB(220,220,220));
		table->setCellColor(1,1,FXRGB(255,255,255));
		table->setColumnWidth(0, 150);
		table->setColumnWidth(1, 100);
		table->setRowHeaderWidth(0);
		table->setColumnText(0, "Name");
		table->setColumnText(1, "Value");
		table->setSelBackColor(FXRGB(128,128,128));
		m_Summary = table;

		//m_WatchedInfo.Init();
		//m_Summary->insertRows(0, m_WatchedInfo.m_Values.size());
		//s32 iRow = 0;
		//WatchedInfos::WatchedValueMapInterator it;
		//for(it = m_WatchedInfo.m_Values.begin(); it != m_WatchedInfo.m_Values.end(); ++it)
		//{
		//	const WatchedInfos::WatchedInfoValue& value = (*it).second;
		//	m_Summary->setItemText(iRow, 0, value.m_ShowingName.c_str());
		//	m_Summary->setItemJustify(iRow, 0, FXTableItem::LEFT|FXTableItem::CENTER_Y);
		//	m_Summary->setItemText(iRow, 1, value.m_Value.c_str());
		//	m_Summary->setItemJustify(iRow, 1, FXTableItem::LEFT|FXTableItem::CENTER_Y);

		//	iRow++;
		//}
	}

	//Register event handler
	GameEngine::GetGameEngine()->GetEventMod()->RegisterHandler(
		(EventType_t)(E_ET_UIUpdateList), 
		new MEventHandler<MyCanvas>(this, &MyCanvas::onUpdateList));
	GameEngine::GetGameEngine()->GetEventMod()->RegisterHandler(
		(EventType_t)(E_ET_UIUpdateSummay), 
		new MEventHandler<MyCanvas>(this, &MyCanvas::onUpdateSummay));
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
			FXMessageBox::error(g_poEngine->GetRenderMod()->GetGUIMainWindow(), MBOX_OK, "Error Executing Command", "Error occurs: %s", commandStr);
		}
		else
		{
			FXMessageBox::information(g_poEngine->GetRenderMod()->GetGUIMainWindow(), MBOX_OK, "Executing Command", "Executing successfully: %s", commandStr);
		}
	}
	m_Command->selectAll();

	return 1;
}
void MyCanvas::onUpdateSummay(const Event* _poEvent)
{
	VMVupManager* pManager = static_cast<VMVupManager*>(_poEvent->GetParam<void*>(0));

	s32 iRow = 0;

	WatchedInfos::WatchedValueMapInterator it = pManager->m_WatchedInfo.m_Values.begin();
	for(;it != pManager->m_WatchedInfo.m_Values.end(); ++it)
	{
		const WatchedInfos::WatchedInfoValue& wiValue = (*it).second;
		if(wiValue.m_Visible)
		{
			if(iRow >= m_Summary->getNumRows())
			{
				m_Summary->insertRows(iRow);
			}
			m_Summary->setItemText(iRow, 0, wiValue.m_ShowingName.c_str());
			m_Summary->setItemJustify(iRow, 0, FXTableItem::LEFT|FXTableItem::CENTER_Y);

			m_Summary->setItemText(iRow, 1, wiValue.m_Value.c_str());
			m_Summary->setItemJustify(iRow, 1, FXTableItem::LEFT|FXTableItem::CENTER_Y);

			iRow++;
		}
	}
	if(iRow < m_Summary->getNumRows())
	{
		m_Summary->removeRows(iRow, m_Summary->getNumRows() - iRow);
	}
}

//#pragma optimize("", off)
void MyCanvas::onUpdateList(const Event* _poEvent)
{
	VMVupManager* pManager = static_cast<VMVupManager*>(_poEvent->GetParam<void*>(0));

	s32 iRow = 0;
	Char zValue[32] = {0};

#ifndef USE_UDT_LIB
	VMVupManager::VUPMapConstIterator it = pManager->m_poVupMap.begin();
	for(;it != pManager->m_poVupMap.end(); ++it)
#else
	const VMVupManager::VUPMap& vupMap = pManager->m_poVupMapByPassport.GetContrainer();
	s32 rowInTable = m_VUPTable->getNumRows();
	s32 sizeInMap = (s32)vupMap.size();
	if(sizeInMap > rowInTable)
	{
		m_VUPTable->insertRows(iRow, sizeInMap - rowInTable);
	}
	VMVupManager::VUPMapConstIterator it = vupMap.begin();
	for(;it != vupMap.end(); ++it)
#endif	
	{
		if(iRow >= m_VUPTable->getNumRows())
		{
			m_VUPTable->insertRows(iRow);
		}
		const VMVup& vup = *((*it).second);

		sprintf(zValue, "%d", vup.GetUniqueID());
		m_VUPTable->setItemText(iRow, 0, zValue);
		m_VUPTable->setItemJustify(iRow, 0, FXTableItem::LEFT|FXTableItem::CENTER_Y);

		sprintf(zValue, "(%d, %d)%d", vup.GetRDVPointID() / 1000, vup.GetRDVPointID() % 1000, vup.GetGroup());
		m_VUPTable->setItemText(iRow, 1, zValue);
		m_VUPTable->setItemJustify(iRow, 1, FXTableItem::LEFT|FXTableItem::CENTER_Y);

		m_VUPTable->setItemText(iRow, 2, VMVup::kTestPhase[vup.GetCurrentTestPhase()].GetName());
		m_VUPTable->setItemJustify(iRow, 2, FXTableItem::LEFT|FXTableItem::CENTER_Y);

		m_VUPTable->setItemText(iRow, 3, VMVup::kStatus[vup.GetLastStaus()].GetName());
		m_VUPTable->setItemJustify(iRow, 3, FXTableItem::LEFT|FXTableItem::CENTER_Y);

		m_VUPTable->setItemText(iRow, 4, VMVup::kTestPhase[vup.GetLastTestPhase()].GetName());
		m_VUPTable->setItemJustify(iRow,4, FXTableItem::LEFT|FXTableItem::CENTER_Y);

		m_VUPTable->setItemText(iRow, 5, vup.GetIPAddress());
		m_VUPTable->setItemJustify(iRow, 5, FXTableItem::LEFT|FXTableItem::CENTER_Y);

		sprintf(zValue, "%d", vup.GetPort());
		m_VUPTable->setItemText(iRow, 6, zValue);
		m_VUPTable->setItemJustify(iRow, 6, FXTableItem::LEFT|FXTableItem::CENTER_Y);

		iRow++;
	}
//#ifndef USE_UDT_LIB
//	pManager->m_poVupMapByPassport.ReleaseContrainer();
//#endif
	if(iRow < m_VUPTable->getNumRows())
	{
		m_VUPTable->removeRows(iRow, m_VUPTable->getNumRows() - iRow);
	}
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