#ifndef __VM_SUMMARY_H__
#define __VM_SUMMARY_H__

#include "VMGlobalDef.h"
#include "VMProtocal.h"

class VMSummary : public Singleton<VMSummary>
{
	typedef std::string AgentName;
	struct SummaryInfo
	{
		static const int kTotalSummaryInfo;

		SummaryInfo()
		{
			m_VupCnt = 0;
			memset(m_StatusCnt, 0, sizeof(s32) * EVupStatus_Num);
			memset(m_TestPhaseCnt, 0, sizeof(s32) * ETestPhase_Num);
		}

		s32 m_VupCnt;
		s32 m_StatusCnt[EVupStatus_Num];
		s32 m_TestPhaseCnt[ETestPhase_Num];
	};
	typedef std::map<AgentName, SummaryInfo>			AgentSummaryMap;
	typedef std::map<AgentName, SummaryInfo>::iterator	AgentSummaryMapIterator;

	typedef std::string GroupName;
	struct GroupInfoData
	{
		GroupInfoData(){
			m_iCurVUPsInGroup = 0;
			m_iMaxVUPsInGroup = 0;
		}
		s32				 m_iCurVUPsInGroup;
		s32				 m_iMaxVUPsInGroup;
		std::deque<s32>	 m_VUPsPassport;
	};
	struct GroupInfo
	{
		GroupInfo(){
			m_TotalVups = 0;
		}
		int						   m_TotalVups;
		std::vector<GroupInfoData> m_Groups;
	};
	typedef std::map<GroupName, GroupInfo>				GroupInfoMap;
	typedef std::map<GroupName, GroupInfo>::iterator	GroupInfoMapIterator;

public:
	struct SummaryUpdateParam{
		enum{
			ESummaryUpdateType_VUP			= (1 << 0),
			ESummaryUpdateType_Status		= (1 << 1),
			ESummaryUpdateType_TestPhase	= (1 << 2),
		};
		u32				m_UpdateType;
		Bool			m_AddorRemove;	//true for add
		s32				m_CurStatus;
		s32				m_LastStatus;
		s32				m_CurTestPhase;
		s32				m_LastTestPhase;
	};
	struct GroupUpdateParam{
		Bool			m_AddorRemove;
		s32				m_MyGroup;
		s32				m_MaxNumberInGroup;
		s32				m_VUPsPassport;
	};
	void UpdateSummary(StringPtr _strAgentName, const SummaryUpdateParam& _pUpdateParam);
	void UpdateGroupInfo(s32 _iMajorID, s32 _iMinorID, s32 _iMaxGroup, const GroupUpdateParam& _pUpdateParam);
	void RemoveRDVPoint(s32 _iMajorID, s32 _iMinorID);

	void UpdateToWatch(WatchedInfos& _pWInfo, Bool _bHideSummary);

private:
	friend class MyCanvas;

	AgentSummaryMap m_AgentSummaryMap;
	GroupInfoMap	m_GroupInfoMap;
};

class VMSummaryUpdater : public Object
{
public:
	virtual void Create(){}
	virtual void Tick(f32 _fDeltaTime);
};

#endif