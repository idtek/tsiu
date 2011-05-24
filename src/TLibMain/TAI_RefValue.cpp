#include "TAI_RefValue.h"

namespace TsiU
{
	namespace AI
	{
		RefValueManager::RefValueManager()
			: m_SharedMemory(0)
		{
#if PLATFORM_TYPE == PLATFORM_WIN32
			int size = sizeof(HeadInfo) * kMaxHeadCount + kMaxDataCount;
			m_SharedMemory = (char*)m_ProccessSM.Malloc(size, "AIRefValue Memory");
#endif
		}

		void RefValueManager::Flush()
		{
#if PLATFORM_TYPE == PLATFORM_WIN32
			//update writeable value
			m_ProccessSM.Lock();
			std::map<std::string, RefValueBase*>::iterator itWritable = m_WritableRefValues.begin();
			while(itWritable != m_WritableRefValues.end())
			{
				RefValueBase* val = (*itWritable).second;
				if(val->IsDirty())
				{
					D_CHECK(val->GetOffsetInMemory() != 0xffffffff);
					HeadInfo* hi = _GetHeadInfo(val->GetOffsetInMemory());
					D_CHECK(hi->m_Flags == EHeadFlag_InUse && !strncmp(val->GetName(), hi->m_VName, kMaxNameSize));
					char* dataMem = _GetDataSegment(hi->m_Offset);
					memcpy(dataMem, val->GetData(), val->GetSize());

					val->SetDirtyState(false);
				}
				++itWritable;
			}
			m_ProccessSM.UnLock();
			//update readonly 
			std::map<std::string, RefValueBase*>::iterator itReadOnly = m_ReadOnlyRefValues.begin();
			while(itReadOnly != m_ReadOnlyRefValues.end())
			{
				RefValueBase* val = (*itReadOnly).second;
				if(val->GetOffsetInMemory() == 0xffffffff)
				{
					HeadInfo* hi = _FindRefValueHeadInfo(val->GetName(), EHeadFlag_InUse);
					if(hi)
					{
						val->SetOffsetInMemory(_GetHeadInfoIndex((const char*)hi));
						break;
					}
				}
				unsigned int index = val->GetOffsetInMemory();
				if(index != 0xffffffff)
				{
					HeadInfo* hi = _GetHeadInfo(index);
					if(hi->m_Flags == EHeadFlag_InUse)
						val->SetData(_GetDataSegment(hi->m_Offset));
				}
				++itReadOnly;
			}
#endif
		}

		bool RefValueManager::AddRefValue(RefValueBase* val, unsigned int attr)
		{
			if(attr == ERefValuFlag_ReadOnly)
			{
				std::map<std::string, RefValueBase*>::iterator it = m_ReadOnlyRefValues.find(val->GetName());
				if(it != m_ReadOnlyRefValues.end())
				{
					return false;
				}
				m_ReadOnlyRefValues.insert(std::pair<std::string, RefValueBase*>(val->GetName(), val));
				return true;
			}
			else
			{
#if PLATFORM_TYPE == PLATFORM_WIN32
				HeadInfo* headInfoSegment = _FindRefValueHeadInfo(NULL, EHeadFlag_Available);
				if(!headInfoSegment)
					return false;

				char* dataSegment = _GetAvailableDataSegment(headInfoSegment, val->GetSize());
				if(!dataSegment)
					return false;

				m_ProccessSM.Lock();

				val->SetOffsetInMemory(_GetHeadInfoIndex((const char*)headInfoSegment));

				strncpy(headInfoSegment->m_VName, val->GetName(), kMaxNameSize - 1);
				headInfoSegment->m_VName[kMaxNameSize - 1] = '\0';
				headInfoSegment->m_Flags = EHeadFlag_InUse;
				headInfoSegment->m_VSize = val->GetSize();
				headInfoSegment->m_Offset = _GetDataSegmentOffset(dataSegment);
				memcpy(dataSegment, val->GetData(), val->GetSize());

				m_ProccessSM.UnLock();

				std::map<std::string, RefValueBase*>::iterator it = m_WritableRefValues.find(val->GetName());
				if(it != m_WritableRefValues.end())
				{
					return false;
				}
				m_WritableRefValues.insert(std::pair<std::string, RefValueBase*>(val->GetName(), val));

				return true;
#endif
			}
			return false;
		}
		bool RefValueManager::RemoveRefValue(RefValueBase* val, unsigned int attr)
		{
			if(attr == ERefValuFlag_ReadOnly)
			{
				std::map<std::string, RefValueBase*>::iterator it = m_ReadOnlyRefValues.find(val->GetName());
				if(it != m_ReadOnlyRefValues.end())
				{
					m_ReadOnlyRefValues.erase(it);
					return true;
				}
			}
			else
			{
				std::map<std::string, RefValueBase*>::iterator it = m_WritableRefValues.find(val->GetName());
				if(it != m_WritableRefValues.end())
				{
					m_WritableRefValues.erase(it);
				}
				m_ProccessSM.Lock();
				D_CHECK(val->GetOffsetInMemory() != 0xffffffff);
				HeadInfo* hi = _GetHeadInfo(val->GetOffsetInMemory());
				hi->m_Flags = EHeadFlag_CanDelete;
				m_ProccessSM.UnLock();

				return true;
			}
			return false;
		}
		RefValueManager::HeadInfo* RefValueManager::_FindRefValueHeadInfo(const char* name, unsigned char expectedFlag) const
		{
			HeadInfo* hiArray = (HeadInfo*)m_SharedMemory;
			for(int i = 0; i < kMaxHeadCount; ++i)
			{
				HeadInfo& hi = hiArray[i];
				if(hi.m_Flags == expectedFlag)
				{
					if(name == NULL || !strncmp(name, hi.m_VName, kMaxNameSize))
						return &hi;
				}
			}
			return NULL;
		}
		char* RefValueManager::_GetDataSegment(unsigned int offset) const
		{
			D_CHECK(offset >= 0 && offset < kMaxDataCount)
			return m_SharedMemory + sizeof(HeadInfo) * kMaxHeadCount + offset;
		}
		RefValueManager::HeadInfo* RefValueManager::_GetHeadInfo(unsigned int i) const 
		{
			D_CHECK(i >= 0 && i < kMaxHeadCount)
			return (HeadInfo*)(m_SharedMemory + sizeof(HeadInfo) * i);
		}
		char* RefValueManager::_GetAvailableDataSegment(const HeadInfo* hi, unsigned int size) const 
		{
			unsigned int offset = 0;
			unsigned int idx = _GetHeadInfoIndex((const char*)hi);
			if(idx)
			{
				HeadInfo* prevhi = _GetHeadInfo(idx - 1);
				offset = prevhi->m_Offset + prevhi->m_VSize;
			}
			if(kMaxDataCount - offset - size < 0)
				return NULL;
			return _GetDataSegment(offset);
		}
		unsigned int RefValueManager::_GetDataSegmentOffset(const char* addr) const
		{
			char* dataStart = _GetDataSegment(0);
			D_CHECK(addr);
			D_CHECK((int)(addr - dataStart) >= 0);
			unsigned int offset = (unsigned int)(addr - dataStart);
			D_CHECK(offset >= 0 && offset < kMaxDataCount);
			return offset;
		}
		unsigned int RefValueManager::_GetHeadInfoIndex(const char* addr) const 
		{
			D_CHECK(addr);
			D_CHECK((int)(addr - m_SharedMemory) >= 0);
			unsigned int offset = (unsigned int)(addr - m_SharedMemory);
			unsigned int index = offset / sizeof(HeadInfo);
			D_CHECK(index >= 0 && index < kMaxHeadCount);
			return index;
		}
	}
}