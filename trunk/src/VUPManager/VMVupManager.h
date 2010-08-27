#ifndef __VM_VUPMANAGER_H__
#define __VM_VUPMANAGER_H__

#include "VMCommand.h"

class VMVup;

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

class VMVupManager : public Object
{
	typedef std::map<s32, VMVup*>					VUPMap;
	typedef std::map<s32, VMVup*>::iterator			VUPMapIterator;
	typedef std::map<s32, VMVup*>::const_iterator	VUPMapConstIterator;

public:
	static s32 AddVup(const VMCommandParamHolder& _p1, const VMCommandParamHolder& _p2);
	static s32 UpdateVup(const VMCommandParamHolder& _p1, const VMCommandParamHolder& _p2);
	static s32 RemoveVup(const VMCommandParamHolder& _p1, const VMCommandParamHolder& _p2);

public:
	VMVupManager();
	~VMVupManager();

	virtual void Create();
	virtual void Tick(f32 _fDeltaTime);

	Bool			AddVup(VMVup* _newVUP);
	Bool			RemoveVup(s32 _id);
	VMVup*			FindVup(s32 _id);
	const VMVup*	FindVup(s32 _id) const;

	friend class MyCanvas;

private:
	 VUPMap		m_poVupMap;
	 Socket*	m_pRecvSocket;
	 Socket*	m_pSendSocket;
	 Thread*	m_pRecvThread;
	 MemPool*	m_pUDPPackBuffer;
};

#endif