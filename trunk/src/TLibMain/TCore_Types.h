#ifndef __TCORE_TYPES__ 
#define __TCORE_TYPES__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <assert.h>

#ifdef NULL
#undef NULL
#endif

#define NULL				0

typedef unsigned char		u8;
typedef unsigned short		u16;
typedef unsigned int		u32;
typedef unsigned __int64	u64;

typedef	signed char			s8;
typedef signed short		s16;
typedef signed int  		s32;
typedef signed __int64		s64;

typedef char				Char;
typedef const char*			StringPtr;

typedef bool    			Bool;
typedef float				f32;
typedef double				f64;

typedef void*				Udef;

/************************************************************************/
/* static		-- s
   const		-- k
   unsigned     -- u
   pointer      -- po
   integer      -- i
   float        -- f
   boolean      -- b
   char         -- c
   string		-- str
   array        -- ar
   list         -- li
   hashmap      -- hm
   parameter    -- p_
   member       -- m_
   local		-- l_
   global		-- g_
   macro		-- D_
   enum         -- E_
   union		-- un

/************************************************************************/

#ifdef TLIB_DEBUG
#define D_CHECK(s)	assert(s);
#else
#define D_CHECK(s)
#endif

#ifdef TLIB_DEBUG
#define D_DebugOut	printf
#else
#define D_DebugOut
#endif

#define D_Output printf

#if !defined Unused
#define Unused(p) ((void)(p))
#endif

#define D_SafeDelete(p)		if(p)		{ delete p; p = NULL; }

#define D_Inline inline

#if _MSC_VER
#define PLATFORM_TYPE	PLATFORM_WIN32
#else
#define PLATFORM_TYPE	PLATFORM_NONE
#endif

#if PLATFORM_TYPE == PLATFORM_WIN32
	#pragma warning(disable:4244)
	#pragma warning(disable:4267)
	#pragma warning(disable:4018)
	#pragma warning(disable:4819)
	#pragma warning(disable:4067)
#endif

#endif