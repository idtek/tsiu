#include "VMGlobalDef.h"
#include "VMCommand.h"
#include "VMVupManager.h"
#include "VMVup.h"
#include <fxkeys.h>
#include <sstream>
#include "VMSummary.h"
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
void WatchedInfos::Clear()
{
	m_Values.clear();
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
		FXTabBook* poTabBook = new FXTabBook(poGroupV22, this, 0, PACK_UNIFORM_WIDTH|PACK_UNIFORM_HEIGHT|LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_RIGHT);
		poTabBook->setTabStyle(TABBOOK_BOTTOMTABS);
		FXuint packing_hints = poTabBook->getPackingHints();
		packing_hints |= PACK_UNIFORM_WIDTH;
		poTabBook->setPackingHints(packing_hints);

		FXTabItem* poTab = new FXTabItem(poTabBook, "Summary View" ,NULL);
		poTab->setTabOrientation(TAB_BOTTOM);
		FXHorizontalFrame* poTableFrame = new FXHorizontalFrame(poTabBook, FRAME_THICK|FRAME_RAISED);
		FXHorizontalFrame* poBoxframe = new FXHorizontalFrame(poTableFrame, FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0);
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

		poTab = new FXTabItem(poTabBook, "Agent View" ,NULL);
		poTab->setTabOrientation(TAB_BOTTOM);
		poTableFrame = new FXHorizontalFrame(poTabBook, FRAME_THICK|FRAME_RAISED);
		poBoxframe = new FXHorizontalFrame(poTableFrame, FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0);
		m_AgentList = new FXTreeList(poBoxframe,NULL,0,FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_TOP|LAYOUT_RIGHT|TREELIST_SHOWS_BOXES|TREELIST_SHOWS_LINES|TREELIST_EXTENDEDSELECT);
		m_AgentList->appendItem(NULL, "Agents");

		poTab = new FXTabItem(poTabBook, "RDV Points View" ,NULL);
		poTab->setTabOrientation(TAB_BOTTOM);
		poTableFrame = new FXHorizontalFrame(poTabBook, FRAME_THICK|FRAME_RAISED);
		poBoxframe = new FXHorizontalFrame(poTableFrame, FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0);
		m_GroupList = new FXTreeList(poBoxframe,NULL,0,FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_TOP|LAYOUT_RIGHT|TREELIST_SHOWS_BOXES|TREELIST_SHOWS_LINES|TREELIST_EXTENDEDSELECT);
		m_GroupList->appendItem(NULL, "RDV Points");
	}

	//Register event handler
	GameEngine::GetGameEngine()->GetEventMod()->RegisterHandler(
		(EventType_t)(E_ET_UIUpdateList), 
		new MEventHandler<MyCanvas>(this, &MyCanvas::onUpdateList));
	GameEngine::GetGameEngine()->GetEventMod()->RegisterHandler(
		(EventType_t)(E_ET_UIUpdateSummary), 
		new MEventHandler<MyCanvas>(this, &MyCanvas::onUpdateSummary));
	GameEngine::GetGameEngine()->GetEventMod()->RegisterHandler(
		(EventType_t)(E_ET_UIUpdateAgent), 
		new MEventHandler<MyCanvas>(this, &MyCanvas::onUpdateAgent));
	GameEngine::GetGameEngine()->GetEventMod()->RegisterHandler(
		(EventType_t)(E_ET_UIUpdateGroup), 
		new MEventHandler<MyCanvas>(this, &MyCanvas::onUpdateGroup));
	GameEngine::GetGameEngine()->GetEventMod()->RegisterHandler(
		(EventType_t)(E_ET_AgentLeave), 
		new MEventHandler<MyCanvas>(this, &MyCanvas::onAgentLeave));
	GameEngine::GetGameEngine()->GetEventMod()->RegisterHandler(
		(EventType_t)(E_ET_GroupSizeChanged), 
		new MEventHandler<MyCanvas>(this, &MyCanvas::onGroupSizeChanged));
	GameEngine::GetGameEngine()->GetEventMod()->RegisterHandler(
		(EventType_t)(E_ET_FindVUP), 
		new MEventHandler<MyCanvas>(this, &MyCanvas::onFindVUP));
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

void MyCanvas::onFindVUP(const Event* _poEvent)
{
	s32 iPassportID = _poEvent->GetParam<s32>(0);
	s32 nIndex = -1;
	s32 jMin = 0;
	s32 jMax = m_VUPTable->getNumRows() - 1;
	s32 jCur = (jMin+jMax)/2;
	do
	{
		s32 iCurPassportID = atoi(m_VUPTable->getItemText(jCur, 0).text());
		if(iCurPassportID > iPassportID) 
		{
			jMax = jCur;
		}
		else if(iCurPassportID < iPassportID)
		{
			jMin =jCur;
		}
		else if(iCurPassportID == iPassportID)
		{
			nIndex = jCur;
			break;
		}
		jCur = (jMin + jMax)/2;
	}while(jMax - jMin > 1);
	m_VUPTable->selectRow(nIndex);
}

void MyCanvas::onAgentLeave(const Event* _poEvent)
{	
	StringPtr pName = static_cast<StringPtr>(_poEvent->GetParam<void*>(0));
	FXTreeItem* pItem = m_AgentList->findItem(pName);
	if(pItem)
	{
		m_AgentList->removeItem(pItem);
	}
}
//#pragma optimize("", off)
void MyCanvas::onUpdateAgent(const Event* _poEvent)
{
	VMSummary* pSummary = static_cast<VMSummary*>(_poEvent->GetParam<void*>(0));
	VMSummary::AgentSummaryMapIterator it = pSummary->m_AgentSummaryMap.begin();
	for(;it != pSummary->m_AgentSummaryMap.end(); ++it)
	{
		const VMSummary::SummaryInfo& si = (*it).second;
		FXTreeItem* pItem = m_AgentList->findItem((*it).first.c_str());
		if(!pItem)
		{
			m_AgentList->expandTree(m_AgentList->getFirstItem());
			pItem = m_AgentList->appendItem(m_AgentList->getFirstItem(), (*it).first.c_str());
			for(s32 i = 0; i < VMSummary::SummaryInfo::kTotalSummaryInfo; ++i)
				m_AgentList->appendItem(pItem, "<Unnamed>");
		}
		if(pItem)
		{
			D_CHECK(pItem->getNumChildren() == VMSummary::SummaryInfo::kTotalSummaryInfo);

			FXTreeItem* pChild = pItem->getFirst();
			
			Char zValue[64] = {0};
			sprintf(zValue, "%s(%d)", SUMMARYNAME_TOTALVUP, si.m_VupCnt);
			m_AgentList->setItemText(pChild, zValue);
			pChild = pChild->getNext();

			for(s32 i = 1; i < EVupStatus_Num; ++i)
			{
				sprintf(zValue, "%s(%d)", VMVup::kStatus[i].GetName(), si.m_StatusCnt[i]);
				m_AgentList->setItemText(pChild, zValue);
				pChild = pChild->getNext();
			}
			for(s32 i = 1; i < ETestPhase_Num; ++i)
			{
				sprintf(zValue, "%s(%d)", VMVup::kTestPhase[i].GetName(), si.m_TestPhaseCnt[i]);
				m_AgentList->setItemText(pChild, zValue);
				pChild = pChild->getNext();
			}
		}
	}
}

void MyCanvas::onGroupSizeChanged(const Event* _poEvent)
{
	StringPtr pName = static_cast<StringPtr>(_poEvent->GetParam<void*>(0));
	FXTreeItem* pItem = m_GroupList->findItem(pName);
	if(pItem)
	{
		m_GroupList->removeItem(pItem);
	}
}

void MyCanvas::onUpdateGroup(const Event* _poEvent)
{
	VMSummary* pSummary = static_cast<VMSummary*>(_poEvent->GetParam<void*>(0));
	VMSummary::GroupInfoMapIterator it = pSummary->m_GroupInfoMap.begin();
	for(;it != pSummary->m_GroupInfoMap.end(); ++it)
	{
		const VMSummary::GroupInfo& gi = (*it).second;
		FXTreeItem* pItem = m_GroupList->findItem((*it).first.c_str());
		if(!pItem)
		{
			m_GroupList->expandTree(m_GroupList->getFirstItem());
			pItem = m_GroupList->appendItem(m_GroupList->getFirstItem(), (*it).first.c_str());
			m_GroupList->appendItem(pItem, "Groups Overview");
			for(s32 i = 0; i < gi.m_Groups.size(); ++i)
			{	
				m_GroupList->appendItem(pItem, "<Unnamed>");
			}
		}
		if(pItem)
		{
			Char zValue[64] = {0};
			FXTreeItem* pSummary = pItem->getFirst();
			FXTreeItem* pChild = pSummary->getNext();

			s32 iTotalVUPs = gi.m_TotalVups;
			s32 iMaxVUPs = 0;

			for(s32 i = 0; i < gi.m_Groups.size(); ++i)
			{
				const VMSummary::GroupInfoData& data = gi.m_Groups[i];
				if(data.m_iMaxVUPsInGroup == -1)
				{
					sprintf(zValue, "Group%d(%d/%d)", i, data.m_iCurVUPsInGroup, iTotalVUPs);
					m_GroupList->setItemText(pChild, zValue);
					
					iMaxVUPs = iTotalVUPs;

					if(pChild->getNumChildren() > data.m_iCurVUPsInGroup)
					{
						s32 removeCnt = pChild->getNumChildren() - data.m_iCurVUPsInGroup;
						for(s32 j = 0; j < removeCnt; ++j)
						{
							m_GroupList->removeItem(pChild->getLast());
						}
					}
					else if(pChild->getNumChildren() < data.m_iCurVUPsInGroup)
					{
						s32 addCnt = data.m_iCurVUPsInGroup - pChild->getNumChildren();
						for(s32 j = 0; j < addCnt; ++j)
						{
							m_GroupList->appendItem(pChild, "<Empty>");
						}
					}
					FXTreeItem* pVUP = pChild->getFirst();
					for(s32 j = 0; j < data.m_VUPsPassport.size(); ++j)
					{
						sprintf(zValue, "VUP(%d)", data.m_VUPsPassport[j]);
						m_GroupList->setItemText(pVUP, zValue);
						pVUP = pVUP->getNext();
					}
				}
				else
				{
					sprintf(zValue, "Group%d(%d/%d)", i, data.m_iCurVUPsInGroup, data.m_iMaxVUPsInGroup);
					m_GroupList->setItemText(pChild, zValue);

					iMaxVUPs += data.m_iMaxVUPsInGroup;

					if(pChild->getNumChildren() != data.m_iMaxVUPsInGroup)
					{
						m_GroupList->removeItems(pChild->getFirst(), pChild->getLast());
						for(s32 j = 0; j < data.m_iMaxVUPsInGroup; ++j)
						{
							m_GroupList->appendItem(pChild, "<Empty>");
						}
					}
					D_CHECK(data.m_VUPsPassport.size() <= data.m_iMaxVUPsInGroup);
					FXTreeItem* pVUP = pChild->getFirst();
					for(s32 j = 0; j < data.m_VUPsPassport.size(); ++j)
					{
						sprintf(zValue, "VUP(%d)", data.m_VUPsPassport[j]);
						m_GroupList->setItemText(pVUP, zValue);
						pVUP = pVUP->getNext();
					}
					for(s32 j = data.m_VUPsPassport.size(); j < data.m_iMaxVUPsInGroup; ++j)
					{
						m_GroupList->setItemText(pVUP, "<Empty>");
						pVUP = pVUP->getNext();
					}
					pChild = pChild->getNext();
				}
			}
			sprintf(zValue, "Groups Overview(%d/%d)", iTotalVUPs, iMaxVUPs);
			m_GroupList->setItemText(pSummary, zValue);
		}
	}
}

void MyCanvas::onUpdateSummary(const Event* _poEvent)
{
	VMVupManager* pManager = static_cast<VMVupManager*>(_poEvent->GetParam<void*>(0));

	s32 iRow = 0;
	Char zValue[64] = {0};

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
	Char zValue[64] = {0};

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

		sprintf(zValue, "(%d, %d)%d", Protocal::GetRDVPointMajor(vup.GetRDVPointID()), Protocal::GetRDVPointMajor(vup.GetRDVPointID()), vup.GetGroup());
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
	VMSummary::Create();
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
	GameEngine::GetGameEngine()->GetSceneMod()->AddObject("VUMSummary", new VMSummaryUpdater);
}

void GameEngine::DoUnInit()
{
	VMSummary::Destroy();
	VMCommandCenter::Destroy();
}