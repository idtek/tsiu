#ifndef __TCORE_ASSERT__ 
#define __TCORE_ASSERT__

namespace TsiU
{
	D_Inline void InnerAssertion(StringPtr _strExpr, StringPtr _strFile, s32 _iLine)
	{
		D_Output("Assert failed: %s in %s : %d\n", _strExpr, _strFile, _iLine);

		//TJQ: cause crash
		*reinterpret_cast<int*>(0)=0;
	}
}
//#ifdef TLIB_DEBUG
#if defined(ASSERT_LEVEL)
	#define D_CHECK_P(S,P)	\
		if(P >= ASSERT_LEVEL)	\
			(void)((S) || (TsiU::InnerAssertion(#S, __FILE__, __LINE__), 0));
#else
	#define D_CHECK_P(S,P) (void)((S) || (TsiU::InnerAssertion(#S, __FILE__, __LINE__), 0));
#endif

#define D_CHECK(S)	D_CHECK_P(S,0)
//#else
//#define D_CHECK(s)
//#endif

#endif