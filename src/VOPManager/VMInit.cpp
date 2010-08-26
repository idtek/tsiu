#include "VMGlobalDef.h"

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

	
	//UDP_PACK stMsg;
	//stMsg.ulFilter = 0;
	//stMsg.ulType = MsgType_Command;
	//stMsg.bIsHidden = true;
	//strncpy(stMsg.unValue._zValue, commandStr.text(), 32); 

	//WSABUF stWSABuf;
	//stWSABuf.buf = (char*)&stMsg;
	//stWSABuf.len = sizeof(stMsg);

	//DWORD cbRet = 0;
	//WSASendTo(g_hSSock, &stWSABuf,1, &cbRet, 0, (struct sockaddr*)&g_stDestAddr, sizeof(struct sockaddr), NULL, NULL);

	m_Command->selectAll();

	return 1;
}

//-------------------------------------------------------------------------------------------


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

		//UDP_PACK pack;
		//s32 iRet = m_pRecvSocket->RecvFrom((Char*)&pack, sizeof(UDP_PACK));
		//if(!iRet)
		//{
		//	m_pUDPPackBuffer->InsertUDPData(pack);
		//}
		D_Output("hello\n");
		::Sleep(2000);
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
}

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
}

void MyEngine::DoUnInit()
{
	m_pRecvThread->Stop(5000);
	D_SafeDelete(m_pRecvThread);
	
	m_pRecvSocket->Destroy();
	D_SafeDelete(m_pRecvSocket);

	m_pSendSocket->Destroy();
	D_SafeDelete(m_pSendSocket);
}