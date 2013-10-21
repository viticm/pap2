////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DMemHeap.cpp
//  Version     : 1.0
//  Creator     : dengchao
//  Create Date : 2006-10-24 14:43:32
//  Comment     : memory cahce heap
//
////////////////////////////////////////////////////////////////////////////////


#include  "stdafx.h"
#include  "KG3DMemHeap.h"


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace
{
	// common
	typedef int PAGE_SIZE_t;
	typedef unsigned char* PAGE_POINTER_t;

	static  unsigned int const PAGE_MINK		=	64;
	static  unsigned int const PAGE_SIZEK		=	1024;

	static  unsigned int const PAGE_MAGIC		=	0xFFFECD80;
	static  unsigned int const PAGE_HDR_MAGIC	=	0xFEFECD88;


	#define PAGE_ALIGN_SIZE			4
	#define PAGE_ALIGN_FACTOR		3

	#define PAGE_HDR_SIZE  sizeof( PAGE_HEADER_t )
	#define PAGE_HDR_CASTPTR( POINTER )  ( PAGE_HEADER_t* )( POINTER )


	struct  PAGE_HEADER_t		// 这里本身是按照 4 or 8 byte 对齐的
	{
		PAGE_HEADER_t*	First;
		PAGE_HEADER_t*	Next;
		
		PAGE_POINTER_t  Pointer;
		PAGE_SIZE_t		Size;

		UINT			Pages;		// 页数
		UINT			Magics;		// 验证数值
	};


	typedef std::list< PAGE_HEADER_t* >   PAGE_TABLE_t;
	typedef CRITICAL_SECTION PAGE_LOCK_T;

	#define PAGE_OWNER()	PageOwner_t  page_owner( M_PIMPL->PageLocker )
	struct PageOwner_t
	{
	public:
		explicit PageOwner_t( CRITICAL_SECTION& crit )
			:	m_crit( crit )
		{
			::EnterCriticalSection( &m_crit );
		}

		~PageOwner_t()
		{
			::LeaveCriticalSection( &m_crit );
		}


	private:
		CRITICAL_SECTION &m_crit;

		/*
		* No copies do not implement
		*/
		PageOwner_t( const PageOwner_t &rhs );
		PageOwner_t &operator=( const PageOwner_t &rhs );
	};

}


struct KG3DMemHeap::IMPL_T
{
	PAGE_SIZE_t		HunkSize;			// const
	PAGE_SIZE_t		HunkMinK;			// const, 每页内容缓冲大小
	PAGE_SIZE_t		HunkPageK;			// const, 每页实际占用内存大小

	PAGE_SIZE_t		HunkPages;			// const
	PAGE_SIZE_t		HunkFreePages;

	PAGE_HEADER_t*	PageSwap;
	PAGE_HEADER_t*	PageMalloc;

	PAGE_LOCK_T		PageLocker;


};



////////////////////////////////////////////////
// constructors / destructors
KG3DMemHeap::KG3DMemHeap()
				:	M_PIMPL( new IMPL_T )
{
	M_PIMPL->HunkSize		= 0;
	M_PIMPL->HunkMinK		= 0;
	M_PIMPL->HunkPageK		= 0;

	M_PIMPL->HunkPages		= 0;
	M_PIMPL->HunkFreePages	= 0;

	M_PIMPL->PageSwap		= NULL;
	M_PIMPL->PageMalloc		= NULL;


	::InitializeCriticalSection( &M_PIMPL->PageLocker );

}


KG3DMemHeap::~KG3DMemHeap()
{
	CleanUp();
	::DeleteCriticalSection( &M_PIMPL->PageLocker );

}


////////////////////////////////////////////////
// initialize / clean
bool  KG3DMemHeap::Initialize( UINT memBudget )
{
	// get system info
	MEMORYSTATUS memStatus;
	UINT  memPages = 0;
	UINT  memMinK = PAGE_MINK;

	::GlobalMemoryStatus( &memStatus );
	if ( memBudget > memStatus.dwTotalPhys / 3 )
	{
		//memBudget = ( UINT )( memStatus.dwTotalPhys / 3 );
	}


	memPages  = ( memBudget / memMinK );
	memBudget = memPages * ( ( memMinK * PAGE_SIZEK ) + PAGE_HDR_SIZE );


	// malloc hunk
	M_PIMPL->HunkSize		= memBudget;
	M_PIMPL->HunkMinK		= memMinK * PAGE_SIZEK;
	M_PIMPL->HunkPageK		= memMinK * PAGE_SIZEK + PAGE_HDR_SIZE;

	M_PIMPL->HunkPages		= memPages;
	M_PIMPL->HunkFreePages	= M_PIMPL->HunkPages;

	M_PIMPL->PageSwap		= NULL;
	M_PIMPL->PageMalloc		= NULL;



	return true;
}

void  KG3DMemHeap::CleanUp( )
{
	PAGE_OWNER();

	PAGE_HEADER_t* pPage = M_PIMPL->PageMalloc;					// free large-heap allocated pages
	while ( NULL != pPage )
	{
		PAGE_HEADER_t* next = pPage->Next;
		Free( pPage->Pointer );
		pPage = next;
	}

	if ( NULL != M_PIMPL->PageSwap ) 
	{
		Free( M_PIMPL->PageSwap->Pointer );
	}


	ASSERT( M_PIMPL->HunkFreePages == M_PIMPL->HunkPages );
	M_PIMPL->HunkSize		= 0;
	M_PIMPL->HunkMinK		= 0;
	M_PIMPL->HunkPages		= 0;
	M_PIMPL->HunkFreePages	= 0;


	M_PIMPL->PageSwap		= NULL;
	M_PIMPL->PageMalloc		= NULL;

}




////////////////////////////////////////////////
// malloc / free
void* KG3DMemHeap::Malloc( unsigned int memSize )
{
	PAGE_OWNER();

	void* memPtr = NULL;
	UINT  memPage = 0;
	UINT  memRealSize = memSize + PAGE_HDR_SIZE;
	

	memPage = memRealSize / M_PIMPL->HunkMinK;
	if ( 0 != ( memRealSize % M_PIMPL->HunkMinK ) )
	{
		++ memPage;
	}

	ASSERT( (INT)memPage <=  M_PIMPL->HunkFreePages );
	memRealSize = memPage * M_PIMPL->HunkMinK;

	if ( (INT)memPage <=  M_PIMPL->HunkFreePages )
	{
		PAGE_HEADER_t* pPage = NULL;


		// if we've got a swap page somewhere
		if ( M_PIMPL->PageSwap
			&& M_PIMPL->PageSwap->Pages == memPage )
		{
			pPage = M_PIMPL->PageSwap;
			M_PIMPL->PageSwap = NULL;
		}
		else 
		{
			pPage = PAGE_HDR_CASTPTR( _aligned_malloc( memRealSize, PAGE_ALIGN_SIZE ) );
			if ( NULL == pPage )
			{
				ASSERT( NULL != pPage );
			}


			pPage->Pointer	= (PAGE_POINTER_t)( pPage ) + PAGE_HDR_SIZE;
			pPage->Size		= memSize;			// 注意，这里不是实际字节数

			pPage->Pages	= memPage;			// 页数
			pPage->Magics	= PAGE_HDR_MAGIC;	// 验证数值
		}



		// adjust list
		pPage->First = NULL;
		pPage->Next	 = M_PIMPL->PageMalloc;
		
		if ( NULL != M_PIMPL->PageMalloc )
		{
			M_PIMPL->PageMalloc->First = pPage;
		}

		
		M_PIMPL->PageMalloc = pPage;
		M_PIMPL->HunkFreePages -= memPage;
		ASSERT( M_PIMPL->HunkFreePages >= 0 );

		memPtr = ( void* )( pPage->Pointer );

	}


	return memPtr;
}

void  KG3DMemHeap::Free( void* memPtr )
{
	PAGE_OWNER();

	if ( NULL != memPtr )
	{
		PAGE_POINTER_t pTemp = ( PAGE_POINTER_t )memPtr;
		PAGE_HEADER_t* pHdr = PAGE_HDR_CASTPTR( pTemp - PAGE_HDR_SIZE );
		ASSERT( pHdr->Magics == PAGE_HDR_MAGIC );

		if ( pHdr->Magics == PAGE_HDR_MAGIC )
		{
			// remove form malloc
			PAGE_HEADER_t* pFirst = pHdr->First;
			PAGE_HEADER_t* pNext  = pHdr->Next;
	
			if ( NULL != pFirst )
			{
				pFirst->Next = pNext;
			}

			if ( NULL != pNext )
			{
				pNext->First = pFirst;
			}


			if ( M_PIMPL->PageMalloc == pHdr )
			{
				 M_PIMPL->PageMalloc = pHdr->Next;
			}


			pHdr->First = NULL;
			pHdr->Next  = NULL;


			// to swap
			if ( NULL == M_PIMPL->PageSwap )
			{
				M_PIMPL->PageSwap = pHdr;
			}
			else
			{

				M_PIMPL->HunkFreePages += pHdr->Pages;
				ASSERT( M_PIMPL->HunkFreePages <= M_PIMPL->HunkPages );

				_aligned_free( pHdr );

			}


		}

	}

}


////////////////////////////////////////////////
// get budget
int   KG3DMemHeap::GetFreePages()
{
	PAGE_OWNER();

	return M_PIMPL->HunkFreePages;
}

int   KG3DMemHeap::GetMallocPages()
{
	PAGE_OWNER();

	return ( M_PIMPL->HunkPages - M_PIMPL->HunkFreePages );
}

