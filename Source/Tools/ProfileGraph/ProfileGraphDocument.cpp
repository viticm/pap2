////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : ProfileGraphDocument.cpp
//  Version     : 1.0
//  Creator     : dengchao
//  Create Date : 2006-01-05 16:43:32
//  Comment     : profile graph 
//
////////////////////////////////////////////////////////////////////////////////


#include  "stdafx.h"
#include  <algorithm>
#include  "ProfileGraphDocument.h"


namespace
{
	static char const KPG_FLAG[] = "KPG_2006";			// flag
	static char const KPG_FRAME[] = "KPF_2006";			// frame

	static char const KPG_VERSION[] = "1013";			// version
	static char const KPG_VERSION_1012[] = "1012";		// version
	static char const KPG_VERSION_1011[] = "1011";		// version
	static char const KPG_VERSION_1010[] = "1010";		// version

	static int  const KPG_HEADER_SIZE = sizeof(KPG_FLAG) + sizeof(KPG_VERSION);
}



////////////////////////////////////////////////
// constructors / destructors
CProfileGraphDocument::CProfileGraphDocument()
{
	m_pRoot = NULL;
	m_pCurrent = m_pRoot;

	memset( &m_propGraph, 0, sizeof( m_propGraph ) );
}


CProfileGraphDocument::~CProfileGraphDocument()
{
	Close();
}



////////////////////////////////////////////////
// open / close
bool CProfileGraphDocument::Open( char const* name )
{
	// node read
	if ( nodeRead( name ) )
	{
		// node write
		ASSERT( m_NodeIdx.size() == m_NodeVector.size() );
		if ( nodeImport( ) )
		{
			// import frame
			if ( nodeFrameImport( name ) )
			{
				return true;
			}
		}

	}


	return false;
}

bool CProfileGraphDocument::Close( )
{
	if ( NULL != m_pRoot ) delete m_pRoot;
	return true;
}


////////////////////////////////////////////////
// get node
CProfileGraphNode* CProfileGraphDocument::GetNode() const
{
	return m_pRoot;
}


CProfileGraphNode* CProfileGraphDocument::GetNodeByIndex( UINT idx ) const
{
	return nodeFindIndex( idx );
}

UINT CProfileGraphDocument::GetNodeNumber() const
{
	return m_NodeVector.size( );
}

UINT CProfileGraphDocument::GetNodeIndex() const
{
	return nodeFindVector( m_pRoot );
}

UINT CProfileGraphDocument::GetNodeIndexByPtr( CProfileGraphNode* pNode ) const
{
	return nodeFindVector( pNode );
}


////////////////////////////////////////////////
// get frame
int    CProfileGraphDocument::GetFrameNumber() const
{
	return (int)m_FrmNodeTable.size();
}

float  CProfileGraphDocument::GetFrameNodeTime( int nFrame, int nCount ) const
{
	if ( 0 < GetFrameNumber() )
	{
		return m_FrmNodeTable[nFrame][nCount];
	}


	return 0.0f;
}


////////////////////////////////////////////////
// get system info
char const* CProfileGraphDocument::GetCpuInfo() const
{
	return m_cpuInfo;
}

PropGraph_t const& CProfileGraphDocument::GetPropInfo() const
{
	return m_propGraph;
}

SYSTEM_INFO const&	CProfileGraphDocument::GetSystemInfo() const
{
	return m_sysInfo;
}

MEMORYSTATUS const&	CProfileGraphDocument::GetMemoryStatus() const
{
	return m_memStatus;
}

OSVERSIONINFOEX const& CProfileGraphDocument::GetOsVersionInfoEx() const
{
	return m_osVerInfo;
}


////////////////////////////////////////////////
// node import frame
bool CProfileGraphDocument::nodeFrameImport( char const* name )
{
	// save frame node file name
	std::string tempStr( name, name + strlen( name ) - 4 );

	tempStr += ".kpf";


	// open file name
	FILE* fp = fopen( tempStr.c_str(), "rb" );
	if ( NULL == fp )
	{
		m_FrmNodeTable.clear();
		m_FrmNodeIter = m_FrmNodeTable.end();

		return true;
	}


	// read frame file header
	char flag[sizeof(KPG_FLAG)] = {0};
	char version[sizeof(KPG_VERSION)] = {0};
	int  frames;
	
	fread( flag, 1, sizeof(flag), fp );			// flag
	fread( version, 1, sizeof(version), fp );	// version
	ASSERT( !strcmp( flag, KPG_FLAG ) );
	//ASSERT( !strcmp( version, KPG_VERSION ) );


	fseek( fp, KPG_HEADER_SIZE, SEEK_SET );
	fread( &frames, 1, sizeof( int ), fp );


	// read all frame 
	if ( !nodeFrameImport( fp, frames ) )
	{
		fclose( fp );
		return false;
	}


	fclose( fp );
	return true;
}

bool CProfileGraphDocument::nodeFrameImport( FILE* fp, int frame )
{
	// alloc frame
	//m_FrmNodeTable.resize( frame );


	// read frame
	int  KPF_HEADER_OFFSET = KPG_HEADER_SIZE;// + sizeof( int );
	int  KPF_FRAME_SIZE = 0;

	char flag[sizeof(KPG_FRAME)] = {0};
	int  nodes = 0;
	int  i = 0;


	fseek( fp, KPF_HEADER_OFFSET, SEEK_SET );
	for ( ; !feof( fp ); ++ i, ++ m_FrmNodeIter )
	{
		FrmNodeVector_t   frmNodeVector;


		fread( flag, 1, sizeof(KPG_FRAME), fp );	// frame
		fread( &nodes, 1, sizeof(int), fp );		// nodes
		ASSERT( !strcmp( flag, KPG_FRAME ) );

		
		frmNodeVector.resize( m_NodeVector.size() );
		for ( int k = 0; k <= nodes; ++ k )
		{
			int id = 0;
			float times = 0;
			
			nodeFrameRead( fp, id, times );
			frmNodeVector[id] = times;

		}


		m_FrmNodeTable.push_back( frmNodeVector );
		m_FrmNodeIter = m_FrmNodeTable.begin();
	}

	
	return true;
}


bool CProfileGraphDocument::nodeFrameRead( FILE* fp, int& id, float& times )
{
	////////////////////////////////////////////////
	// read counter / times
	fread( &id, 1, sizeof(int), fp );
	fread( &times, 1, sizeof(float), fp );
	return true;
}


////////////////////////////////////////////////
// import 
bool CProfileGraphDocument::nodeImport( )
{
	// import root node 
	m_pRoot = ( *m_NodeVector.begin() );
	m_pRoot->m_NodeCalls = 1;
	m_pRoot->m_NodeTimes = 0;

	if ( nodeImportVector( m_pRoot ) )
	{

	}


	// calc root node
	for ( CProfileGraphNode* pChild = m_pRoot->GetChildPtr(); 
		NULL != pChild; 
		pChild = pChild->GetSiblingPtr() 
		)
	{
		m_pRoot->m_NodeTimes += pChild->m_NodeTimes;
	}


	return true;
}


bool CProfileGraphDocument::nodeImportVector( CProfileGraphNode* pNode )
{
	// find node, child, parent, sibling index
	int const idxNode = nodeFindVector( pNode );
	int const idxOther = m_NodeIdx[idxNode];

	if ( 0x3FFFFFFF != idxOther )
	{
		int const idxChild = ( idxOther & 0x3FF );
		int const idxParent = ( ( idxOther >> 20 ) & 0x3FF );
		int const idxSibling = ( ( idxOther >> 10 ) & 0x3FF );


		pNode->m_pChild   = nodeFindIndex( idxChild );
		pNode->m_pParent  = nodeFindIndex( idxParent );
		pNode->m_pSibling = nodeFindIndex( idxSibling );

	
		// import child node
		for ( CProfileGraphNode* pChild = pNode->GetChildPtr(); 
			NULL != pChild; 
			pChild = pChild->GetSiblingPtr() 
			)
		{
			nodeImportVector( pChild );
		}
	
	}


	return true;
}



////////////////////////////////////////////////
// node write
bool CProfileGraphDocument::nodeRead( char const* name )
{
	FILE* fp = fopen( name, "rb" );
	if ( NULL == fp )
	{
		return false;
	}


	// write node
	nodeRead( fp );


	fclose( fp );
	return true;
}

bool CProfileGraphDocument::nodeRead( FILE* fp )
{
	// read header
	char flag[sizeof(KPG_FLAG)] = {0};
	char version[sizeof(KPG_VERSION)] = {0};
	int  nOldVer = 1010;
	int  number = 0;

	fread( flag, 1, sizeof(flag), fp );			// flag
	fread( version, 1, sizeof(version), fp );	// version
	fread( &number, 1, sizeof( int ), fp );

	ASSERT( !strcmp( flag, KPG_FLAG ) );
	//ASSERT( !strcmp( version, KPG_VERSION ) );
	if ( !strcmp( version, KPG_VERSION_1011 ) )
	{
		nOldVer = 1011;
	}
	else if ( !strcmp( version, KPG_VERSION_1012 ) )
	{
		nOldVer = 1012;
	}
	else if ( !strcmp( version, KPG_VERSION ) )
	{
		nOldVer = 1013;
	}


	// read system info
	if ( 1012 <= nOldVer )
	{
		fread( &m_sysInfo, 1, sizeof( m_sysInfo ), fp );
		fread( &m_memStatus, 1, sizeof( m_memStatus ), fp );
		fread( &m_osVerInfo, 1, sizeof( m_osVerInfo ), fp );

		fread( m_cpuInfo, 1, sizeof( m_cpuInfo ), fp );
		
		if ( 1013 == nOldVer )
		{
			bool isEnable = false;

			fread( &isEnable, 1, sizeof( isEnable), fp );
			if ( isEnable )
			{
				fread( &m_propGraph.m_nTotalTextures, 1, sizeof(m_propGraph.m_nTotalTextures), fp );
				fread( &m_propGraph.m_nTotalRenderTextures, 1, sizeof(m_propGraph.m_nTotalRenderTextures), fp );
				fread( &m_propGraph.m_nTotalDepthStencilTextures, 1, sizeof(m_propGraph.m_nTotalDepthStencilTextures), fp );

				fread( &m_propGraph.m_nTotalIBUsedMems, 1, sizeof(m_propGraph.m_nTotalIBUsedMems), fp );
				fread( &m_propGraph.m_nTotalVBUsedMems, 1, sizeof(m_propGraph.m_nTotalVBUsedMems), fp );

				fread( &m_propGraph.m_nTotalIndexBuffers, 1, sizeof(m_propGraph.m_nTotalIndexBuffers), fp );
				fread( &m_propGraph.m_nTotalVertexBuffers, 1, sizeof(m_propGraph.m_nTotalVertexBuffers), fp );

			}
		}

	}
	else
	{
		// get system info
		::GetSystemInfo( &m_sysInfo ); 
		::GlobalMemoryStatus( &m_memStatus );

		 
		// Try calling GetVersionEx using the OSVERSIONINFOEX structure.
		// If that fails, try using the OSVERSIONINFO structure.
		BOOL bOsVersionInfoEx;

		memset(&m_osVerInfo, 0, sizeof(OSVERSIONINFOEX));
		m_osVerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

		if (!(bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO*) &m_osVerInfo)))
		{
			m_osVerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

			if (!GetVersionEx((OSVERSIONINFO*) &m_osVerInfo)) 
			{

			}
		}


		// get cpu info
		char szName[64] = {0};

		#define CPU_ID __asm _emit 0xf __asm _emit 0xa2
		__asm
		{
			mov      eax, 0x80000002
			CPUID
			mov      DWORD PTR [szName+0],eax
			mov      DWORD PTR [szName+4],ebx
			mov      DWORD PTR [szName+8],ecx
			mov      DWORD PTR [szName+12],edx
			mov      eax, 0x80000003
			CPUID
			mov      DWORD PTR [szName+16],eax
			mov      DWORD PTR [szName+20],ebx
			mov      DWORD PTR [szName+24],ecx
			mov      DWORD PTR [szName+28],edx
			mov      eax, 0x80000004
			CPUID
			mov      DWORD PTR [szName+32],eax
			mov      DWORD PTR [szName+36],ebx
			mov      DWORD PTR [szName+40],ecx
			mov      DWORD PTR [szName+44],edx
		}
		#undef CPU_ID

		memcpy( m_cpuInfo, szName, sizeof(szName) );
	}


	// resize node table
	m_NodeIdx.resize( number );
	m_NodeVector.resize( number );



	// read node index table
	fread( &(*m_NodeIdx.begin() ), 1, sizeof( int ) * number, fp );
	ASSERT( m_NodeIdx.size() == number );


	// read node index table
	ASSERT( m_NodeVector.size() == number );
	NodeVector_t::iterator iter = m_NodeVector.begin();
	NodeVector_t::iterator iterEnd = m_NodeVector.end();

	for ( ; iter != iterEnd; ++ iter )
	{
		CProfileGraphNode* pNodeTemp = *iter;
		nodeRead( fp, nOldVer, &pNodeTemp );
		*iter = pNodeTemp;
	}


	return true;
}


bool CProfileGraphDocument::nodeRead( FILE* fp, int nOldVer, CProfileGraphNode** pNode )
{
	////////////////////////////////////////////////
	// read name / calls / times
	char tempName[256] = {0};
	int  sizeName = 0;
	int  sizeTotal = 0;
	int  sizeFloat = sizeof( float );// : sizeof( float ) * 3;
	int  sizeOther = sizeof( int ) + sizeof( int );

	if ( 1011 <= nOldVer )
	{
		sizeFloat = sizeof( float ) * 3;
	}
	
	if ( 1012 <= nOldVer )
	{
		sizeOther = sizeof( int ) * 3;
	}

	sizeOther += sizeFloat;
	fread( &sizeTotal, 1, sizeof(int), fp );
	fread( &sizeName, 1, sizeof(int), fp );
	ASSERT( sizeTotal == ( sizeName + sizeOther ) );

	fread(  tempName,  1, sizeName, fp );
	*pNode = new CProfileGraphNode( tempName, NULL );

	fread( &(*pNode)->m_NodeCalls, 1, sizeof(int), fp );
	fread( &(*pNode)->m_NodeTimes, 1, sizeof(float), fp );

	if ( 1012 <= nOldVer )
	{
		fread( &(*pNode)->m_NodeCounter, 1, sizeof(int), fp );

		fread( &(*pNode)->m_NodeTimeMax, 1, sizeof(float), fp );
		fread( &(*pNode)->m_NodeTimeMin, 1, sizeof(float), fp );

	}	
	else if ( 1011 == nOldVer )
	{
		fread( &(*pNode)->m_NodeTimeMax, 1, sizeof(float), fp );
		fread( &(*pNode)->m_NodeTimeMin, 1, sizeof(float), fp );
	}


	return true;
}



////////////////////////////////////////////////
// node find
CProfileGraphNode* CProfileGraphDocument::nodeFindIndex( UINT idx ) const
{
	if ( idx < m_NodeVector.size() )
	{
		return m_NodeVector.at( idx );
	}


	return NULL;
}

int  CProfileGraphDocument::nodeFindVector( CProfileGraphNode* pNode ) const
{
	NodeVector_t::const_iterator iter = std::find( m_NodeVector.begin(), m_NodeVector.end(), pNode );
	if ( iter != m_NodeVector.end() )
	{
		return (int)( iter - m_NodeVector.begin() );
	}


	return 0x3FF;
}


