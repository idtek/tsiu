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
#define D_CHECK(s)	(void)((s) || (TsiU::InnerAssertion(#s, __FILE__, __LINE__), 0))
//#else
//#define D_CHECK(s)
//#endif

#endif