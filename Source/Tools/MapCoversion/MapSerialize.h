////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : MapSerialize.h
//  Version     : 1.0
//  Creator     : dengchao
//  Create Date : 2006-05-18 14:43:32
//  Comment     : terrain serialize
//
////////////////////////////////////////////////////////////////////////////////


#ifndef _INCLUDE_MAPSERIALIZE_H_
#define _INCLUDE_MAPSERIALIZE_H_


#include  "MapDescr.h"
#include  "MapDescrOld.h"



//////////////////////////////////////////////////////////////////////
//   class  :   CMapSerialize
//   brief  :
//   author :   dengchao
//   log    :     
//               
//////////////////////////////////////////////////////////////////////
//enum
//{
//	COMMIT_BACKUP,
//	COMMIT_MODIFYED,
//};

class CMapSerialize
{
public:
	////////////////////////////////////////////////
	// constructors / destructors
	CMapSerialize();
	~CMapSerialize();


protected:
	////////////////////////////////////////////////
	// noncopyable
	CMapSerialize( CMapSerialize const& rhs );
	CMapSerialize& operator=( CMapSerialize const& rhs );


public:
	////////////////////////////////////////////////
	// prepare / commit / rollback
	bool  Prepare( char const* PathName );
	bool  Commit( bool bModify );
	bool  Rollback( );

	bool  Split();
	bool  Merged();
	bool  Coversion();
	bool  Comparison( char const* SrcName, char const* DestName );


	////////////////////////////////////////////////
	// is Operator
	bool  IsFmtSplit();
	bool  IsFmtCompact();


	////////////////////////////////////////////////
	// header Operator
	int   GetMapObjs();
	int   GetMapRegions();


private:
	////////////////////////////////////////////////
	// make Operator
	bool  makeHeader( KG3DTerrainMapFileHeader& mapHeader );
	bool  makeSect( KG3DTerrainMapFileHeader& mapHeader );
	
	bool  makeRegionData( int region );
	bool  makeRegion( int i, BYTE*& pointer, int& size );


	////////////////////////////////////////////////
	// parse
	int   parseRegion( int i, BYTE* pointer );
	int   parseRegionSize( int i );


	////////////////////////////////////////////////
	bool  parseRequire();
	bool  parseEnsure();


private:
	////////////////////////////////////////////////
	// member
	CString		m_nameWrite;
	CString		m_nameReader;

	FILE*		m_fileWrite;
	FILE*		m_fileReader;


	////////////////////////////////////////////////
	// temp buffer member
	typedef BYTE*  TempBuf_T;

	TempBuf_T	m_tempWrite;
	TempBuf_T	m_tempReader;



	////////////////////////////////////////////////
	// coversion member
	typedef KG3DTerrainMapFileHeader MapOldHeader_T;
	typedef std::vector< KG3DTerrainRegionFileData > MapOldSectVector_T;
	typedef std::vector< MapFileSect_T > MapSectVector_T;

	MapOldHeader_T			m_mapOldHeader;
	MapOldSectVector_T		m_mapOldSectVector;


	MapFileHeader_T			m_mapFileHeader;
	MapSectVector_T			m_mapSectVector;


};



#endif // _INCLUDE_MAPSERIALIZE_H_