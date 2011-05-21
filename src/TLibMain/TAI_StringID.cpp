#include "TAI_StringID.h"

namespace TsiU
{
	namespace AI
	{
		u32 StringID::Hash(StringPtr str)
		{
			u32 l = (u32)strlen(str);
			u32 h =  l;  
			u32 step = ( l >> 6 ) + 1;  /* if string is too long, don't hash all its chars */
			for(int i = l; i >= step; i -= step) 
				h = h ^ ((h<<5)+(h>>2) + str[i-1]);
			return h;
		}

		StringID::StringID(StringPtr str, Bool useString)
#if STRINGID_USE_STRING
			:m_useString(useString)
#endif
		{

			if( str == NULL)
			{
				m_id = 0;
#if STRINGID_USE_STRING
				m_originalString = NULL;
				m_useString = false;
#endif
				return;
			}
#if STRINGID_USE_STRING
			if( m_useString )
			{	
				m_originalString = new Char[strlen(str) + 1];
				strcpy(m_originalString, str);
			}
			else
			{
				m_originalString = const_cast<char *> ( str );
			}
#endif
			m_id = Hash(str);		
		}


		Bool StringID::operator == (StringID str ) const
		{
#if STRINGID_USE_STRING
			if(this->m_useString && str.m_useString )
			{
				return 0 == strcmp(this->m_originalString, str.m_originalString);
			}
			else
#endif
			{
				return this->m_id == str.m_id;
			}
		}

		Bool StringID::operator < (StringID str) const
		{
#if STRINGID_USE_STRING
			if(this->m_useString && str.m_useString )
			{
				return 0 > strcmp(this->m_originalString, str.m_originalString);
			}
			else
#endif
			{
				return this->m_id < str.m_id;
			}
		}
	}
}