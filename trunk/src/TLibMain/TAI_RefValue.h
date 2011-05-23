#ifndef __TAI_REFVALUE_H__
#define __TAI_REFVALUE_H__

#if PLATFORM_TYPE == PLATFORM_WIN32
#include "TUtility_ProcessSharedMemory.h"
#endif
#include "TUtility_Singleton.h"
#include <map>

namespace TsiU
{
	namespace AI
	{
		class RefValueManager;

		class RefValueBase
		{
			static const unsigned int kMaxNameSize = 64;

		public:
			RefValueBase(const char* registeredName)
				: m_IsDirty(false)
				, m_OffsetInMemory(0xffffffff)
			{
				strncpy(m_VName, registeredName, kMaxNameSize - 1);
				m_VName[kMaxNameSize - 1] = '\0';
			}
			virtual ~RefValueBase()
			{}
			const char* GetName() const{
				return m_VName;
			}
			bool IsDirty() const{
				return m_IsDirty;
			}
			void SetDirtyState(bool val){
				m_IsDirty = val;
			}
			unsigned int GetOffsetInMemory() const{
				return m_OffsetInMemory;
			}
			void SetOffsetInMemory(unsigned int offset){
				m_OffsetInMemory = offset;
			}
			virtual unsigned int GetSize() const = 0;
			virtual const char* GetData() const = 0;
			virtual void SetData(const char* rawData) = 0;
		private:
			unsigned int m_OffsetInMemory;
			bool		 m_IsDirty;
			char		 m_VName[kMaxNameSize];
		};

		enum
		{
			ERefValuFlag_ReadOnly,
			ERefValuFlag_Writable,
		};

		template<bool condition>
		struct FlagCheckByCompilingError
		{
			typedef int check_flag_of_this_ref_value_failed[condition ? 1 : -1];
		};

#define WRITABLE_FLAG_CHECK() \
	FlagCheckByCompilingError<ERefValuFlag_Writable == flag> flagCheck;\
	D_Unused(flagCheck);

		template<typename T, unsigned int flag>
		class RefValue : public RefValueBase
		{
		public:
			RefValue(const char* registeredName, const T& initValue)
				: RefValueBase(registeredName)
				, m_Value(initValue)
			{
				RefValueManager::Get().AddRefValue(this, flag);
			}
			~RefValue()
			{
				RefValueManager::Get().RemoveRefValue(this);
			}
			operator T() const
			{
				return As();
			}
			void operator =(const T& lhs)
			{
				WRITABLE_FLAG_CHECK();
				m_Value = lhs;
				SetDirtyState(true);
			}
			bool operator ==(const RefValue<T, flag>& lhs) const
			{
				return m_Value == lhs.m_Value;
			}
			bool operator ==(const T& val) const
			{
				return m_Value == val;
			}
			bool operator !=(const RefValue<T, flag>& lhs) const
			{
				return !operator==(lhs);
			}
			bool operator !=(const T& val) const
			{
				return !operator==(val);
			}
			const T& As() const
			{
				return m_Value;
			}
			//Default Approach
			virtual unsigned int GetSize() const
			{
				return sizeof(T);
			}
			virtual const char* GetData() const
			{
				return (const char*)&m_Value;
			}
			virtual void SetData(const char* rawData)
			{
				memcpy((char*)&m_Value, rawData, GetSize());
			}
		private:
			T m_Value;
		};

		class IRefValueUpdater
		{
		public:
			virtual void Flush() = 0;
		};

		class RefValueManager : public Singleton<RefValueManager>, IRefValueUpdater
		{
			static const unsigned int kMaxNameSize = 64;
			static const unsigned int kMaxHeadCount = 64;
			static const unsigned int kMaxDataCount = 1024 * 1024;

			enum{
				EHeadFlag_Available,
				EHeadFlag_InUse,
				EHeadFlag_CanDelete,
			};

			struct HeadInfo{
				char			m_VName[kMaxNameSize];
				unsigned char	m_Flags;
				unsigned int	m_VSize;
				unsigned int	m_Offset;
			};
		public:
			RefValueManager()
				: m_NextAvailableHeadOffset(0)
				, m_NextAvailableDataOffset(0)
				, m_SharedMemory(0)
			{
#if PLATFORM_TYPE == PLATFORM_WIN32
				int size = sizeof(HeadInfo) * kMaxHeadCount + kMaxDataCount;
				m_SharedMemory = (char*)m_ProccessSM.Malloc(size, "AIRefValue Memory");
#endif
			}

			void Flush()
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
						HeadInfo* hi = (HeadInfo*)(m_SharedMemory + val->GetOffsetInMemory());
						D_CHECK(hi->m_Flags == EHeadFlag_InUse && !strncmp(val->GetName(), hi->m_VName, kMaxNameSize));
						char* dataMem = m_SharedMemory + _GetDataSegmentOffset() + hi->m_Offset;
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
						HeadInfo* hiArray = (HeadInfo*)m_SharedMemory;
						for(int i = 0; i < kMaxHeadCount; ++i)
						{
							const HeadInfo& hi = hiArray[i];
							if(hi.m_Flags == EHeadFlag_InUse && !strncmp(val->GetName(), hi.m_VName, kMaxNameSize))
							{
								val->SetOffsetInMemory(i * sizeof(HeadInfo));
								break;
							}
						}
					}
					unsigned int offset = val->GetOffsetInMemory();
					if(offset != 0xffffffff)
					{
						HeadInfo* hi = (HeadInfo*)(m_SharedMemory + offset);
						val->SetData(m_SharedMemory + _GetDataSegmentOffset() + hi->m_Offset);
					}
					++itReadOnly;
				}
#endif
			}

			bool AddRefValue(RefValueBase* val, unsigned int attr)
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
					std::map<std::string, RefValueBase*>::iterator it = m_WritableRefValues.find(val->GetName());
					if(it != m_WritableRefValues.end())
					{
						return false;
					}
					m_WritableRefValues.insert(std::pair<std::string, RefValueBase*>(val->GetName(), val));

#if PLATFORM_TYPE == PLATFORM_WIN32
					if(m_NextAvailableHeadOffset < _GetDataSegmentOffset() && 
					   m_NextAvailableDataOffset + val->GetSize() < sizeof(HeadInfo) * kMaxHeadCount + kMaxDataCount)
					{
						m_ProccessSM.Lock();

						val->SetOffsetInMemory(m_NextAvailableHeadOffset);

						HeadInfo* hi = (HeadInfo*)(m_SharedMemory + m_NextAvailableHeadOffset);
						strncpy(hi->m_VName, val->GetName(), kMaxNameSize - 1);
						hi->m_VName[kMaxNameSize - 1] = '\0';
						hi->m_Flags = EHeadFlag_InUse;
						hi->m_VSize = val->GetSize();
						hi->m_Offset = m_NextAvailableDataOffset;

						memcpy(m_SharedMemory + _GetDataSegmentOffset() + m_NextAvailableDataOffset, val->GetData(), val->GetSize());

						m_NextAvailableHeadOffset += sizeof(HeadInfo);
						m_NextAvailableDataOffset += hi->m_VSize;

						m_ProccessSM.UnLock();

						return true;
					}
#endif
				}
				return false;
			}
			bool RemoveRefValue(RefValueBase* val)
			{
				return false;
			}

		private:
			unsigned int _GetDataSegmentOffset(){
				return sizeof(HeadInfo) * kMaxHeadCount;
			}


		private:
#if PLATFORM_TYPE == PLATFORM_WIN32
			ProccessSharedMemory m_ProccessSM;
#endif
			char*			m_SharedMemory;
			unsigned int	m_NextAvailableHeadOffset;
			unsigned int	m_NextAvailableDataOffset;

			std::map<std::string, RefValueBase*> m_ReadOnlyRefValues;
			std::map<std::string, RefValueBase*> m_WritableRefValues;
		};
	}
}

#endif