#ifndef __VM_VUPMANAGER_H__
#define __VM_VUPMANAGER_H__

#include "VMCommand.h"
#include "VMProtocal.h"

class VMVup;

struct UDP_PACKWrapper{
	UDP_PACK	m_InnerData;

#ifndef USE_UDT_LIB
	std::string	m_SrcIPAddress;
	u16			m_SrcPort;
#else
	UDTSOCKET	m_ClientSocket;
#endif
};

#ifndef USE_UDT_LIB
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
#else

class VMVupManager;


template <typename T>
class VMThreadSafeContainer
{
public:
	VMThreadSafeContainer()
	{
	}
	const T& GetContrainer() const{
		return m_Container;
	}
	T& RetrieveContrainer(){
		//D_Output("0x%x Lock\n", (int)&m_Mutex);
		m_Mutex.Lock();
		return m_Container;
	}
	const T& RetrieveContrainer() const{
		//D_Output("0x%x Lock\n", (int)&m_Mutex);
		m_Mutex.Lock();
		return m_Container;
	}
	void ReleaseContrainer() const{
		//D_Output("0x%x UnLock\n", (int)&m_Mutex);
		m_Mutex.UnLock();
	}

private:
	T				m_Container;
	mutable Mutex	m_Mutex;
};

class ListeningRunner : public IThreadRunner
{
public:
	ListeningRunner(VMVupManager* _pVUPMan, u16 _uiPort);

	virtual u32		Run();
	virtual void	NotifyQuit();

public:
	UDTSOCKET		m_pListeningSocket;
	u16				m_uiPort;
	VMVupManager*	m_pMan;
	Bool			m_bRequestStop;
};

class WorkingRunner : public IThreadRunner
{
public:
	WorkingRunner(VMVupManager* _pVUPMan, MemPool<UDP_PACKWrapper>* _pMempool);

	virtual u32		Run();
	virtual void	NotifyQuit();

private:
	VMVupManager*				m_pMan;
	MemPool<UDP_PACKWrapper>*	m_pUDPPackBuffer;
	Bool						m_bRequestStop;
};
#endif

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
			m_CurrentGoup = 0;
			m_CurrentNumberOfVUPInGroup = 0;
		}
		Bool IsValid() const{
			return m_bHasValidValue;
		}
		RDVPointID	m_uiCurrentRunningID;
		Bool		m_bHasValidValue;
		f32			m_fStartTime;
		Array<s32>	m_ClientList;
		s32			m_CurrentGoup;
		s32			m_CurrentNumberOfVUPInGroup;
	};

	struct ManagerParameter{
		s32			m_iIntervalOfEachGroup;
		s32			m_iDelayOfStartTime;
		s32			m_iGroupNum;
		s32			m_iVUPNumInEachGroup;
	};

public:
	static s32 AddVup(const VMCommand::ParamList& _paramList);
	static s32 UpdateVup(const VMCommand::ParamList& _paramList);
	static s32 RemoveVup(const VMCommand::ParamList& _paramList);
	static s32 StartTesting(const VMCommand::ParamList& _paramList);
	static s32 Refresh(const VMCommand::ParamList& _paramList);
	static s32 KillClient(const VMCommand::ParamList& _paramList);
	static s32 SetParameter(const VMCommand::ParamList& _paramList);

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

	void			SetParameter(StringPtr _pOption, const VMCommandParamHolder& _param);

#ifdef USE_UDT_LIB
	void											AddClientSocket(UDTSOCKET _pNewSocket);
	VMThreadSafeContainer<std::vector<UDTSOCKET>>&	GetClientSocket(){	return m_pClientSockets;	}
	Bool											AddVupBySocket(VMVup* _newVUP);
	Bool											LostConnection(UDTSOCKET _pLostConnection);
	VMVup*											FindVupBySocket(s32 _id);
#endif

	friend class MyCanvas;

private:
	Bool		_InitParameter();
	void		_HandleUdpPack();
	void		_UpdateRDVPoint();
	void		_UpdateSummary();

private:
	MemPool<UDP_PACKWrapper>*		m_pUDPPackBuffer;
#ifndef USE_UDT_LIB
	VUPMap						m_poVupMap;
	Socket*						m_pRecvSocket;
	Socket*						m_pSendSocket;
	Thread*						m_pRecvThread;
#else
	Thread*											m_ListeningThread;
	Thread*											m_WorkingThread;
	VMThreadSafeContainer<std::vector<UDTSOCKET>>	m_pClientSockets;
	VMThreadSafeContainer<VUPMap>					m_poVupMapByPassport;
	VMThreadSafeContainer<VUPMap>					m_poVupMapBySocket;
#endif

	RDVPointList				m_poRDVList;
	RDVPointRunningInfo			m_RDVRunningInfo;

	u16							m_uiServerPort;
	u16							m_uiClientStartPort;
	u16							m_uiClientEndPort;
	std::string					m_strBroadCastAddress;

	ManagerParameter			m_Parameters;
};

#endif