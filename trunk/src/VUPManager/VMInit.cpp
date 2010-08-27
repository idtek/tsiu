#include "VMGlobalDef.h"
#include "VMCommand.h"

Engine*	g_poEngine = NULL;

//--------------------------------------------------------------------------------------
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
public:
	long onCmdSendCommand(FXObject* sender, FXSelector sel, void* ptr);

public:
	enum {
		ID_SENDCOMMAND = FXMainWindow::ID_LAST,
		ID_TABLE
	};

private:
	FXTextField*		m_Command;
	Array<FXString>		m_CommandHistory;
	s32					m_CurrentCommand;
};

FXDEFMAP(MyCanvas) MyCanvasMap[]={
	FXMAPFUNC(SEL_COMMAND,		MyCanvas::ID_SENDCOMMAND,					MyCanvas::onCmdSendCommand)
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
	table->setColumnText(0, "Name");
	table->setColumnText(1, "Value");
	table->setSelBackColor(FXRGB(128,128,128));
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
//--------------------------------------------------------------------

class RecvUDPRunner : public IThreadRunner
{
public:
	RecvUDPRunner(Socket* _pRecvSock, MemPool* _pMempool);
	virtual u32		Run();
	virtual void	NotifyQuit();

private:
	MemPool* m_pUDPPackBuffer;
	Socket*  m_pRecvSocket;
	Bool	 m_bRequestStop;
};

RecvUDPRunner::RecvUDPRunner(Socket* _pRecvSock, MemPool* _pMempool)
	:m_pRecvSocket(_pRecvSock)
	,m_pUDPPackBuffer(_pMempool)
	,m_bRequestStop(false)
{
}

u32 RecvUDPRunner::Run()
{
	while(1)
	{
		if(m_bRequestStop)
			return 0;

		if(!m_pRecvSocket || !m_pRecvSocket->bIsValid())
			return 1;

		UDP_PACK pack;
		s32 iRet = m_pRecvSocket->RecvFrom((Char*)&pack, sizeof(UDP_PACK));
		if(!iRet)
		{
			m_pUDPPackBuffer->InsertUDPData(pack);
		}
	}
}

void RecvUDPRunner::NotifyQuit()
{
	m_bRequestStop = true;
}

//---------------------------------------------------------------------------------------------
MyEngine::MyEngine(u32 _uiWidth, u32 _uiHeight, const Char* _strTitle, Bool _bIsWindow)
	:Engine(_uiWidth, _uiHeight, _strTitle, _bIsWindow, 60)
	,m_pRecvThread(NULL)
	,m_pRecvSocket(NULL)
	,m_pSendSocket(NULL)
	,m_pUDPPackBuffer(NULL)
{
	VMCommandCenter::Create();
}

//-------------------------------------------------------------------------------------------------------------------------------
s32 Test_SayHello(const VMCommandParamHolder& _p1, const VMCommandParamHolder& _p2)
{
	D_Output("[Test_SayHello]: Hello, %s\n", _p1.ToString());
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------------------
void MyEngine::DoInit()
{
	//Init UI
	FXApp *poApp = g_poEngine->GetRenderMod()->GetGUIApp();
	FXMainWindow *poMain = g_poEngine->GetRenderMod()->GetGUIMainWindow();
	new MyCanvas(poMain);

	//Init network
	m_pRecvSocket = new WinSocket;
	s32 iRet = m_pRecvSocket->Create(E_NETWORK_PROTO_UDP, false);
	D_CHECK(!iRet);
	iRet = m_pRecvSocket->Bind(NULL, 52345);
	D_CHECK(!iRet);

	m_pSendSocket = new WinSocket;
	iRet = m_pSendSocket->Create(E_NETWORK_PROTO_UDP, false);
	D_CHECK(!iRet);

	//Init mem pool
	m_pUDPPackBuffer = new MemPool();
	m_pUDPPackBuffer->SetMaxSize(1000);

	//Init recv thread
	m_pRecvThread = new Thread(new RecvUDPRunner(m_pRecvSocket, m_pUDPPackBuffer));
	Bool bRet = m_pRecvThread->Start();
	D_CHECK(bRet);

	//Register cmd
	VMCommandCenter::GetPtr()->RegisterCommand("say", Test_SayHello, VMCommand::EParamType_String);
}

void MyEngine::DoUnInit()
{
	VMCommandCenter::Destroy();

	m_pRecvThread->Stop(1000);
	D_SafeDelete(m_pRecvThread);
	
	m_pRecvSocket->Destroy();
	D_SafeDelete(m_pRecvSocket);

	m_pSendSocket->Destroy();
	D_SafeDelete(m_pSendSocket);
}