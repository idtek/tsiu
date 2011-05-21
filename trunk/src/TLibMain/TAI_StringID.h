#ifndef __TAI_STRINGID__
#define __TAI_STRINGID__

#define STRINGID_USE_STRING 1

namespace TsiU
{
	namespace AI
	{
		class StringID
		{
		public:
			StringID()
				: m_id(0)
#if STRINGID_USE_STRING
				, m_useString(false)
				, m_originalString(NULL)
#endif
			{}

			StringID(const StringID& str)
			{
				this->m_id = str.m_id;
#if STRINGID_USE_STRING
				this->m_originalString = str.m_originalString;		// for debug, we'll store a name, but don't use it, it may have been deleted
				this->m_useString = false;			
#endif
			}

			StringID( u32 id )
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

			StringID(StringPtr str, Bool useString = false);

			u32 GetID() const { return m_id; }
			StringPtr GetString() const { 
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


			Bool operator ==(StringID str ) const;
			Bool operator < (StringID id ) const;


			Bool operator == (u32 id ) const
			{
				return this->m_id == id;
			}

			Bool operator < (u32 id) const
			{
				return this->m_id < id;
			}

		private:
#if STRINGID_USE_STRING
			Bool  m_useString;
			Char* m_originalString;
#endif
			u32	  m_id;	

		public:
			static u32 Hash(StringPtr str);
		};
	}
}

#endif