#ifndef __TAI_STRINGID__
#define __TAI_STRINGID__

#include "TCore_Types.h"

#define STRINGID_USE_STRING 1

namespace TsiU
{
	namespace AI
	{
		class StringID
		{
		public:
			StringID():m_id(0)
#if STRINGID_USE_STRING
				,m_useString(false)
				,m_originalString(NULL)
#endif
			{
			}

			StringID(const StringID & str)
			{
				this->m_id = str.m_id;
#if STRINGID_USE_STRING
				this->m_originalString = str.m_originalString;		// for debug, we'll store a name, but don't use it, it may have been deleted
				this->m_useString = false;			
#endif
			}

			StringID( unsigned int id )
			{
				m_id = id;
#if STRINGID_USE_STRING
				m_originalString = NULL;
				m_useString = false;
#endif
			}

			~StringID()
			{
#if STRINGID_USE_STRING
				if(m_useString && m_originalString) delete m_originalString;	
#endif
			}

			StringID(const char * str, bool useString = false);

			unsigned int GetID() const { return m_id; }
			const char * GetString() const { 
#if STRINGID_USE_STRING
				return m_originalString;
#else
				return "Don't Support";
#endif
			}

			StringID & operator = (const StringID & str)
			{
#if STRINGID_USE_STRING
				if(m_useString && m_originalString) delete m_originalString;	
#endif

				this->m_id = str.m_id;
#if STRINGID_USE_STRING
				this->m_originalString = str.m_originalString;		// for debug, we'll store a name, but don't use it, it may have been deleted
				this->m_useString = false;
#endif

				return *this;
			}


			bool operator == (StringID str ) const;
			bool operator < (StringID id ) const;


			bool operator == (unsigned int id ) const
			{
				return this->m_id == id;
			}

			bool operator < (unsigned int id) const
			{
				return this->m_id < id;
			}

		private:
#if STRINGID_USE_STRING
			bool				m_useString;
			char * m_originalString;
#endif
			unsigned int		m_id;	

		public:
			static unsigned int Hash(const char * str);
		};
	}
}

#endif