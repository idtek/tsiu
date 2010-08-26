#ifndef __TCORE_PANIC__ 
#define __TCORE_PANIC__

#include "TCore_Creator.h"

namespace TsiU
{
	class Panic
	{
	public:
		virtual void FatalError(StringPtr _strInfo)	= 0;
		virtual void Error(StringPtr _strInfo)		= 0;
		virtual void Warning(StringPtr _strInfo)	= 0;
	};

	class DefaultPanic : public Panic
	{
	public:
		virtual void FatalError(StringPtr _strInfo);
		virtual void Error(StringPtr _strInfo);
		virtual void Warning(StringPtr _strInfo);
	
	private:
		void ShowInfo(const Char* _strInfo);
	};
	
	class DefaultPanicCreator : public Creator
	{
	public:
		virtual void* Create()
		{
			return (void*)new DefaultPanic;
		}
	};

	/************************************************************************/
	/* Global Func                                                          */
	/************************************************************************/
	extern Panic* g_poPanic;
}

#endif