////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : ProfileGraphDocument.h
//  Version     : 1.0
//  Creator     : dengchao
//  Create Date : 2006-01-05 16:43:32
//  Comment     : profile graph 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_PROFILEGRAPHDOCUMENT_H_
#define _INCLUDE_PROFILEGRAPHDOCUMENT_H_


#include  <vector>


struct  PropGraph_t
{
	int		m_nTotalTextures;
	int		m_nTotalRenderTextures;
	int		m_nTotalDepthStencilTextures;

	int		m_nTotalIBUsedMems;
	int		m_nTotalVBUsedMems;

	int		m_nTotalIndexBuffers;
	int		m_nTotalVertexBuffers;

};



//////////////////////////////////////////////////////////////////////
//   class  :   CProfileGraphNode
//   brief  :   profile node
//   author :   dengchao
//   log    :     
//               
//////////////////////////////////////////////////////////////////////
class CProfileGraphNode
{
public:
	////////////////////////////////////////////////
	// overload new / delete
	static void* operator new( size_t size )
	{
		return ::operator new( size );
	}

	static void operator delete( void *p, size_t size )
	{
		::operator delete( p );
	}


public:
	////////////////////////////////////////////////
	// constructors / destructors
	CProfileGraphNode( char const* name, CProfileGraphNode* parent )
		:	m_pChild( NULL ),
			m_pParent( parent ),
			m_pSibling( NULL ),
			m_NodeCalls( 0 ),
			m_NodeTimes( 0 ),
			m_NodeTimeMax( 0 ),
			m_NodeTimeMin( 0 )
	{
		strncpy( m_NodeName, name, sizeof( m_NodeName ) );
	}

	~CProfileGraphNode()
	{
		if ( NULL != m_pChild ) delete m_pChild;
		if ( NULL != m_pSibling ) delete m_pSibling;
	}


public:
	////////////////////////////////////////////////
	// get parent / sibling / child
	CProfileGraphNode* GetChildPtr() const	{	return	m_pChild;	}
	CProfileGraphNode* GetParentPtr() const	{	return	m_pParent;	}
	CProfileGraphNode* GetSiblingPtr() const	{	return	m_pSibling;	}



	////////////////////////////////////////////////
	// get name / calls / times
	char const*  GetName() const	{	return m_NodeName;		}
	int			 GetCalls() const	{	return m_NodeCalls;		}
	int			 GetCounter() const {   return m_NodeCounter;   }
	float		 GetTimes() const	{	return m_NodeTimes;		}

	float		 GetTimeMax() const	{	return m_NodeTimeMax;	}
	float		 GetTimeMin() const	{	return m_NodeTimeMin;	}


protected:
	////////////////////////////////////////////////
	// friend member
	friend class CProfileGraphDocument;


	////////////////////////////////////////////////
	// member
	CProfileGraphNode*	m_pChild;
	CProfileGraphNode*	m_pParent;
	CProfileGraphNode*	m_pSibling;


	char				m_NodeName[256];
	int					m_NodeCalls;
	int					m_NodeCounter;
	float				m_NodeTimes;

	float				m_NodeTimeMax;
	float				m_NodeTimeMin;

};



//////////////////////////////////////////////////////////////////////
//   class  :   CProfileGraphDocument
//   brief  :   profile document
//   author :   dengchao
//   log    :     
//               
//////////////////////////////////////////////////////////////////////
class CProfileGraphDocument
{
public:
	////////////////////////////////////////////////
	// constructors / destructors
	CProfileGraphDocument();
	~CProfileGraphDocument();


	////////////////////////////////////////////////
	// open / close
	bool Open( char const* name );
	bool Close( );


	////////////////////////////////////////////////
	// get node
	CProfileGraphNode* GetNode() const;
	CProfileGraphNode* GetNodeByIndex( UINT idx ) const;

	UINT GetNodeNumber() const;
	UINT GetNodeIndex() const;
	UINT GetNodeIndexByPtr( CProfileGraphNode* pNode ) const;


	////////////////////////////////////////////////
	// get frame
	int    GetFrameNumber() const;
	float  GetFrameNodeTime( int nFrame, int nCount ) const;


	////////////////////////////////////////////////
	// get system info
	char const* GetCpuInfo() const;
	PropGraph_t const& GetPropInfo() const;

	SYSTEM_INFO const&	GetSystemInfo() const;
	MEMORYSTATUS const&	GetMemoryStatus() const;
	OSVERSIONINFOEX const& GetOsVersionInfoEx() const;


protected:
	////////////////////////////////////////////////
	// node import frame
	bool nodeFrameImport( char const* name );
	bool nodeFrameImport( FILE* fp, int frame );
	bool nodeFrameRead( FILE* fp, int& id, float& times );


	////////////////////////////////////////////////
	// node import 
	bool nodeImport( );
	bool nodeImportVector( CProfileGraphNode* pNode );


	////////////////////////////////////////////////
	// node write
	bool nodeRead( char const* name );
	bool nodeRead( FILE* fp );
	bool nodeRead( FILE* fp, int nOldVer, CProfileGraphNode** pNode );


	////////////////////////////////////////////////
	// node find
	CProfileGraphNode* nodeFindIndex( UINT idx ) const;
	int  nodeFindVector( CProfileGraphNode* pNode ) const;



protected:
	////////////////////////////////////////////////
	// node member
	CProfileGraphNode*		m_pRoot;
	CProfileGraphNode*		m_pCurrent;


	////////////////////////////////////////////////
	// node table member
	typedef std::vector< CProfileGraphNode* >  NodeVector_t;
	typedef std::vector< int  >  NodeIdx_t;


	NodeVector_t	m_NodeVector;
	NodeIdx_t		m_NodeIdx;


	////////////////////////////////////////////////
	// node frame table member
	typedef std::vector< float > FrmNodeVector_t;
	typedef std::vector< FrmNodeVector_t > FrmNodeTable_t;
	typedef FrmNodeTable_t::iterator FrmNodeIter_t;


	FrmNodeTable_t	m_FrmNodeTable;
	FrmNodeIter_t	m_FrmNodeIter;



	////////////////////////////////////////////////
	// system info member
	char			m_cpuInfo[64];
	SYSTEM_INFO		m_sysInfo;
	MEMORYSTATUS	m_memStatus;
	OSVERSIONINFOEX m_osVerInfo;

	PropGraph_t		m_propGraph;



};



#endif //_INCLUDE_PROFILEGRAPHDOCUMENT_H_