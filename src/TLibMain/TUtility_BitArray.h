#ifndef __TUTILITY_BITARRAY_H__
#define __TUTILITY_BITARRAY_H__

namespace TsiU
{
	typedef u32 BitArray;

	#define D_AddFlag(a, f)			(a |= (f))
	#define D_RemoveFlag(a, f)		(a &= ~(f))
	#define D_TestFlag(a, f)		(a & (f))
}

#endif