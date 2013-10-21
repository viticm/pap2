#pragma once
#define CAPTION_INVALIDATE_INDEX 0xffffffff

class KG3DTexture;
class KG3DCaptionTextureManager;

class KG3DCaption2D
{
public:
	KG3DCaption2D();
	~KG3DCaption2D(){};
	//设置全局的可见性
	void SetVisible(DWORD dwID,
		DWORD dwIndex, 
		BOOL bVisible);

	HRESULT Init();
	void UnInit();

    HRESULT OnLostDevice();
    HRESULT OnResetDevice();

	DWORD Add(DWORD dwID,
		DWORD dwIndex, 
		LPCSTR strContent);
	void Remove(unsigned int uID);
	void SetRenderData(DWORD dwID, 
		const D3DXVECTOR3 *pPos, 
		const D3DCOLORVALUE *pColor, 
		const float *pPercentage,
		const D3DCOLORVALUE *pPercentageColor);
	HRESULT Render();
	HRESULT Render2D();
	const static DWORD MAX_NUM_CAPTION = 4;
private:
	struct LineContent
	{
		DWORD dwMgrIndex;
		DWORD dwBlockIndex;
		D3DCOLORVALUE color;
		LineContent()
		{
			UnInit();
		}
		void UnInit()
		{
			dwBlockIndex = CAPTION_INVALIDATE_INDEX;
			dwMgrIndex = 0;
		}
	};

	struct RenderData
	{
		BOOL bUsed;
		D3DXVECTOR3 vec3Pos;
		float fPercentage;
		D3DCOLORVALUE PercentageColor;
		BOOL m_bVisible[MAX_NUM_CAPTION + 1];//可见性， MAX_NUM_CAPTION的这个是血条的
		LineContent Content[MAX_NUM_CAPTION];
		RenderData();
		void UnInit();
	};
	std::vector<RenderData> m_RenderData;
	std::set<DWORD> m_Free;
	KG3DCaptionTextureManager *m_pTextureMgr;

	struct Vertex 
	{
		D3DXVECTOR3 vec3Pos;
		D3DXVECTOR2 vec2Tex0;
		const static DWORD FVF = D3DFVF_XYZ | D3DFVF_TEX1;
	};
	Vertex m_Vertex[6];
	Vertex m_Percentage[6];
	float m_fCaptionHeight;
	//下面是来自配置文件的血条参数
	KG3DTexture *m_pFrontTexture;//血条的前景贴图
	KG3DTexture *m_pBackTexture;//血条的背景贴图
	float m_fPercentageWidth;//血条的宽度
	float m_fPercentageHeight;//血条的高度
	float m_fBorderX;//血条边的X宽度
	float m_fBorderY;//血条边的Y宽度
	DWORD m_dwBackBorderColor;//血条边框的颜色
	float m_fBackBorderColorScale;//血条的颜色衰减根据前景色
};

class KG3DCaptionTextureManager
{
	friend void KG3DCaption2D::UnInit();
public:
	enum ItemState
	{
		IS_PENDING,//空间完成 需要绘制
		IS_READY,//完成 可以使用
	};

	struct ItemInfo
	{
		DWORD dwWidth;
		DWORD dwHeight;
		ItemState State;
		std::string strContent;
		int nRef;
		ItemInfo()
		{
			Reset();
		}
		void Reset()
		{
			dwWidth = 0;
			dwHeight = 0;
			State = IS_PENDING;
			strContent = "";
			nRef = 0;
		}
	};

	KG3DCaptionTextureManager();
	LPDIRECT3DTEXTURE9 m_pTexture;
	const static float s_fOffset;

	HRESULT Init(LPCSTR strFontFile, 
		int nSize, 
		BOOL bBold, 
		BOOL bItalic, 
		DWORD dwMgrIndex);
	void UnInit();

    HRESULT OnLostDevice();
    HRESULT OnResetDevice();

	DWORD Add(LPCSTR strContent, 
		DWORD &dwMgrIndex);
	void Remove(DWORD dwMgrIndex, 
		DWORD dwBlockIndex);

	HRESULT GetUVCoordinate(DWORD dwMgrIndex,
		DWORD dwBlockIndex, 
		D3DXVECTOR2 &vA, 
		D3DXVECTOR2 &vC);

	float GetAspect();

	HRESULT OnRender(bool bNeedRestore);
	KG3DCaptionTextureManager *m_pNextMgr;

	HRESULT SetTexture(DWORD dwStage,
		DWORD dwIndex);

private:
	HRESULT CreateFont();
	typedef struct _tagTT_OFFSET_TABLE{
		USHORT	uMajorVersion;
		USHORT	uMinorVersion;
		USHORT	uNumOfTables;
		USHORT	uSearchRange;
		USHORT	uEntrySelector;
		USHORT	uRangeShift;
	}TT_OFFSET_TABLE;

	typedef struct _tagTT_TABLE_DIRECTORY{
		char	szTag[4];			//table name
		ULONG	uCheckSum;			//Check sum
		ULONG	uOffset;			//Offset from beginning of file
		ULONG	uLength;			//length of the table in bytes
	}TT_TABLE_DIRECTORY;

	typedef struct _tagTT_NAME_TABLE_HEADER{
		USHORT	uFSelector;			//format selector. Always 0
		USHORT	uNRCount;			//Name Records count
		USHORT	uStorageOffset;		//Offset for strings storage, from start of the table
	}TT_NAME_TABLE_HEADER;

	typedef struct _tagTT_NAME_RECORD{
		USHORT	uPlatformID;
		USHORT	uEncodingID;
		USHORT	uLanguageID;
		USHORT	uNameID;
		USHORT	uStringLength;
		USHORT	uStringOffset;	//from start of storage area
	}TT_NAME_RECORD;
	
	HRESULT GetFontNameFromFile(LPCSTR lpszFilePath, 
		LPSTR strFaceName,
		size_t uSize);

	DWORD FindContentNext(LPCSTR strContent, DWORD& dwIndex);
	static std::string m_strFontName;
	static std::string m_strFontFaceName;
	static BOOL m_bBold;
	static BOOL m_bItalic;
	static int m_nFontSize;
	static DWORD m_dwBlockWidth;
	static DWORD m_dwBlockHeight;
	static DWORD m_dwNumCloumn;
	static DWORD m_dwNumRow;
	static LPD3DXFONT m_pFont;
	std::map<std::string, DWORD> m_StringToData;
	std::set<DWORD> m_FreeBlock;
	std::deque<DWORD> m_Released;
	std::set<DWORD> m_DataPending;
	std::vector<ItemInfo> m_Data;
	BOOL m_bNeedClear;
	DWORD m_dwMgrIndex;//Mgr的Index
	static HANDLE m_hFontHandle;
};


extern KG3DCaption2D g_cCaption2D;