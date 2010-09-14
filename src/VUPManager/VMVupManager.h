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
public:
	enum{
		ESort_Passport,
		ESort_RDVPoint,
		ESort_CurPhase,
		ESort_LastStatus,
		ESort_LastPhase,
		ESort_IP,
		ESort_Port,

		ESort_Num,
		ESort_Invalid = ESort_Num
	};
	enum{
		ESort_Up,
		ESort_Down
	};

private:
	typedef std::map<s32, VMVup*>					VUPMap;
	typedef std::map<s32, VMVup*>::iterator			VUPMapIterator;
	typedef std::map<s32, VMVup*>::const_iterator	VUPMapConstIterator;

	typedef u16 RDVPointID;;
	struct RDVPointRunningInfo{
		RDVPointRunningInfo(){
			Reset();
		}
		void Reset(){
			m_bHasValidValue = false;
			m_uiCurrentRunningID = (RDVPointID)-1;
			m_fStartTime = 0.f;
			m_ClientList.clear();
			m_GroupList.Clear();
			m_bIsGroupFull = false;
			//m_CurrentGoup = 0;
			//m_CurrentNumberOfVUPInGroup = 0;
		}
		Bool IsValid() const{
			return m_bHasValidValue;
		}
		RDVPointID			m_uiCurrentRunningID;
		Bool				m_bHasValidValue;
		f32					m_fStartTime;
		std::vector<s32>	m_ClientList;
		Array<s32>			m_GroupList;
		Bool				m_bIsGroupFull;
		//s32			m_CurrentGoup;
		//s32			m_CurrentNumberOfVUPInGroup;
	};
	struct RDVPointInfo{
		RDVPointID			m_uiID;
		u16					m_uiExpectedNum;
		u16					m_uiTimeOut;
		RDVPointRunningInfo	m_RunningInfo;
	};
	typedef std::map<RDVPointID, RDVPointInfo>				RDVPointList;
	typedef std::map<RDVPointID, RDVPointInfo>::iterator	RDVPointListIterator;

	struct RDVPointParameter
	{
		s32	m_iIntervalOfEachGroup;
		s32	m_iDelayOfStartTime;
		s32	m_iGroupNum;
		s32	m_iVUPNumInEachGroup;
	};
	typedef std::map<RDVPointID, RDVPointParameter>				RDVPointParameterList;
	typedef std::map<RDVPointID, RDVPointParameter>::iterator	RDVPointParameterListIterator;

	struct ManagerParameter{
		ManagerParameter()
			: m_iHideSummaryIfZero(false)
			, m_iFreezeList(true)
			, m_iSortUpOrDown(ESort_Up)
		{
			for(s32 i = 0; i < ESort_Num; ++i)
			{
				m_iSortOrder[i] = ESort_Invalid;
				m_Filters[i].m_bActiveFilter = false;
			}
		}
		s32					m_iHideSummaryIfZero;
		s32					m_iSortOrder[ESort_Num];
		s32					m_iSortUpOrDown;
		struct ViewFilter{
			Bool			m_bActiveFilter;
			std::string		m_strFilterName;
		};
		ViewFilter			m_Filters[ESort_Num];
		s32					m_iFreezeList;
	};

	typedef std::map<std::string, VMVup*>							VUPViewMap;
	typedef std::map<std::string, VMVup*>::iterator					VUPViewMapIterator;
	typedef std::map<std::string, VMVup*>::const_iterator			VUPViewMapConstIterator;
	typedef std::map<std::string, VMVup*>::reverse_iterator			VUPViewMapRIterator;
	typedef std::map<std::string, VMVup*>::const_reverse_iterator	VUPViewMapRConstIterator;

public:
	static s32 AddVup(const VMCommand::ParamList& _paramList);
	static s32 UpdateVup(const VMCommand::ParamList& _paramList);
	static s32 RemoveVup(const VMCommand::ParamList& _paramList);
	static s32 Refresh(const VMCommand::ParamList& _paramList);
	static s32 KillClient(const VMCommand::ParamList& _paramList);
	static s32 SetParameter(const VMCommand::ParamList& _paramList);
	static s32 GetParameter(const VMCommand::ParamList& _paramList);
	static s32 SetRDVParameter(const VMCommand::ParamList& _paramList);
	static s32 GetRDVParameter(const VMCommand::ParamList& _paramList);
	static s32 FindVUP(const VMCommand::ParamList& _paramList);
	static s32 Sort(const VMCommand::ParamList& _paramList);
	static s32 Filter(const VMCommand::ParamList& _paramList);

public:
	VMVupManager();
	~VMVupManager();

	virtual void Create();
	virtual void Tick(f32 _fDeltaTime);

	Bool			AddVup(VMVup* _newVUP);
	Bool			RemoveVup(s32 _id);
	VMVup*			FindVup(s32 _id);
	const VMVup*	FindVup(s32 _id) const;

	void			StartTesting(VMVup* _curVUP, __int64 _startTime);
	void			Refresh();
	void			KillClient(s32 _id);

	RDVPointParameter*	FindRDVParam(s32 _iMajor, s32 _iMiner);
	RDVPointParameter*	AddRDVParam(s32 _iMajor, s32 _iMiner);
	void				SetRDVParameter(RDVPointParameter& _pRDVParam, StringPtr _pOption, const VMCommandParamHolder& _param);
	void				GetRDVParameter(const RDVPointParameter& _pRDVParam, StringPtr _pOption) const;
	void				SetParameter(StringPtr _pOption, const VMCommandParamHolder& _param);
	void				GetParameter(StringPtr _pOption) const;
	void				SetSortOrder(s32 _iOrder, StringPtr _pOrder);
	void				SetSortUpOrDwon(StringPtr _pOrder);
	std::string 		GetViewKey(const VMVup* _curVUP) const;
	void				RefreshViewMap();
	void				SetFilter(StringPtr _pOrder, StringPtr _pName);

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
	void		_UpdateViewMap();

	//Event handler
	void onAgentLeave(const Event* _poEvent);
	void onVUPInfoChange(const Event* _poEvent);

private:
	MemPool<UDP_PACKWrapper>*		m_pUDPPackBuffer;
	WatchedInfos					m_WatchedInfo;

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
	VMThreadSafeContainer<VUPViewMap>				m_poVupViewMap;
	VMThreadSafeContainer<std::set<VMVup*>>			m_poDirtyVUP;
#endif

	RDVPointList				m_poRDVList;
	RDVPointParameterList		m_poRDVParam;

	u16							m_uiServerPort;
	u16							m_uiClientStartPort;
	u16							m_uiClientEndPort;
	std::string					m_strBroadCastAddress;

	ManagerParameter			m_Parameters;
};

#endif