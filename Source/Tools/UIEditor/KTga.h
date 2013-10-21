#ifndef _KTGA_H_
#define _KTGA_H_

#define KG_PROCESS_ERROR(Condition) \
    do  \
    {   \
        if (!(Condition))   \
            goto Exit0;     \
    } while (false)


struct TGAFILEHEADER
{
    BYTE		IDLength;					// ID length
	BYTE		ColorMapType;				// Color map type
	BYTE		ImageType;					// Image type
	BYTE		ColorMapSpec[5];			// Color map specification
	WORD		X,Y;							// Image specification
	WORD		Width,Height;
	BYTE		PixelDep;
	BYTE		Desc;
};


class KTga32
{
private:
    TGAFILEHEADER m_TgaFileHeader;
    char          m_szTgaFireName[MAX_PATH];
    void          *m_pDataBuffer;
public:
    int         CreateFromFile(char *pTgaFileName, BOOL bCreateDataBuffer = FALSE);
    int         Create(int nWidth, int nHeight, int nDepth);
    int         Release();
    int         ReadARowToMemery(void * pTarget, int nRow); //nRow strat from zero.
    int         ReadAllDataToMemery(void * pTarget, int x, int y, int nWidth); //x,y start from zero.
    int         ReadAllDataToSelfMemery();
    int         SaveDataBufferToTgaFile(char *pTgaFileName);
    int         SaveDataBufferToBmpFile(char *pBmpFileName);
	int			Premultiplied();
    
    char        *GetTgaFileName(){ return m_szTgaFireName;     };
    void        *GetDataBufferPointer(){ return m_pDataBuffer; };
	long		GetWidth()	{ return m_TgaFileHeader.Width;    };
	long		GetHeight() { return m_TgaFileHeader.Height;   };
    long        GetDepth()  { return m_TgaFileHeader.PixelDep; };
    KTga32();
    ~KTga32();
};

#endif


