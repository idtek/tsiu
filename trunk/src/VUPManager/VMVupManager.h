#ifndef __VM_VUPMANAGER_H__
#define __VM_VUPMANAGER_H__

#include "VMCommand.h"
#include "VMProtocal.h"

class VMVup;

struct UDP_PACKWrapper{
	UDP_PACK	m_InnerData;
	std::string	m_SrcIPAddress;
	u16			m_SrcPort;
};


class RecvUDPRunner : public IThreadRunner
{
public:
	RecvUDPRunner(Socket* _pRecvSock, MemPool<UDP_PACKWrapper>* _pMempool);
	virtual u32		Run();
	virtual void	NotifyQuit();

private:
	MemPool<UDP_PACKWrapper>*	m_pUDPPackBuffer;
	Socket*						m_pRecvSocket;
	Bool						m_bRequestStop;
};

class VMVupManager : public Object
{
	typedef std::map<s32, VMVup*>					VUPMap;
	typedef std::map<s32, VMVup*>::iterator			VUPMapIterator;
	typedef std::map<s32, VMVup*>::const_iterator	VUPMapConstIterator;

public:
	static s32 AddVup(const VMCommandParamHolder& _p1, const VMCommandParamHolder& _p2, const VMCommandParamHolder& _p3);
	static s32 UpdateVup(const VMCommandParamHolder& _p1, const VMCommandParamHolder& _p2, const VMCommandParamHolder& _p3);
	static s32 RemoveVup(const VMCommandParamHolder& _p1, const VMCommandParamHolder& _p2, const VMCommandParamHolder& _p3);
	static s32 StartTesting(const VMCommandParamHolder& _p1, const VMCommandParamHolder& _p2, const VMCommandParamHolder& _p3);
	static s32 Refresh(const VMCommandParamHolder& _p1, const VMCommandParamHolder& _p2, const VMCommandParamHolder& _p3);

public:
	VMVupManager();
	~VMVupManager();

	virtual void Create();
	virtual void Tick(f32 _fDeltaTime);

	Bool			AddVup(VMVup* _newVUP);
	Bool			RemoveVup(s32 _id);
	VMVup*			FindVup(s32 _id);
	const VMVup*	FindVup(s32 _id) const;

	void			StartTesting(s32 _id);
	void			Refresh();

	friend class MyCanvas;

private:
	 VUPMap						m_poVupMap;
	 Socket*					m_pRecvSocket;
	 Socket*					m_pSendSocket;
	 Thread*					m_pRecvThread;
	 MemPool<UDP_PACKWrapper>*	m_pUDPPackBuffer;
};

#endif