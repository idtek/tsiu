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

	typedef u16 RDVPointID;
	struct RDVPointInfo{
		RDVPointID	m_uiID;
		u16			m_uiExpectedNum;
		u16			m_uiTimeOut;
	};
	typedef std::map<RDVPointID, RDVPointInfo>				RDVPointList;
	typedef std::map<RDVPointID, RDVPointInfo>::iterator	RDVPointListIterator;

	struct RDVPointRunningInfo{
		RDVPointRunningInfo(){
			Reset();
		}
		void Reset(){
			m_bHasValidValue = false;
			m_uiCurrentRunningID = (RDVPointID)-1;
			m_fStartTime = 0.f;
			m_ClientList.Clear();
		}
		Bool IsValid() const{
			return m_bHasValidValue;
		}
		RDVPointID	m_uiCurrentRunningID;
		Bool		m_bHasValidValue;
		f32			m_fStartTime;
		Array<s32>	m_ClientList;
	};

public:
	static s32 AddVup(const VMCommand::ParamList& _paramList);
	static s32 UpdateVup(const VMCommand::ParamList& _paramList);
	static s32 RemoveVup(const VMCommand::ParamList& _paramList);
	static s32 StartTesting(const VMCommand::ParamList& _paramList);
	static s32 Refresh(const VMCommand::ParamList& _paramList);
	static s32 KillClient(const VMCommand::ParamList& _paramList);

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
	void			KillClient(s32 _id);

	friend class MyCanvas;

private:
	Bool _InitParameter();
	void _HandleUdpPack();
	void _UpdateRDVPoint();

private:
	VUPMap						m_poVupMap;
	Socket*						m_pRecvSocket;
	Socket*						m_pSendSocket;
	Thread*						m_pRecvThread;
	MemPool<UDP_PACKWrapper>*	m_pUDPPackBuffer;

	RDVPointList				m_poRDVList;
	RDVPointRunningInfo			m_RDVRunningInfo;

	u16							m_uiServerPort;
	u16							m_uiClientStartPort;
	u16							m_uiClientEndPort;
	std::string					m_strBroadCastAddress;
};

#endif