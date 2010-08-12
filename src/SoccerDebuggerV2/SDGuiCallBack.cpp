#include "SDGlobalDef.h"
#include "SDMempool.h"
#include "SDNetWork.h"
#include "SDObject.h"
#include "SDSerializer.h"
#include "tinyxml.h"

void OnCBChangeShowDetail(GuiEvent* p_poEvent)
{	
	u32 uiValue;
	p_poEvent->GetPareToVar(0, uiValue);

	g_bIsShowDetailed = uiValue ? true : false;
}

void OnCBChangeWatchOwner(GuiEvent* p_poEvent)
{	
	u32 uiValue;
	p_poEvent->GetPareToVar(0, uiValue);

	g_bIsWatchOwner = uiValue ? true : false;
}

void OnChangeSlowMotion(GuiEvent* p_poEvent)
{
	f32 fValue;
	p_poEvent->GetPareToVar(0, fValue);

	printf("%f\n", fValue);

	UDP_PACK stMsg;
	stMsg.ulFilter = 0;
	stMsg.ulType = MsgType_SlowMotion;
	stMsg.bIsHidden = true;
	stMsg.unValue._fValue = fValue;

	WSABUF stWSABuf;
	stWSABuf.buf = (char*)&stMsg;
	stWSABuf.len = sizeof(stMsg);

	DWORD cbRet = 0;
	WSASendTo(g_hSSock, &stWSABuf,1, &cbRet, 0, (struct sockaddr*)&g_stDestAddr, sizeof(struct sockaddr), NULL, NULL);
}

void OnCBAlwaysOnTop(GuiEvent* p_poEvent)
{
	u32 uiValue;
	p_poEvent->GetPareToVar(0, uiValue);

	g_bIsAlwaysOnTop = uiValue ? true : false;
	
	g_poEngine->GetActiveRenderer()->SetFullScreen(!g_bIsAlwaysOnTop);
}

void OnClickExit(GuiEvent* p_poEvent)
{
	exit(0);
}

void LoadFile(char* zName)
{
	TiXmlDocument xmlDoc(zName);
	xmlDoc.LoadFile();

	SoccerSerializer* poSerializer = new SoccerSerializer(&xmlDoc);
	poSerializer->Deserialize();
	delete poSerializer;
}

void OnClickOpen(GuiEvent* p_poEvent)
{
#define kMAX_FILENAME_LENGTH	256

	static TCHAR szFileName[kMAX_FILENAME_LENGTH], szTitleName[kMAX_FILENAME_LENGTH];
	static OPENFILENAME pOFN;
	static bool bInitialized = false;
	if (bInitialized == false)
	{
		pOFN.lStructSize		= sizeof(OPENFILENAME);
		pOFN.hwndOwner			= static_cast<WinGDIRenderer*>(g_poEngine->GetActiveRenderer())->GetHWND();
		pOFN.hInstance         	= NULL;
		pOFN.lpstrFilter       	= NULL;
		pOFN.lpstrCustomFilter 	= NULL;
		pOFN.nMaxCustFilter    	= 0;
		pOFN.nFilterIndex      	= 0;
		pOFN.lpstrFile         	= szFileName;
		pOFN.nMaxFile          	= kMAX_FILENAME_LENGTH;
		pOFN.lpstrFileTitle    	= szTitleName;
		pOFN.nMaxFileTitle     	= kMAX_FILENAME_LENGTH;
		pOFN.lpstrInitialDir   	= NULL;
		pOFN.lpstrTitle        	= NULL;
		pOFN.Flags             	= OFN_HIDEREADONLY;
		pOFN.nFileOffset       	= 0;
		pOFN.nFileExtension    	= 0;
		pOFN.lpstrDefExt       	= NULL;
		pOFN.lCustData         	= 0L;
		pOFN.lpfnHook          	= NULL;
		pOFN.lpTemplateName    	= NULL;

		bInitialized = true;
	}

	if (GetOpenFileName(&pOFN))
	{
		strcpy(g_zLastOpenFile, szFileName);
		LoadFile(szFileName);
	}
}

void OnClickOpenLast(GuiEvent* p_poEvent)
{
	if (*g_zLastOpenFile)
	{
		LoadFile(g_zLastOpenFile);
	}
}

void OnClickSave(GuiEvent* p_poEvent)
{
	static TCHAR szFileName[kMAX_FILENAME_LENGTH], szTitleName[kMAX_FILENAME_LENGTH];
	static OPENFILENAME pOFN;
	static bool bInitialized = false;
	if (bInitialized == false)
	{
		pOFN.lStructSize		= sizeof(OPENFILENAME);
		pOFN.hwndOwner			= static_cast<WinGDIRenderer*>(g_poEngine->GetActiveRenderer())->GetHWND();
		pOFN.hInstance         	= NULL;
		pOFN.lpstrFilter       	= NULL;
		pOFN.lpstrCustomFilter 	= NULL;
		pOFN.nMaxCustFilter    	= 0;
		pOFN.nFilterIndex      	= 0;
		pOFN.lpstrFile         	= szFileName;
		pOFN.nMaxFile          	= kMAX_FILENAME_LENGTH;
		pOFN.lpstrFileTitle    	= szTitleName;
		pOFN.nMaxFileTitle     	= kMAX_FILENAME_LENGTH;
		pOFN.lpstrInitialDir   	= NULL;
		pOFN.lpstrTitle        	= NULL;
		pOFN.Flags             	= OFN_HIDEREADONLY;
		pOFN.nFileOffset       	= 0;
		pOFN.nFileExtension    	= 0;
		pOFN.lpstrDefExt       	= NULL;
		pOFN.lCustData         	= 0L;
		pOFN.lpfnHook          	= NULL;
		pOFN.lpTemplateName    	= NULL;

		bInitialized = true;
	}

	if (GetSaveFileName(&pOFN))
	{
		TiXmlDocument xmlDoc;

		SoccerSerializer* poSerializer = new SoccerSerializer(&xmlDoc);
		poSerializer->Serialize();
		delete poSerializer;

		xmlDoc.SaveFile(szFileName);
	}
}
