#ifndef __TAI_REFVALUE_H__
#define __TAI_REFVALUE_H__

#if PLATFORM_TYPE == PLATFORM_WIN32
#include "TUtility_ProcessSharedMemory.h"
#endif
#include "TUtility_Singleton.h"
#include <map>
#include <string>

namespace TsiU
{
	namespace AI
	{
		class RefValueManager;

		class RefValueBase
		{
		protected:
			static const unsigned int kMaxNameSize = 64;

		public:
			RefValueBase()
				: m_HasRegisted(false)
				, m_IsDirty(false)
				, m_OffsetInMemory(0xffffffff)
			{
				m_VName[0] = '\0';
			}
			RefValueBase(const char* registeredName)
				: m_HasRegisted(false)
				, m_IsDirty(false)
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
		protected:
			bool		 m_HasRegisted;
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
			RefValue()
				: RefValueBase()
				, m_Value()
			{}
			RefValue(const char* registeredName, const T& initValue)
				: RefValueBase(registeredName)
				, m_Value(initValue)
			{
				m_HasRegisted = RefValueManager::Get().AddRefValue(this, flag);
			}
			~RefValue()
			{
				if(m_HasRegisted)
					RefValueManager::Get().RemoveRefValue(this, flag);
			}
			bool RegisterValue(const char* registeredName, const T& initValue)
			{
				if(!m_HasRegisted)
				{
					strncpy(m_VName, registeredName, kMaxNameSize - 1);
					m_VName[kMaxNameSize - 1] = '\0';
					m_Value = initValue;

					m_HasRegisted = RefValueManager::Get().AddRefValue(this, flag);
				}
				return m_HasRegisted;
			}
			operator T() const
			{
				return As();
			}
			RefValue<T, flag>&	operator = (const RefValue<T, flag>& rhs)		{ WRITABLE_FLAG_CHECK(); m_Value =  rhs.m_Value; SetDirtyState(true); return (*this);	}
			RefValue<T, flag>&	operator +=(const RefValue<T, flag>& rhs)		{ WRITABLE_FLAG_CHECK(); m_Value += rhs.m_Value; SetDirtyState(true); return (*this);	}
			RefValue<T, flag>&	operator -=(const RefValue<T, flag>& rhs)		{ WRITABLE_FLAG_CHECK(); m_Value -= rhs.m_Value; SetDirtyState(true); return (*this);	}
			RefValue<T, flag>&	operator *=(const RefValue<T, flag>& rhs)		{ WRITABLE_FLAG_CHECK(); m_Value *= rhs.m_Value; SetDirtyState(true); return (*this);	}
			RefValue<T, flag>&	operator /=(const RefValue<T, flag>& rhs)		{ WRITABLE_FLAG_CHECK(); m_Value /= rhs.m_Value; SetDirtyState(true); return (*this);	}
			
			RefValue<T, flag>&	operator = (const T& val)						{ WRITABLE_FLAG_CHECK(); m_Value =  val;		 SetDirtyState(true); return (*this);	}
			RefValue<T, flag>&	operator +=(const T& val)						{ WRITABLE_FLAG_CHECK(); m_Value += val;		 SetDirtyState(true); return (*this);	}
			RefValue<T, flag>&	operator -=(const T& val)						{ WRITABLE_FLAG_CHECK(); m_Value -= val;		 SetDirtyState(true); return (*this);	}
			RefValue<T, flag>&	operator *=(const T& val)						{ WRITABLE_FLAG_CHECK(); m_Value *= val;		 SetDirtyState(true); return (*this);	}
			RefValue<T, flag>&	operator /=(const T& val)						{ WRITABLE_FLAG_CHECK(); m_Value /= val;		 SetDirtyState(true); return (*this);	}

			T 					operator - () const								{ return -m_Value;	}
			T 					operator + (const RefValue<T, flag>& rhs) const { return m_Value + rhs.m_Value;	}
			T 					operator - (const RefValue<T, flag>& rhs) const	{ return m_Value - rhs.m_Value;	}
			T 					operator * (const RefValue<T, flag>& rhs) const { return m_Value * rhs.m_Value;	}
			T 					operator / (const RefValue<T, flag>& rhs) const { return m_Value / rhs.m_Value;	}
			T 					operator + (const T& val) const 				{ return m_Value + val;	}
			T 					operator - (const T& val) const					{ return m_Value - val;	}
			T 					operator * (const T& val) const 				{ return m_Value * val;	}
			T 					operator / (const T& val) const 				{ return m_Value / val;	}

			bool				operator ==(const RefValue<T, flag>& rhs) const	{ return m_Value == rhs.m_Value;	}
			bool				operator ==(const T& val) const					{ return m_Value == val;			}
			bool				operator !=(const RefValue<T, flag>& rhs) const	{ return !operator==(rhs);			}
			bool				operator !=(const T& val) const					{ return !operator==(val);			}
			bool				operator <(const RefValue<T, flag>& rhs) const	{ return m_Value < rhs.m_Value;		}
			bool				operator <=(const T& val) const					{ return m_Value <= val;			}
			bool				operator >(const RefValue<T, flag>& rhs) const	{ return m_Value > rhs.m_Value;		}
			bool				operator >=(const T& val) const					{ return m_Value >= val;			}
			bool				operator !() const								{ return !m_Value;					}

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
			RefValue(const RefValue<T, flag>&);

		private:
			T m_Value;
		};

		typedef RefValue<int,	ERefValuFlag_Writable> RFInt;
		typedef RefValue<int,	ERefValuFlag_ReadOnly> RFCInt;
		typedef RefValue<float, ERefValuFlag_Writable> RFFloat;
		typedef RefValue<float, ERefValuFlag_ReadOnly> RFCFloat;
		typedef RefValue<bool,	ERefValuFlag_Writable> RFBool;
		typedef RefValue<bool,	ERefValuFlag_ReadOnly> RFCBool;

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
			RefValueManager();

			void Flush();
			bool AddRefValue(RefValueBase* val, unsigned int attr);
			bool RemoveRefValue(RefValueBase* val, unsigned int attr);

		private:
			HeadInfo*		_FindRefValueHeadInfo(const char* name, unsigned char expectedFlag) const;
			char*			_GetDataSegment(unsigned int offset) const;
			char*			_GetAvailableDataSegment(const HeadInfo* hi, unsigned int size) const;
			unsigned int	_GetDataSegmentOffset(const char* addr) const;
			HeadInfo*		_GetHeadInfo(unsigned int i) const;
			unsigned int	_GetHeadInfoIndex(const char* addr) const;

		private:
#if PLATFORM_TYPE == PLATFORM_WIN32
			ProccessSharedMemory m_ProccessSM;
#endif
			char*			m_SharedMemory;

			std::map<std::string, RefValueBase*> m_ReadOnlyRefValues;
			std::map<std::string, RefValueBase*> m_WritableRefValues;
		};
	}
}

#endif