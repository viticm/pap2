////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : MapSerialize.cpp
//  Version     : 1.0
//  Creator     : dengchao
//  Create Date : 2006-05-18 14:43:32
//  Comment     : terrain serialize
//
////////////////////////////////////////////////////////////////////////////////


#include  "stdafx.h"
#include  <algorithm>
#include  <functional>
#include  "MapSerialize.h"



//////////////////////////////////////////////////////////////////////
//   class  :   CMapSerialize
//   brief  :
//   author :   dengchao
//   log    :     
//               
//////////////////////////////////////////////////////////////////////

#define  TEMP_BUFFER_WRITE			( 1024 * 1024 )
#define  TEMP_BUFFER_READER			( 1024 * 1024 )


#define  BLOCK_VERTEX	( 17 * 17 )
#define  BLOCK_TEXTURE	( 64 * 64 )


#define  BLOCK_ORDER_16( ORDER )		( ( ORDER ) / 16 )//( ( ( ORDER ) + ( 15 ) ) / 16 )
#define  BLOCK_UNORDER_16( ORDER )		( ( ORDER ) * 16 )




// array new/delete
#define SAFE_NEW_ARRAY( POINTER, CONSTRUCTOR, SIZE )		\
	try{ POINTER = new CONSTRUCTOR[SIZE]; }					\
	catch( std::bad_alloc )	{ POINTER = NULL; }


#define SAFE_DELETE_ARRAY( POINTER )						\
	if ( NULL != POINTER )									\
	{														\
		delete [] POINTER;									\
		POINTER = NULL;										\
	}





#define SAFE_READ( buffer, size )							\
	{	int read = fread( buffer, 1, (size_t)size,  m_fileReader );	\
		ASSERT( read == size );								\
	}

#define SAFE_WRITE( buffer, size )							\
	{	int write = fwrite( buffer, 1, (size_t)size, m_fileWrite );	\
		ASSERT( write == size );							\
	}



////////////////////////////////////////////////
// constructors
CMapSerialize::CMapSerialize()
			:	m_fileWrite( NULL ),
				m_fileReader( NULL )
{
	SAFE_NEW_ARRAY( m_tempWrite,  BYTE, TEMP_BUFFER_WRITE );
	SAFE_NEW_ARRAY( m_tempReader, BYTE, TEMP_BUFFER_READER );
}

CMapSerialize::~CMapSerialize()
{
	SAFE_DELETE_ARRAY( m_tempWrite );
	SAFE_DELETE_ARRAY( m_tempReader );
}


////////////////////////////////////////////////
// prepare / commit / rollback
bool  CMapSerialize::Prepare( char const* PathName )
{
	// check valid
	ASSERT( NULL != PathName );

	if ( NULL == PathName ) 
	{
		return false;
	}


	// prepare
	std::string	 nameWrite;
	std::string	 nameReader;

	nameReader.assign( PathName );
	nameWrite.assign( nameReader );
	nameWrite.erase( nameWrite.size() - 4 );
	nameWrite.insert( nameWrite.end(), '_' );




	// open read / write file
	m_nameReader = nameReader.c_str();
	m_nameWrite  = nameWrite.c_str();
	m_nameWrite.Format( "%s_%d.map", nameWrite.c_str(), ::GetTickCount() );


	m_fileWrite  = fopen( (LPCSTR)m_nameWrite, "wb" );
	m_fileReader = fopen( (LPCSTR)m_nameReader, "rb" );


	return ( m_fileWrite && m_fileReader );
}


bool  CMapSerialize::Commit( bool bModify )
{
	fclose( m_fileWrite );
	fclose( m_fileReader );

	if ( bModify )
	{
		::CopyFile( m_nameWrite, m_nameReader, FALSE );
		::DeleteFile( m_nameWrite );

	}


	return true;
}

bool  CMapSerialize::Rollback( )
{
	fclose( m_fileWrite );
	fclose( m_fileReader );

	::DeleteFile( m_nameWrite );
	return true;
}


bool  CMapSerialize::Split()
{
	// check valid
	if ( IsFmtSplit() )
	{
		return false;
	}


	return true;
}


bool  CMapSerialize::Merged()
{
	// check valid
	if ( !IsFmtSplit() )
	{
		return false;
	}


	return true;
}


bool  CMapSerialize::Coversion()
{
	// check valid
	if ( !parseRequire() )
	{
		return false;
	}

	if ( IsFmtCompact() )
	{
		return true;
	}


	// cleanup
	memset( &m_mapOldHeader, 0, sizeof( m_mapOldHeader ) );
	m_mapOldSectVector.clear();


	// make header
	bool flag;


	flag = this->makeHeader( m_mapOldHeader );
	ASSERT( flag && "makeHeader" );

	flag = this->makeSect( m_mapOldHeader );
	ASSERT( flag && "makeSect" );



	return true;
}

bool  CMapSerialize::Comparison( char const* SrcName, char const* DestName )
{
	FILE* fpRead1 = fopen( SrcName, "rb" );
	FILE* fpRead2 = fopen( DestName, "rb" );
	bool flag = false;

	
	fseek( fpRead1, 0, SEEK_END );
	fseek( fpRead2, 0, SEEK_END );
	if ( ftell( fpRead1 ) == ftell( fpRead2 ) )
	{
		int Size = ftell( fpRead1 );
		int Size1 = ftell( fpRead2 );
		int Counter = 0;

		BYTE* pReader1 = new BYTE [ Size + 1 ];
		BYTE* pReader2 = new BYTE [ Size1 + 1 ];
		BYTE* pTempReader1 = pReader1;
		BYTE* pTempReader2 = pReader2;
		
		fseek( fpRead1, 0, SEEK_SET );
		fseek( fpRead2, 0, SEEK_SET );

		fread( pReader1, 1, Size, fpRead1 );
		fread( pReader2, 1, Size1, fpRead2 );

		fclose( fpRead1 );
		fclose( fpRead2 );


		pReader1[Size] = pReader2[Size1] = 0;
		//for ( ; *pReader1 ++ == *pReader2 ++; ++ Counter )
		pReader1 += TERRAIN_HEADER_LENGTHS + sizeof( MapFileLump_T );// + sizeof( MapFileSect_T ) * 675;
		pReader2 += TERRAIN_HEADER_LENGTHS + sizeof( MapFileLump_T );// + sizeof( MapFileSect_T ) * 675;

		MapFileSect_T* pMapSect1 = ( MapFileSect_T* )pReader1;
		MapFileSect_T* pMapSect2 = ( MapFileSect_T* )pReader2;

		for ( int i = 0; i < 675; ++ i, ++ pMapSect1, ++ pMapSect2 )
		{
			ASSERT( pMapSect1->dwOffset == pMapSect2->dwOffset );
			ASSERT( pMapSect1->dwSize   == pMapSect2->dwSize   );
			ASSERT( pMapSect1->nGrassNums == pMapSect2->nGrassNums );
			ASSERT( pMapSect1->nWaterNums == pMapSect2->nWaterNums );
		}


		Size  -= TERRAIN_HEADER_LENGTHS + sizeof( MapFileLump_T ) + sizeof( MapFileSect_T ) * 675;
		Size1 -= TERRAIN_HEADER_LENGTHS + sizeof( MapFileLump_T ) + sizeof( MapFileSect_T ) * 675;

		pReader1 = ( BYTE* )pMapSect1;
		pReader2 = ( BYTE* )pMapSect2;

		for ( Counter = 0; Counter != Size1; ++ Counter )
		{
			 if ( *pReader1 ++ != *pReader2 ++ )
				 break ;
		}

		if ( Counter == Size )
		{
			flag = true;
		}


		delete[] pTempReader1;
		delete[] pTempReader2;
	}
	else
	{
		fclose( fpRead1 );
		fclose( fpRead2 );
	}


	return flag;
}

////////////////////////////////////////////////
// is Operator
bool  CMapSerialize::IsFmtSplit()
{
	// check valid
	if ( !IsFmtCompact() )
	{
		return false;
	}


	// read header
	MapFileHeader_T mapHeader;

	fseek( m_fileReader, 0, SEEK_SET );
	SAFE_READ( &mapHeader, sizeof( mapHeader ) );
	ASSERT( TERRAIN_FMTMARK == mapHeader.dwMask );

	if ( 0 != mapHeader.dwOptional )
	{
		return true;
	}


	return false;
}

bool  CMapSerialize::IsFmtCompact()
{
	// check valid
	if ( !parseRequire() )
	{
		return false;
	}


	// read header
	MapFileMask_T mapHeader;

	fseek( m_fileReader, 0, SEEK_SET );
	SAFE_READ( &mapHeader, sizeof( mapHeader ) );
	ASSERT( TERRAIN_FMTMARK == mapHeader.dwMask );

	if ( TERRAIN_FMTVER_COMPACT == mapHeader.dwVersion )
	{
		return true;
	}


	return false;
}


////////////////////////////////////////////////
// header Operator
int   CMapSerialize::GetMapObjs()
{
	int Nums = 0;
	return Nums;
}

int   CMapSerialize::GetMapRegions()
{
	int Nums = 0;

	if ( !IsFmtCompact() )
	{
		// read header
		KG3DTerrainMapFileHeader mapHeader;

		fseek( m_fileReader, 0, SEEK_SET );
		SAFE_READ( &mapHeader, sizeof( mapHeader ) );
		ASSERT( TERRAIN_FMTMARK == mapHeader.dwFileMask );
		ASSERT( TERRAIN_FMTVER_OLD == mapHeader.dwVersion );


		Nums = (mapHeader.nRegionEndX - mapHeader.nRegionStartX ) * (mapHeader.nRegionEndZ - mapHeader.nRegionStartZ );
	}
	else
	{
		// read header
		MapFileHeader_T mapHeader;

		fseek( m_fileReader, 0, SEEK_SET );
		SAFE_READ( &mapHeader, sizeof( mapHeader ) );
		ASSERT( TERRAIN_FMTMARK == mapHeader.dwMask );
		ASSERT( TERRAIN_FMTVER_COMPACT == mapHeader.dwVersion );


		Nums = mapHeader.dwRegions;
	}


	return Nums;
}


////////////////////////////////////////////////
// make Operator
bool  CMapSerialize::makeHeader( KG3DTerrainMapFileHeader& mapHeader )
{
	// check valid
	if ( !parseRequire() )
	{
		return false;
	}


	// read header
	fseek( m_fileReader, 0, SEEK_SET );
	SAFE_READ( &mapHeader, sizeof( mapHeader ) );
	ASSERT( TERRAIN_FMTMARK == mapHeader.dwFileMask );
	ASSERT( TERRAIN_FMTVER_OLD == mapHeader.dwVersion );


	// write map file header
	memset( &m_mapFileHeader, 0, sizeof( m_mapFileHeader ) );
	m_mapFileHeader.dwMask = TERRAIN_FMTMARK;
	m_mapFileHeader.dwVersion = TERRAIN_FMTVER_COMPACT;

	m_mapFileHeader.dwRegions  = 	(mapHeader.nRegionEndX - mapHeader.nRegionStartX ) * (mapHeader.nRegionEndZ - mapHeader.nRegionStartZ );
	m_mapFileHeader.dwOptional = 	0;

	m_mapFileHeader.nRegionStartX = mapHeader.nRegionStartX;
	m_mapFileHeader.nRegionStartZ = mapHeader.nRegionStartZ;
	m_mapFileHeader.nRegionEndX   = mapHeader.nRegionEndX;
	m_mapFileHeader.nRegionEndZ   = mapHeader.nRegionEndZ;

	m_mapFileHeader.dwNumTerrainVertexX = mapHeader.dwNumTerrainVertexX;
	m_mapFileHeader.dwNumTerrainVertexZ = mapHeader.dwNumTerrainVertexZ;

	m_mapFileHeader.dwNumRegionDeleted = mapHeader.dwExtend[0];


	fseek( m_fileWrite, 0, SEEK_SET );
	SAFE_WRITE( &m_mapFileHeader,sizeof(MapFileHeader_T) );


	char tempWrite[TERRAIN_HEADER_SKIPS] = {0};
	SAFE_WRITE( &tempWrite,sizeof(char) * TERRAIN_HEADER_SKIPS );
	ASSERT( TERRAIN_HEADER_LENGTHS == ftell( m_fileWrite ) );



	return true;
}


bool  CMapSerialize::makeSect( KG3DTerrainMapFileHeader& mapHeader )
{
	// load regions header together
	DWORD dwSize = (mapHeader.nRegionEndX - mapHeader.nRegionStartX ) * (mapHeader.nRegionEndZ - mapHeader.nRegionStartZ );
	bool  flag = false;


	flag = makeRegionData( dwSize );
	ASSERT( flag && "makeRegionData" );



	// make sector
	MapFileSect_T  mapFileSect;
	MapFileLump_T  mapFileSectLump = { TERR_LUMP_SECT, dwSize * sizeof( MapFileSect_T ) };

	m_mapSectVector.resize( dwSize );
	memset( &mapFileSect, 0, sizeof( mapFileSect ) );
	std::fill( m_mapSectVector.begin(), m_mapSectVector.end(), mapFileSect );

	ASSERT( TERRAIN_HEADER_LENGTHS == ftell( m_fileWrite ) );
	
	SAFE_WRITE( &mapFileSectLump, sizeof( MapFileLump_T ) );
	SAFE_WRITE( &( *m_mapSectVector.begin() ), dwSize * sizeof( MapFileSect_T ) );
	ASSERT( ( TERRAIN_HEADER_LENGTHS + dwSize * sizeof( MapFileSect_T ) + sizeof( MapFileLump_T ) ) == ftell( m_fileWrite ) );



	// make region
	BYTE* regionPointer = NULL;
	DWORD regionBegin = 0, regionOffset = ftell( m_fileWrite );
	int   regionSize = 0;
	int   regionSizeMax = 0, regionSizeMin = 1024*1024;


	mapFileSectLump.dwMask   = TERR_LUMP_REGION;
	mapFileSectLump.dwLength = 0;
	SAFE_WRITE( &mapFileSectLump, sizeof( MapFileLump_T ) );

	regionBegin  = regionOffset;
	regionOffset = ftell( m_fileWrite );

	for ( int i = 0; i < dwSize; ++ i )
	{
		ASSERT( regionOffset == ftell( m_fileWrite ) );
		makeRegion( i, regionPointer, regionSize );
		SAFE_WRITE( regionPointer, regionSize );

			
		// fill sector
		KG3DTerrainRegionFileData* pFileData = &m_mapOldSectVector.at(i);
		MapFileSect_T*  pMapFileSect = &m_mapSectVector.at( i );
		ASSERT( NULL != pFileData );
		ASSERT( NULL != pMapFileSect );


		// fill info
		pMapFileSect->m_nIndex_X  = pFileData->m_nIndex_X;
		pMapFileSect->m_nIndex_Z  = pFileData->m_nIndex_Z;

		pMapFileSect->dwTextureCount  = pFileData->m_vTextureInfo.TextureCount;
		pMapFileSect->nTextureIndex[0] = pFileData->m_vTextureInfo.TextureIndex[0];
		pMapFileSect->nTextureIndex[1] = pFileData->m_vTextureInfo.TextureIndex[1];
		pMapFileSect->nTextureIndex[2] = pFileData->m_vTextureInfo.TextureIndex[2];
		pMapFileSect->nTextureIndex[3] = pFileData->m_vTextureInfo.TextureIndex[3];


		pMapFileSect->dwOffset = regionOffset; 
		pMapFileSect->dwSize = regionSize;

		regionSizeMin = min( regionSizeMin, regionSize ); 
		regionSizeMax = max( regionSizeMax, regionSize );

		regionOffset += regionSize;
		regionSize    = 0;

	}


	mapFileSectLump.dwMask   = TERR_LUMP_REGION;
	mapFileSectLump.dwLength = regionOffset - regionBegin;



	// ¸²Ð´ sect
	fseek( m_fileWrite, TERRAIN_HEADER_LENGTHS, SEEK_SET );
	fseek( m_fileWrite, sizeof( MapFileLump_T ), SEEK_CUR );
	//ASSERT( TERRAIN_HEADER_LENGTHS == ftell( m_fileWrite ) );
	SAFE_WRITE( &( *m_mapSectVector.begin() ), dwSize * sizeof( MapFileSect_T ) );
	ASSERT( ( TERRAIN_HEADER_LENGTHS + dwSize * sizeof( MapFileSect_T ) + sizeof( MapFileLump_T ) ) == ftell( m_fileWrite ) );



	// ¸²Ð´ region mask
	fseek( m_fileWrite, regionBegin, SEEK_SET );
	ASSERT( ( TERRAIN_HEADER_LENGTHS + dwSize * sizeof( MapFileSect_T ) + sizeof( MapFileLump_T ) ) == ftell( m_fileWrite ) );
	SAFE_WRITE( &mapFileSectLump, sizeof( MapFileLump_T ) );


	// info
	CString  tempInfo;

	tempInfo.Format( " map = %s,\r\n region max = %d, min = %d", m_nameReader, regionSizeMax, regionSizeMin );
	::MessageBox( NULL, tempInfo, "INFO", MB_OK );
	return true;
}

bool  CMapSerialize::makeRegionData( int region )
{
	// load regions header together
	DWORD dwSize = region;
	DWORD Version = 0 ;
	DWORD BlockEnd = 0 ;

	fseek( m_fileReader, m_mapOldHeader.dwRegionsBlock, SEEK_SET );
	SAFE_READ( &Version, sizeof( DWORD ) );

	for ( DWORD i = 0; i < dwSize ; ++ i )
	{
		KG3DTerrainRegionFileData regionFileData;

		SAFE_READ( &regionFileData,sizeof(KG3DTerrainRegionFileData) );
		ASSERT( -1 != regionFileData.m_vTextureInfo.TextureIndex[0] );
		m_mapOldSectVector.push_back( regionFileData );
	}

	SAFE_READ( &BlockEnd,sizeof(DWORD) );
	ASSERT( TERRAIN_FMTBLOCKEND == BlockEnd );


	return true;
}

bool  CMapSerialize::makeRegion( int i, BYTE*& pointer, int& size )
{
	ASSERT( m_mapOldSectVector.size() > i );
	KG3DTerrainRegionFileData* pFileData = &m_mapOldSectVector.at(i);
	ASSERT( NULL != pFileData );

	size  = parseRegionSize( i );
	ASSERT( 0 < size );

	//size += sizeof( MapFileRegion_T );
	
	//SAFE_NEW_ARRAY( pointer, char, size );
	pointer = m_tempReader;
	ASSERT( NULL != pointer );
	ASSERT( TEMP_BUFFER_READER >= size );

	memset( pointer, 0, size );
	int newSize = parseRegion( i, pointer );
	ASSERT( size >= newSize );
	size = newSize;


	return true;
}


////////////////////////////////////////////////
// parse
int   CMapSerialize::parseRegion( int i, BYTE* pointer )
{
	ASSERT( m_mapOldSectVector.size() > i );
	KG3DTerrainRegionFileData* pFileData = &m_mapOldSectVector.at(i);
	MapFileSect_T*  pMapFileSect = &m_mapSectVector.at( i );
	ASSERT( NULL != pFileData );
	ASSERT( NULL != pMapFileSect );

	MapFileRegion_T* pMapRegion = reinterpret_cast< MapFileRegion_T* >( pointer );
	DWORD BlockVaildate = 0;
	int Size = 0;


	pointer += sizeof( MapFileRegion_T );
	Size    += sizeof( MapFileRegion_T );

	memset( pMapRegion, 0, sizeof( MapFileRegion_T ) );


	// fill vertex block
	if ( pFileData->m_dwVertexBlock )
	{
		MapFileRegionVB_T  tempVB[ BLOCK_VERTEX ];
		OLD_RegionVB  tempOldVB[ BLOCK_VERTEX ];

		fseek( m_fileReader, pFileData->m_dwVertexBlock, SEEK_SET );
		fseek( m_fileReader, sizeof(DWORD), SEEK_CUR ); // Version 

		SAFE_READ( &tempOldVB, ( BLOCK_VERTEX * sizeof(OLD_RegionVB) ) );
		SAFE_READ( &BlockVaildate, sizeof(BlockVaildate) );
		ASSERT( BlockVaildate == TERRAIN_FMTBLOCKEND );
		
		for ( int i = 0; i < BLOCK_VERTEX; ++ i )
		{
			tempVB[i].Height = tempOldVB[i].Height;
			
			D3DXVec3Normalize( &tempOldVB[i].Normal, &tempOldVB[i].Normal );

			tempOldVB[i].Normal += D3DXVECTOR3( 1.0f, 1.0f, 1.0f );
			tempOldVB[i].Normal /= 2.0f;
			tempOldVB[i].Normal *= 255.0f;
			//tempVB[i].Normal = D3DCOLOR_RGBA( 0, (BYTE)tempOldVB[i].Normal.x, (BYTE)tempOldVB[i].Normal.y, (BYTE)tempOldVB[i].Normal.z );
			tempVB[i].NormalR = (BYTE)tempOldVB[i].Normal.x;
			tempVB[i].NormalG = (BYTE)tempOldVB[i].Normal.y;
			tempVB[i].NormalB = (BYTE)tempOldVB[i].Normal.z;
		}


		memcpy( pointer, &tempVB, ( BLOCK_VERTEX * sizeof(MapFileRegionVB_T) ) );
		pMapRegion->m_dwVertexBlock = ( BLOCK_VERTEX * sizeof(MapFileRegionVB_T) );
		
		Size += ( BLOCK_VERTEX * sizeof(MapFileRegionVB_T) );
		pointer += ( BLOCK_VERTEX * sizeof(MapFileRegionVB_T) );
	}



	// fill blend texture block
	if ( pFileData->m_dwBlendTextureBlock )
	{
		// read length
		int Length = 0;

		fseek( m_fileReader, pFileData->m_dwBlendTextureBlock, SEEK_SET );
		fseek( m_fileReader, sizeof(DWORD), SEEK_CUR ); // Version 

		SAFE_READ( &Length, sizeof(int) );


		// read buffer
		BYTE* TexBuffer = m_tempWrite;
		//SAFE_NEW_ARRAY( TexBuffer, BYTE, Length );
		ASSERT( NULL != TexBuffer );
		ASSERT( TEMP_BUFFER_WRITE >= Length );
		ASSERT( TEMP_BUFFER_WRITE >= ( BLOCK_TEXTURE * 6) );
		

		SAFE_READ( TexBuffer, Length );
		SAFE_READ( &BlockVaildate, sizeof(BlockVaildate) );
		ASSERT( BlockVaildate == TERRAIN_FMTBLOCKEND );

		ASSERT( TexBuffer[2] == 2 ); // image type, Uncompressed, RGB images
		ASSERT( ( TexBuffer[16] == 24 ) || ( TexBuffer[16] == 32 ) ); // pixel size

		// compact buffer
		WORD* pTexCompact = (WORD*)TexBuffer;
		DWORD const  nTexSkip   = Length - ( BLOCK_TEXTURE * 4 );
		DWORD const* pTexReader = (DWORD*)( TexBuffer + nTexSkip );
		DWORD const* pTexEnd    = pTexReader + BLOCK_TEXTURE;


		UINT TexSize = 0;
		for ( ; pTexReader != pTexEnd; )
		{
			DWORD lhsValue = *pTexReader ++;
			DWORD lastValue = 0;


			lastValue <<= 4; lastValue |= BLOCK_ORDER_16( ( lhsValue >> 24 ) & 0xFF );
			lastValue <<= 4; lastValue |= BLOCK_ORDER_16( ( lhsValue >> 16 ) & 0xFF );
			lastValue <<= 4; lastValue |= BLOCK_ORDER_16( ( lhsValue >>  8 ) & 0xFF );
			lastValue <<= 4; lastValue |= BLOCK_ORDER_16( ( lhsValue >>  0 ) & 0xFF );

			* pTexCompact ++ = (WORD)( lastValue & 0xFFFF );
			++ TexSize;

		}



		// write buffer
		BYTE  TempBuffer[ BLOCK_TEXTURE * 4 ] = {0};
		DWORD TempBytes = 0;

		WORD const* pTgaReader = (WORD*)TexBuffer;
		WORD const* pTgaEnd    = pTgaReader + TexSize;
		DWORD CompressSize = 0, CompressNums = 0;

		for ( ; pTgaReader != pTgaEnd; )
		{
			WORD  rleValue = 0;
			BYTE  rleLength = 0;


			// compressed
			for (  rleValue = *pTgaReader; 
				( pTgaReader != pTgaEnd ) && ( rleValue == (*pTgaReader) ) && ( rleLength < 128 );
				++ rleLength, ++ pTgaReader )
			{
			}

			if ( 0 != rleLength )
			{
				ASSERT( rleLength <= 128 );
				TempBuffer[ TempBytes ++ ] = ( rleLength - 1 ) | 0x80;
				TempBuffer[ TempBytes ++ ] = ( rleValue & 0xFF );
				TempBuffer[ TempBytes ++ ] = ( ( rleValue >> 8 ) & 0xFF );

				CompressSize += rleLength;
				CompressNums ++ ;

				continue ;
			}


			// no compressed
			while ( ( pTgaReader != pTgaEnd ) && ( rleValue != (*pTgaReader) ) && ( rleLength < 128 ) )
			{
				++ rleLength;
				rleValue = *pTgaReader ++;
			}

			if ( 0 != rleLength )
			{
				ASSERT( rleLength <= 128 );
				TempBuffer[ TempBytes ++ ] = ( rleLength - 1 );
				memcpy( &TempBuffer[ TempBytes ], pTgaReader - rleLength - 1, rleLength << 1 );

				TempBytes  += rleLength << 1;
				pTgaReader --;
				CompressSize += rleLength;
			}

		}


		if ( TempBytes < (BLOCK_TEXTURE * 2) )
		{
			memcpy( pointer, TempBuffer, TempBytes );
			pMapRegion->m_dwBlendTextureBlock = TempBytes;
		}
		else
		{
			TempBytes = BLOCK_TEXTURE * 2;

			memcpy( pointer, (DWORD*)( TexBuffer + nTexSkip ), TempBytes );
			pMapRegion->m_dwBlendTextureBlock = TempBytes;
		}

		Size += TempBytes;
		pointer += TempBytes;


		//SAFE_DELETE_ARRAY( TempBuffer );
	}


	// fill lightmap texture block
	if ( pFileData->m_dwLightmapTextureBlock )
	{
		int Length = 0;

		fseek( m_fileReader, pFileData->m_dwLightmapTextureBlock, SEEK_SET );
		fseek( m_fileReader, sizeof(DWORD), SEEK_CUR ); // Version 

		SAFE_READ( &Length, sizeof(int) );


		// read buffer
		BYTE* TexBuffer = m_tempWrite;
		//SAFE_NEW_ARRAY( TexBuffer, BYTE, Length );
		ASSERT( NULL != TexBuffer );
		ASSERT( TEMP_BUFFER_WRITE >= Length );

		SAFE_READ( TexBuffer, Length );	
		SAFE_READ( &BlockVaildate, sizeof(BlockVaildate) );
		ASSERT( BlockVaildate == TERRAIN_FMTBLOCKEND );

		ASSERT( TexBuffer[2] == 2 ); // image type, Uncompressed, RGB images
		ASSERT( ( TexBuffer[16] == 24 ) || ( TexBuffer[16] == 32 ) ); // pixel size

		// compact buffer
		BYTE* pTexCompact = (BYTE*)TexBuffer;
		DWORD const  nTexSkip   = Length - ( BLOCK_TEXTURE * 4 );
		DWORD const* pTexReader = (DWORD*)( TexBuffer + nTexSkip );
		DWORD const* pTexEnd    = pTexReader + BLOCK_TEXTURE;


		UINT TexSize = 0;
		for ( BYTE compact = 0; pTexReader != pTexEnd; compact = 0 )
		{
			DWORD lhsValue = *pTexReader ++;
			DWORD rhsValue = ( pTexReader != pTexEnd ) ? *pTexReader ++ : 0;

			compact <<= 4;	compact  |=  BLOCK_ORDER_16( ( ( lhsValue >> 0  ) & 0xFF ) );
			compact <<= 4;	compact  |=  BLOCK_ORDER_16( ( ( rhsValue >> 0  ) & 0xFF ) );

			* pTexCompact ++ = compact;
			++ TexSize;

		}



		// rle compressed
		BYTE  TempBuffer[ BLOCK_TEXTURE*6 ] = {0};
		DWORD TempBytes = 0;

		BYTE const* pTgaReader = (BYTE*)TexBuffer;
		BYTE const* pTgaEnd    = pTgaReader + TexSize;

		for ( ; pTgaReader != pTgaEnd; )
		{
			BYTE  rleValue = 0;
			BYTE  rleLength = 0;


			// compressed
			for (  rleValue = *pTgaReader; 
				( pTgaReader != pTgaEnd ) && ( rleValue == (*pTgaReader) ) && ( rleLength < 128 );
				++ rleLength, ++ pTgaReader )
			{
			}

			if ( 0 != rleLength )
			{
				TempBuffer[ TempBytes ++ ] = ( rleLength - 1 ) | 0x80;
				TempBuffer[ TempBytes ++ ] = ( rleValue & 0xFF );
				continue ;
			}


			// no compressed
			while ( ( pTgaReader != pTgaEnd ) && ( rleValue != (*pTgaReader) ) && ( rleLength < 128 ) )
			{
				++ rleLength;
				rleValue = *pTgaReader ++;
			}

			if ( 0 != rleLength )
			{
				TempBuffer[ TempBytes ++ ] = ( rleLength - 1 );
				memcpy( &TempBuffer[ TempBytes ], pTgaReader - rleLength - 1, rleLength );

				TempBytes  += rleLength ;
				pTgaReader --;
			}

		}



		memcpy( pointer, TempBuffer, TempBytes );
		pMapRegion->m_dwLightmapTextureBlock = TempBytes;

		Size += TempBytes;
		pointer += TempBytes;


	//	SAFE_DELETE_ARRAY( TempBuffer );	
	}


	// fill grass block
	if ( pFileData->m_dwGrassBlock )
	{
		int NumGrass = 0;

		fseek( m_fileReader, pFileData->m_dwGrassBlock, SEEK_SET );
		fseek( m_fileReader, sizeof(DWORD), SEEK_CUR ); // Version 


		SAFE_READ( &NumGrass, sizeof(int) );		// NumGrass
		pMapFileSect->nGrassNums = NumGrass;
		

		// read
		BYTE* TempBuffer = m_tempWrite;
		DWORD TempBytes  = NumGrass * sizeof(OLD_RegionGrass);
		ASSERT( NULL != TempBuffer );
		ASSERT( TEMP_BUFFER_WRITE >= TempBytes );


		SAFE_READ( TempBuffer, TempBytes );
		memcpy( pointer, TempBuffer, TempBytes );
		pMapRegion->m_dwGrassBlock = TempBytes;

		Size += TempBytes;
		pointer += TempBytes;


		SAFE_READ( &BlockVaildate, sizeof(BlockVaildate) );
		ASSERT( BlockVaildate == TERRAIN_FMTBLOCKEND );

	}


	if ( pFileData->m_dwWaterBlock )
	{
		fseek( m_fileReader, pFileData->m_dwWaterBlock, SEEK_SET );
		fseek( m_fileReader, sizeof(DWORD), SEEK_CUR ); // Version 


		// read
		BYTE* TempBuffer = m_tempWrite;
		DWORD TempBytes  = BLOCK_VERTEX * ( sizeof(float) + sizeof(D3DXCOLOR) );
		ASSERT( NULL != TempBuffer );
		ASSERT( TEMP_BUFFER_WRITE >= TempBytes );


		pMapFileSect->nWaterNums = BLOCK_VERTEX;
		SAFE_READ( TempBuffer, TempBytes );
		memcpy( pointer, TempBuffer, TempBytes );
		pMapRegion->m_dwWaterBlock = TempBytes;

		Size += TempBytes;
		pointer += TempBytes;


		SAFE_READ( &BlockVaildate, sizeof(BlockVaildate) );
		ASSERT( BlockVaildate == TERRAIN_FMTBLOCKEND );

	}



	return Size;
}

int   CMapSerialize::parseRegionSize( int i )
{
	ASSERT( m_mapOldSectVector.size() > i );
	KG3DTerrainRegionFileData* pFileData = &m_mapOldSectVector.at(i);
	int Size = 0;

	if ( pFileData->m_dwVertexBlock )
	{
		Size += 17 * 17 * sizeof(OLD_RegionVB);
	}


	if ( pFileData->m_dwBlendTextureBlock )
	{
		int Length = 0;

		fseek( m_fileReader, pFileData->m_dwBlendTextureBlock, SEEK_SET );
		fseek( m_fileReader, sizeof(DWORD), SEEK_CUR ); // Version 
	
		SAFE_READ( &Length, sizeof(int) );
		Size += Length;
	}


	if ( pFileData->m_dwLightmapTextureBlock )
	{
		int Length = 0;

		fseek( m_fileReader, pFileData->m_dwLightmapTextureBlock, SEEK_SET );
		fseek( m_fileReader, sizeof(DWORD), SEEK_CUR ); // Version 

		SAFE_READ( &Length, sizeof(int) );
		Size += Length;
	}


	if ( pFileData->m_dwGrassBlock )
	{
		int NumGrass = 0;

		fseek( m_fileReader, pFileData->m_dwGrassBlock, SEEK_SET );
		fseek( m_fileReader, sizeof(DWORD), SEEK_CUR ); // Version 


		SAFE_READ( &NumGrass, sizeof(int) );		// NumGrass
		Size += NumGrass * sizeof(OLD_RegionGrass);

	}


	if ( pFileData->m_dwWaterBlock )
	{
		Size += 17 * 17 * ( sizeof(float) + sizeof(D3DXCOLOR) );
	}



	return Size;
}


////////////////////////////////////////////////
bool  CMapSerialize::parseRequire()
{
	ASSERT( ( NULL != m_fileWrite ) && ( NULL != m_fileReader ) );
	if ( ( NULL != m_fileWrite )
		&& ( NULL != m_fileReader ) )
	{
		return true;
	}


	return false;
}

bool  CMapSerialize::parseEnsure()
{
	return true;
}
