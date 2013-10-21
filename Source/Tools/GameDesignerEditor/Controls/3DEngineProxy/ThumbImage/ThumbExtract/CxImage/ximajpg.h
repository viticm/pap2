/*
 * File:	ximajpg.h
 * Purpose:	JPG Image Class Loader and Writer
 */
/* === C R E D I T S  &  D I S C L A I M E R S ==============
 * CxImageJPG (c) 07/Aug/2001 <ing.davide.pizzolato@libero.it>
 * Permission is given by the author to freely redistribute and include
 * this code in any program as long as this credit is given where due.
 *
 * CxImage version 5.00 23/Aug/2002
 * See the file history.htm for the complete bugfix and news report.
 *
 * Special thanks to Troels Knakkergaard for new features, enhancements and bugfixes
 *
 * Special thanks to Chris Shearer Cooper for CxFileJpg tips & code
 *
 * original CImageJPG  and CImageIterator implementation are:
 * Copyright:	(c) 1995, Alejandro Aguilar Sierra <asierra@servidor.unam.mx>
 *
 * This software is based in part on the work of the Independent JPEG Group.
 * Copyright (C) 1991-1998, Thomas G. Lane.
 *
 * COVERED CODE IS PROVIDED UNDER THIS LICENSE ON AN "AS IS" BASIS, WITHOUT WARRANTY
 * OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, WITHOUT LIMITATION, WARRANTIES
 * THAT THE COVERED CODE IS FREE OF DEFECTS, MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE
 * OR NON-INFRINGING. THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE COVERED
 * CODE IS WITH YOU. SHOULD ANY COVERED CODE PROVE DEFECTIVE IN ANY RESPECT, YOU (NOT
 * THE INITIAL DEVELOPER OR ANY OTHER CONTRIBUTOR) ASSUME THE COST OF ANY NECESSARY
 * SERVICING, REPAIR OR CORRECTION. THIS DISCLAIMER OF WARRANTY CONSTITUTES AN ESSENTIAL
 * PART OF THIS LICENSE. NO USE OF ANY COVERED CODE IS AUTHORIZED HEREUNDER EXCEPT UNDER
 * THIS DISCLAIMER.
 *
 * Use at your own risk!
 * ==========================================================
 */
#if !defined(__ximaJPEG_h)
#define __ixmaJPEG_h

#include "ximage.h"

#if CXIMAGE_SUPPORT_JPG

#define CXIMAGEJPG_SUPPORT_EXIF 0

extern "C" {
 #include "../jpeg/jpeglib.h"
 #include "../jpeg/jerror.h"
};

class CxImageJPG: public CxImage
{
public:
	CxImageJPG(): CxImage(CXIMAGE_FORMAT_JPG) {}

//	bool Load(const char * imageFileName){ return CxImage::Load(imageFileName,CXIMAGE_FORMAT_JPG);}
//	bool Save(const char * imageFileName){ return CxImage::Save(imageFileName,CXIMAGE_FORMAT_JPG);}
	bool Decode(CxFile * hFile);
	bool Encode(CxFile * hFile);

	bool Decode(FILE *hFile) { CxIOFile file(hFile); return Decode(&file); }
	bool Encode(FILE *hFile) { CxIOFile file(hFile); return Encode(&file); }

////////////////////////////////////////////////////////////////////////////////////////
//////////////////////        E X I F             //////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
#if CXIMAGEJPG_SUPPORT_EXIF

#define MAX_COMMENT 1000

typedef struct tag_ExifInfo {
    char  CameraMake   [32];
    char  CameraModel  [40];
    char  DateTime     [20];
    int   Height, Width;
    int   IsColor;
    int   FlashUsed;
    float FocalLength;
    float ExposureTime;
    float ApertureFNumber;
    float Distance;
    float CCDWidth;
    float ExposureBias;
    int   Whitebalance;
    int   MeteringMode;
    int   ExposureProgram;
    int   ISOequivalent;
    int   CompressionLevel;
    char  Comments[MAX_COMMENT];

    unsigned char * ThumbnailPointer;  /* Pointer at the thumbnail */
    unsigned ThumbnailSize;     /* Size of thumbnail. */
} EXIFINFO;

class CxExifInfo
{

typedef struct tag_ExifTable{
    unsigned short Tag;
    char * Desc;
} EXIFTABLE;

public:
	EXIFINFO* m_exifinfo;
	char m_szLastError[256];
	CxExifInfo(EXIFINFO* info);
	bool DecodeInfo(struct jpeg_decompress_struct cinfo);
protected:
	bool exif_process(unsigned char * CharBuf, unsigned int length, EXIFINFO* pInfo);
	int Get16u(void * Short);
	int Get32s(void * Long);
	unsigned long Get32u(void * Long);
	bool ProcessExifDir(unsigned char * DirStart, unsigned char * OffsetBase, unsigned ExifLength,
                           EXIFINFO * const pInfo, unsigned char ** const LastExifRefdP);
	double ConvertAnyFormat(void * ValuePtr, int Format);
	bool is_exif(struct jpeg_marker_struct const marker);

	double FocalplaneXRes;
	double FocalplaneUnits;
	int ExifImageWidth;
	int MotorolaOrder;
};

	EXIFINFO m_exifinfo;

#endif //CXIMAGEJPG_SUPPORT_EXIF

////////////////////////////////////////////////////////////////////////////////////////
//////////////////////        C x F i l e J p g         ////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

// thanks to Chris Shearer Cooper <cscooper@frii.com>
class CxFileJpg : public jpeg_destination_mgr, public jpeg_source_mgr
	{
public:
	enum { eBufSize = 4096 };

	CxFileJpg(CxFile* pFile)
	{
        m_pFile = pFile;

		init_destination = InitDestination;
		empty_output_buffer = EmptyOutputBuffer;
		term_destination = TermDestination;

		init_source = InitSource;
		fill_input_buffer = FillInputBuffer;
		skip_input_data = SkipInputData;
		resync_to_restart = jpeg_resync_to_restart; // use default method
		term_source = TermSource;
		next_input_byte = NULL; //* => next byte to read from buffer 
		bytes_in_buffer = 0;	//* # of bytes remaining in buffer 

		m_pBuffer = new unsigned char[eBufSize];
	}
	~CxFileJpg()
	{
		delete [] m_pBuffer;
	}

	static void InitDestination(j_compress_ptr cinfo)
	{
		CxFileJpg* pDest = (CxFileJpg*)cinfo->dest;
		pDest->next_output_byte = pDest->m_pBuffer;
		pDest->free_in_buffer = eBufSize;
	}

	static boolean EmptyOutputBuffer(j_compress_ptr cinfo)
	{
		CxFileJpg* pDest = (CxFileJpg*)cinfo->dest;
		if (pDest->m_pFile->Write(pDest->m_pBuffer,1,eBufSize)!=(size_t)eBufSize)
			ERREXIT(cinfo, JERR_FILE_WRITE);
		pDest->next_output_byte = pDest->m_pBuffer;
		pDest->free_in_buffer = eBufSize;
		return TRUE;
	}

	static void TermDestination(j_compress_ptr cinfo)
	{
		CxFileJpg* pDest = (CxFileJpg*)cinfo->dest;
		size_t datacount = eBufSize - pDest->free_in_buffer;
		/* Write any data remaining in the buffer */
		if (datacount > 0) {
			if (!pDest->m_pFile->Write(pDest->m_pBuffer,1,datacount))
				ERREXIT(cinfo, JERR_FILE_WRITE);
		}
		pDest->m_pFile->Flush();
		/* Make sure we wrote the output file OK */
		if (pDest->m_pFile->Error()) ERREXIT(cinfo, JERR_FILE_WRITE);
		return;
	}

	static void InitSource(j_decompress_ptr cinfo)
	{
		CxFileJpg* pSource = (CxFileJpg*)cinfo->src;
		pSource->m_bStartOfFile = TRUE;
	}

	static boolean FillInputBuffer(j_decompress_ptr cinfo)
	{
		size_t nbytes;
		CxFileJpg* pSource = (CxFileJpg*)cinfo->src;
		nbytes = pSource->m_pFile->Read(pSource->m_pBuffer,1,eBufSize);
		if (nbytes <= 0){
			if (pSource->m_bStartOfFile)	//* Treat empty input file as fatal error 
				ERREXIT(cinfo, JERR_INPUT_EMPTY);
			WARNMS(cinfo, JWRN_JPEG_EOF);
			// Insert a fake EOI marker 
			pSource->m_pBuffer[0] = (JOCTET) 0xFF;
			pSource->m_pBuffer[1] = (JOCTET) JPEG_EOI;
			nbytes = 2;
		}
		pSource->next_input_byte = pSource->m_pBuffer;
		pSource->bytes_in_buffer = nbytes;
		pSource->m_bStartOfFile = FALSE;
		return TRUE;
	}

	static void SkipInputData(j_decompress_ptr cinfo, long num_bytes)
	{
		CxFileJpg* pSource = (CxFileJpg*)cinfo->src;
		if (num_bytes > 0){
			while (num_bytes > (long)pSource->bytes_in_buffer){
				num_bytes -= (long)pSource->bytes_in_buffer;
				FillInputBuffer(cinfo);
				// note we assume that fill_input_buffer will never return FALSE,
				// so suspension need not be handled.
			}
			pSource->next_input_byte += (size_t) num_bytes;
			pSource->bytes_in_buffer -= (size_t) num_bytes;
		}
	}

	static void TermSource(j_decompress_ptr cinfo)
	{
		return;
	}
protected:
    CxFile  *m_pFile;
	unsigned char *m_pBuffer;
	bool m_bStartOfFile;
};

};

#endif

#endif
