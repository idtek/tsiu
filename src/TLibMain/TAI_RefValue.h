#ifndef __TAI_REFVALUE_H__
#define __TAI_REFVALUE_H__

namespace TsiU
{
	namespace AI
	{
		class RefValueBase
		{
		};

		enum
		{
			ERefValuFlag_ReadOnly,
			ERefValuFlag_Writable,
		};

		template<bool condition>
		struct FlagCheckByCompilingError
		{
			enum{
				check_flag_of_this_ref_value_to_see_if_its_readonly
			};
		};
		template<>
		struct FlagCheckByCompilingError<false>
		{};

#define WRITABLE_FLAG_CHECK() \
	FlagCheckByCompilingError<ERefValuFlag_Writable == flag>::check_flag_of_this_ref_value_to_see_if_its_readonly;

		template<typename T, unsigned int flag>
		class RefValue : public RefValueBase
		{
		public:
			RefValue(const char* registeredName, const T& initValue)
				: m_Value(initValue)
			{
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
		private:
			T m_Value;
		};

		//class IRefValueUpdater
		//{
		//public:
		//	virtual void Flush() = 0;
		//};

		//class RefValueManager : public Singleton<RefValueManager>
		//{
		//public:

		//private:

		//};
	}
}

#endif