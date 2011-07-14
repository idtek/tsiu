#include "VMSummary.h"
#include "VMVup.h"
#include <sstream>
#include <algorithm>

const int VMSummary::SummaryInfo::kTotalSummaryInfo = 1 + EVupStatus_Num + ETestPhase_Num - 1 - 1;	//exclude two invalid states

void VMSummary::UpdateSummary(StringPtr _strAgentName, const SummaryUpdateParam& _pUpdateParam)
{
	AgentSummaryMapIterator it = m_AgentSummaryMap.find(_strAgentName);
	if(it == m_AgentSummaryMap.end())
	{
		it = m_AgentSummaryMap.insert(std::pair<AgentName, SummaryInfo>(_strAgentName, SummaryInfo())).first;
	}
	SummaryInfo& info = (*it).second;
	if(_pUpdateParam.m_UpdateType & SummaryUpdateParam::ESummaryUpdateType_VUP)
	{
		_pUpdateParam.m_AddorRemove ? 
			++info.m_VupCnt : 
			--info.m_VupCnt;

		if(!info.m_VupCnt)
		{		
			m_AgentSummaryMap.erase(it);

			Event evt((EventType_t)E_ET_AgentLeave);
			evt.AddParam(_strAgentName);
			GameEngine::GetGameEngine()->GetEventMod()->SendEvent(&evt);

			return;
		}
	}
	if(_pUpdateParam.m_UpdateType & SummaryUpdateParam::ESummaryUpdateType_Status)
	{
		if(_pUpdateParam.m_LastStatus > EVupStatus_Invalid && _pUpdateParam.m_LastStatus < EVupStatus_Num)
			--info.m_StatusCnt[_pUpdateParam.m_LastStatus];
		if(_pUpdateParam.m_CurStatus > EVupStatus_Invalid && _pUpdateParam.m_CurStatus < EVupStatus_Num)
			++info.m_StatusCnt[_pUpdateParam.m_CurStatus];
	}
	if(_pUpdateParam.m_UpdateType & SummaryUpdateParam::ESummaryUpdateType_TestPhase)
	{
		if(_pUpdateParam.m_LastTestPhase > ETestPhase_INVALID && _pUpdateParam.m_LastTestPhase < ETestPhase_Num)
			--info.m_TestPhaseCnt[_pUpdateParam.m_LastTestPhase];
		if(_pUpdateParam.m_CurTestPhase > ETestPhase_INVALID && _pUpdateParam.m_CurTestPhase < ETestPhase_Num)
			++info.m_TestPhaseCnt[_pUpdateParam.m_CurTestPhase];
	}
}

void VMSummary::UpdateToWatch(WatchedInfos& _pWInfo, Bool _bHideSummary)
{
	//_pWInfo.Clear();

	s32 iTotalVUP = 0;
	s32 iStatusSummary[EVupStatus_Num] = {0};
	s32 iTestPhaseSummary[ETestPhase_Num] = {0};

	AgentSummaryMapIterator it;
	for(it = m_AgentSummaryMap.begin(); it != m_AgentSummaryMap.end(); ++it)
	{
		const SummaryInfo& info = (*it).second;
		iTotalVUP += info.m_VupCnt;
		for(s32 i = 0; i < EVupStatus_Num; ++i)
		{
			iStatusSummary[i] += info.m_StatusCnt[i];
		}
		for(s32 i = 0; i < ETestPhase_Num; ++i)
		{
			iTestPhaseSummary[i] += info.m_TestPhaseCnt[i];
		}
		_pWInfo.UpdateValue((*it).first.c_str(), info.m_VupCnt, _bHideSummary);
	}
	for(s32 i = 0; i < EVupStatus_Num; ++i)
	{
		_pWInfo.UpdateValue(VMVup::kStatus[i].GetName(), iStatusSummary[i], _bHideSummary);
	}
	for(s32 i = 0; i < ETestPhase_Num; ++i)
	{
		_pWInfo.UpdateValue(VMVup::kTestPhase[i].GetName(), iTestPhaseSummary[i], _bHideSummary);
	}
	_pWInfo.UpdateValue(SUMMARYNAME_TOTALVUP, iTotalVUP, _bHideSummary);
}
void VMSummary::RemoveRDVPoint(s32 _iMajorID, s32 _iMinorID)
{
	std::stringstream ss;
	ss << "RP(" << _iMajorID << ", " << _iMinorID << ")";
	GroupInfoMapIterator it = m_GroupInfoMap.find(ss.str());
	if(it != m_GroupInfoMap.end())
	{
		m_GroupInfoMap.erase(it);

		std::string name = ss.str();
		Event evt((EventType_t)E_ET_GroupSizeChanged);
		evt.AddParam(name.c_str());
		GameEngine::GetGameEngine()->GetEventMod()->SendEvent(&evt);
	}
}
void VMSummary::UpdateGroupInfo(s32 _iMajorID, s32 _iMinorID, s32 _iMaxGroup, const GroupUpdateParam& _pUpdateParam)
{
	std::stringstream ss;
	ss << "RP(" << _iMajorID << ", " << _iMinorID << ")";
	GroupInfoMapIterator it = m_GroupInfoMap.find(ss.str());
	if(it == m_GroupInfoMap.end())
	{
		if(_pUpdateParam.m_AddorRemove)
		{
			it = m_GroupInfoMap.insert(std::pair<GroupName, GroupInfo>(ss.str(), GroupInfo())).first;
		}
		else
		{
			return;
		}
	}
	GroupInfo& info = (*it).second;
	s32 myGroup = _pUpdateParam.m_MyGroup;
	if(_pUpdateParam.m_AddorRemove)
	{
		if(_iMaxGroup != info.m_Groups.size())
		{
			info.m_Groups.resize(_iMaxGroup);

			std::string name = ss.str();
			Event evt((EventType_t)E_ET_GroupSizeChanged);
			evt.AddParam(name.c_str());
			GameEngine::GetGameEngine()->GetEventMod()->SendEvent(&evt);
		}
		if(myGroup >= 0 && myGroup < _iMaxGroup)
		{
			info.m_TotalVups++;

			GroupInfoData& data = info.m_Groups[myGroup];
			data.m_iCurVUPsInGroup++;
			data.m_iMaxVUPsInGroup = _pUpdateParam.m_MaxNumberInGroup;
			data.m_VUPsPassport.push_back(_pUpdateParam.m_VUPsPassport);
		}
	}
	else
	{
		if(myGroup >= 0 && myGroup < info.m_Groups.size())
		{
			GroupInfoData& data = info.m_Groups[myGroup];
			data.m_iCurVUPsInGroup--;
			//data.m_iMaxVUPsInGroup = _pUpdateParam.m_MaxNumberInGroup;
			std::deque<UDTSOCKET>::iterator itPassport = find(data.m_VUPsPassport.begin(), data.m_VUPsPassport.end(), _pUpdateParam.m_VUPsPassport);
			if(itPassport != data.m_VUPsPassport.end())
			{
				data.m_VUPsPassport.erase(itPassport);
			}
			info.m_TotalVups--;
		}
		if(!info.m_TotalVups)
		{
			m_GroupInfoMap.erase(it);

			std::string name = ss.str();
			Event evt((EventType_t)E_ET_GroupSizeChanged);
			evt.AddParam(name.c_str());
			GameEngine::GetGameEngine()->GetEventMod()->SendEvent(&evt);
		}
	}
}
//----------------------------------------
void VMSummaryUpdater::Tick(f32 _fDeltaTime)
{
	//Update Summary
	Event evtSummary((EventType_t)E_ET_UIUpdateAgent);
	evtSummary.AddParam((void*)VMSummary::GetPtr());
	GameEngine::GetGameEngine()->GetEventMod()->SendEvent(&evtSummary);

	//Update Group
	Event evtGroup((EventType_t)E_ET_UIUpdateGroup);
	evtGroup.AddParam((void*)VMSummary::GetPtr());
	GameEngine::GetGameEngine()->GetEventMod()->SendEvent(&evtGroup);
}