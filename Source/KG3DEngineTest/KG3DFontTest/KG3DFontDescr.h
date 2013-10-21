////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DFontDescr.h
//  Version     : 1.0
//  Creator     : dengchao
//  Create Date : 2006-02-13 16:43:32
//  Comment     : font def
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DFONTDESCR_H_
#define _INCLUDE_KG3DFONTDESCR_H_



//////////////////////////////////////////////////////
//  special vc++ 8.0  warnings to disable.
//	This compiler emits warnings for calls to functions 
//	from the standard library which are deemed security risks. 
#pragma warning(disable : 4996)



////////////////////////////////////////////////
// include standard
#include  <windows.h>
#include  <tchar.h>

#include  <memory>
#include  <vector>
#include  <map>
#include  <hash_map>


////////////////////////////////////////////////
// include d3d9
#include  <d3d9.h>
#include  <d3dx9.h>



////////////////////////////////////////////////
// font type
enum  eKG3DFONT_TYPE
{
	//KG3DFONT_BITMAP,		// render use bitmap
	//KG3DFONT_OUTLINE,		// render use outline
	//KG3DFONT_ANTIALIAS,		// render use antialias texture
};



// font style
#define  KG3DFONT_STYLE_NONE			0x00
#define  KG3DFONT_STYLE_BORDER			0x01
#define  KG3DFONT_STYLE_VERTICAL		0x02
#define  KG3DFONT_STYLE_PROJECTION		0x04



// font enum
enum
{
	KG3DFONT_SLOT_X		= 0,
	KG3DFONT_SLOT_Y		= 1,
	//KG3DFONT_SLOT_Z		= 2
};



////////////////////////////////////////////////
// handy routines for converting from fixed point
#define KG3DFONT_FIXDIV( X )		( ( X ) / 64.0f )
#define KG3DFONT_FIXMULT( X )		( ( X ) * 64.0f )

#define KG3DFONT_CEIL( X )			( ( ( X + 63 ) & -64 ) / 64 )
#define KG3DFONT_FLOOR( X )			( ( X & -64 ) / 64 )



////////////////////////////////////////////////
// font point / dpi
UINT  const  KG3DFONT_DEFAULT_DPI		= 96;
UINT  const  KG3DFONT_DEFAULT_POINT		= 16;

UINT  const  KG3DFONT_DEFAULT_PADDING	= 3;	// 最大支持 2x2 的衬边
//UINT  const  KG3DFONT_DEFAULT_PADDING2	= 6;



////////////////////////////////////////////////
// memory limit
//UINT  const  KG3DFONT_MEMORY_GPU	=  1 * 256  * 256;		// GPU 内存限制
//UINT  const  KG3DFONT_MEMORY_LIMIT	=  1 * 1024 * 1024;		// OS 内存限制




////////////////////////////////////////////////
// font glyph
// 参考教育部 1998 年出版品之常用字统计资料库，
// 此资料库统计 1579771 个字词，其中常用字共有 5063 个。
// 不过，出现率最高的前 80% 只有 641 个字，
// 考虑到前瞻性，变态性( 有些国家的语言变态的说) 等等，
// 这里使用 1024 作为最大缓冲大小。

UINT  const  KG3DFONT_GLYPH_NUMS	= 1024;
UINT  const  KG3DFONT_GLYPH_LIMIT	= 0xFFFF;


struct KG3DFontGlyph_t
{
	INT		glyphIndex;
	INT		glyphFormat;

	FLOAT	glyphAdvance[2];	// the advance distance for this glyph
	FLOAT	glyphVAdvance[2];	// the advance distance for this glyph

	FLOAT	glyphBLower[2], glyphBUpper[2];	// the bounding box of this glyph.
};




////////////////////////////////////////////////
// font vertex / texture
UINT const KG3DFONT_NUMS_INDEX = 1024 * 6;
UINT const KG3DFONT_NUMS_VERTEX = 1024;
UINT const KG3DFONT_NUMS_TLVERTEX = 1024;

UINT const KG3DFONT_FVF_VERTEX = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;
UINT const KG3DFONT_FVF_TLVERTEX = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;



struct KG3DFontVertex_t
{
	FLOAT	x, y, z;
	UINT	color;
	float	u, v;
};

struct KG3DFontTLVertex_t
{
	FLOAT	x, y, z, w;
	UINT	color;
	float	u, v;
};




//////////////////////////////////////////////////////
//   assert declaration
//////////////////////////////////////////////////////

// run time
#include  <cassert>

#undef  KG3DFONT_ASSERT
#define KG3DFONT_ASSERT(condition)		\
	{if ( !((int) (condition))) {		\
		assert( condition );			\
	}}


#define KG3DFONT_MSG_ASSERT(condition)		KG3DFONT_ASSERT( !##condition );
#define KG3DFONT_MSG_INFO(info)				\
	{										\
		::MessageBox( NULL, (PCTSTR)info, TEXT("INFO"), MB_OK|MB_ICONINFORMATION );	\
		::DebugBreak();							\
	}

#define KG3DFONT_MSG_ERR(error)			\
	{										\
		HLOCAL hlocal = NULL;				\
		BOOL fOk = ::FormatMessage(			\
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,	\
			NULL, dwError, MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),	\
			(PTSTR) &hlocal, 0, NULL);										\
		if (fOk)								\
		{										\
			if (hlocal != NULL)					\
			{									\
				::MessageBox( NULL, (PCTSTR) LocalLock(hlocal), TEXT("INFO"), MB_OK|MB_ICONINFORMATION );	\
				::LocalFree(hlocal);			\
			}									\
		}										\
		::DebugBreak();							\
	}




// compiler time
namespace core_impl_template
{
	template< bool generic_flag > struct KG3DFONT_STATIC_FAILURE;
	template<> struct KG3DFONT_STATIC_FAILURE<true> { enum { value = 1 }; };

	template< int generic_value > struct KG3DFONT_STATIC_FAILURE_TEST{ };

}

// generic version
#undef	KG3DFONT_STATIC_ASSERT
#define KG3DFONT_STATIC_ASSERT(condition)		\
   typedef ::core_impl_template::KG3DFONT_STATIC_FAILURE_TEST<\
      sizeof(::core_impl_template::KG3DFONT_STATIC_FAILURE< (bool)( condition ) >)>\
         KG3DFONT_STR_JOIN(KG3DFONT_STATIC_ASSERT_TYPEDEF_, __COUNTER__)




//////////////////////////////////////////////////////
//   new/delete declaration
//////////////////////////////////////////////////////

// new/delete
#define KG3DFONT_NEW( POINTER, CONSTRUCTOR )		\
	try{ POINTER = new CONSTRUCTOR; }			\
	catch( std::bad_alloc )	{ POINTER = NULL; }

#define KG3DFONT_DELETE( POINTER )					\
	if ( NULL != POINTER )						\
	{											\
		delete POINTER;							\
		POINTER = NULL;							\
	}


// array new/delete
#define KG3DFONT_NEW_ARRAY( POINTER, CONSTRUCTOR, SIZE )		\
	try{ POINTER = new CONSTRUCTOR[SIZE]; }					\
	catch( std::bad_alloc )	{ POINTER = NULL; }


#define KG3DFONT_DELETE_ARRAY( POINTER )						\
	if ( NULL != POINTER )									\
	{														\
		delete [] POINTER;									\
		POINTER = NULL;										\
	}




#endif //_INCLUDE_KG3DFONTDESCR_H_