#ifndef __VM_GLOBALDEF_H__
#define __VM_GLOBALDEF_H__

#include "TsiU.h"

using namespace TsiU;

#if PLATFORM_TYPE == PLATFORM_WIN32
#include "TWin32_Header.h"
#endif

//-------------------------------------------------------------------------
extern Engine*		g_poEngine;

//-------------------------------------------------------------------------
enum{
	E_ET_UIUpdateList = E_ET_User,
};
//-------------------------------------------------------------------------
struct UDP_PACK
{
	u32	 ulType;
	union
	{
		struct
		{
			f32 x,y,z;
		}_vValue;
		Char _zValue[32];
		s32 _iValue;
		f32 _fValue;	
	}unValue;
};
//--------------------------------------------------------------------------
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
	void onUpdateList(const Event* _poEvent);

public:
	long onCmdSendCommand(FXObject* sender, FXSelector sel, void* ptr);
	long onKeyPress(FXObject* sender, FXSelector sel, void* ptr);

public:
	enum {
		ID_SENDCOMMAND = FXMainWindow::ID_LAST,
		ID_TABLE
	};

private:
	FXTextField*		m_Command;
	Array<FXString>		m_CommandHistory;
	s32					m_CurrentCommand;

	FXTable*			m_VUPTable;
};

//--------------------------------------------------------------------------
#include <deque>

class MemPool
{
public:
	MemPool();
	virtual ~MemPool();

	int			GetUDPData(UDP_PACK *buf, int cnt = 1);	
	void		InsertUDPData(const UDP_PACK& up);
	void		CleanBuff();

	inline int	GetSize() { return size; };
	inline int	GetLoss() { return loss; };
	inline int	GetMaxSize() { return maxsize; };

	inline void	SetMaxSize(int nmax) 
	{ 
		maxsize = nmax; 
	};

private:
	std::deque<UDP_PACK> mempool;
	Mutex s;
	int size;
	int loss;
	int maxsize;
};
//----------------------------------------------------------------
class GameEngine : public Engine
{
public:
	static GameEngine* GetGameEngine() { return (GameEngine*)g_poEngine;	}

	GameEngine(u32 _uiWidth, u32 _uiHeight, const Char* _strTitle, Bool _bIsWindow);

	virtual void DoInit();
	virtual void DoUnInit();
};

#endif