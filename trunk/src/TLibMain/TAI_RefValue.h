#ifndef __TAI_REFVALUE_H__
#define __TAI_REFVALUE_H__

#if PLATFORM_TYPE == PLATFORM_WIN32
#include "TUtility_ProcessSharedMemory.h"
#endif
#include "TUtility_Singleton.h"

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
			{
				strncpy(m_VName, registeredName, kMaxNameSize - 1);
				m_VName[kMaxNameSize - 1] = '\0';
			}
			virtual ~RefValueBase()
			{}
			const char* GetName() const{
				return m_VName;
			}
			virtual unsigned int GetSize() const = 0;
			virtual const char* GetData() const = 0;
		private:
			char m_VName[kMaxNameSize];
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
				RefValueManager::Get().AddRefValue(this);
			}
			~RefValue()
			{
				RefValueManager::Get().RemoveRefValue(this);
			}
			operator T() const
			{
				return m_Value;
			}
			void operator =(const T& lhs)
			{
				WRITABLE_FLAG_CHECK();
				m_Value = lhs;
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
		private:
			T m_Value;
		};

		class IRefValueUpdater
		{
		public:
			virtual void Flush() = 0;
		};

		class RefValueManager : public Singleton<RefValueManager>
		{
			static const unsigned int kMaxNameSize = 64;
			static const unsigned int kMaxHeadCount = 512;
			static const unsigned int kMaxDataCount = 1024 * 1024;

			struct HeadInfo{
				char			m_VName[kMaxNameSize];
				unsigned char	m_Flags;
				unsigned int	m_VSize;
				unsigned int	m_Offset;
			};
		public:
			RefValueManager()
				: m_NextAvailableHeadOffset(0)
				, m_NextAvailableDataOffset(sizeof(HeadInfo) * kMaxHeadCount)
				, m_SharedMemory(0)
			{
#if PLATFORM_TYPE == PLATFORM_WIN32
				m_SharedMemory = (char*)m_ProccessSM.Malloc(sizeof(HeadInfo) * kMaxHeadCount + kMaxDataCount, "AIRefValue Memory");
#endif
			}

			bool AddRefValue(RefValueBase* val)
			{
#if PLATFORM_TYPE == PLATFORM_WIN32
				if(m_NextAvailableHeadOffset < _GetDataSegmentOffset() && 
				   m_NextAvailableDataOffset + val->GetSize() < sizeof(HeadInfo) * kMaxHeadCount + kMaxDataCount)
				{
					m_ProccessSM.Lock();

					HeadInfo* hi = (HeadInfo*)(m_SharedMemory + m_NextAvailableHeadOffset);
					strncpy(hi->m_VName, val->GetName(), kMaxNameSize - 1);
					hi->m_VName[kMaxNameSize - 1] = '\0';
					hi->m_VSize = val->GetSize();
					hi->m_Offset = m_NextAvailableDataOffset;

					memcpy(m_SharedMemory + _GetDataSegmentOffset() + m_NextAvailableDataOffset, val->GetData(), val->GetSize());

					m_NextAvailableHeadOffset += sizeof(HeadInfo);
					m_NextAvailableDataOffset += hi->m_VSize;

					m_ProccessSM.UnLock();

					return true;
				}
#endif
				return false;
			}
			bool RemoveRefValue(RefValueBase* val)
			{

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
		};
	}
}

#endif