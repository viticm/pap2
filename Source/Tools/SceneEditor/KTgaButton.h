#pragma once
//#include "StdAfx.h"
#include "afxext.h"
#include <list>
#include <string>
#include "resource.h"
#include "KGAssociateGrassDlg.h"

class IEKG3DTerrain;
//class KG3DSceneSceneEditor;

#define		GROUNDTEXTURE	0
#define		BRUSHMAP		1

class KSceneSceneEditorDialogGroundTextureUV;
class KStaticBitmapSectionSelector;
interface IEKG3DSceneSceneEditor;//by dengzhihui Refactor 2006年11月27日

class KTgaButton : public CButton
{
	KSceneSceneEditorDialogGroundTextureUV* m_pDialogTextureUV;

#pragma pack(push,1)
	struct TGAHeader 
	{
		unsigned char   Id ;
		unsigned char   ColourMapType ;
		unsigned char   ImageType ;
		unsigned short  ColourMapOrigin ;
		unsigned short  ColourMapLength ;
		unsigned char   ColourMapDepth ;
		unsigned short  x_origin ;
		unsigned short  y_origin ;
		unsigned short  width ;
		unsigned short  height ;
		unsigned char   bitsperpixel ;
		unsigned char   imagedescriptor ;
	};
#pragma pack(pop)

	struct TGAInfo
	{
		TGAInfo()
		{
			pBuffer = NULL;
		}
		
		~TGAInfo()
		{	
			SAFE_DELETE_ARRAY(pBuffer);

		}

		TGAHeader Header;
		BYTE*     pBuffer;
	};

private:

	BYTE	*SacleData(const RECT &rect,const byte *SrcData,const int &BitPerPixel);

public:
	KStaticBitmapSectionSelector* m_lpStaticSection;

	KTgaButton(void);
	~KTgaButton(void);

	HRESULT LoadTgaFile(const char* strFileName,BOOL bAddToTerrian);//是否将贴图添加地形中

	HRESULT ChangeTagFile(const char *strFileName);

	bool InitButton(const int &Layer,const int &ButtonType);
    void    SetTexCoordIndex(int &Index);

	TGAInfo m_TgaInfo;
	CBitmap m_Bitmap;

public:
	inline std::string GetTextureName()
	{
		return m_TextureDir;
	}
	inline void SetLayer(const int Layer)
	{
		m_nLayer = Layer;
	}
	inline int GetLayer()
	{
		return m_nLayer;
	}

	inline void SetTerrain(IEKG3DTerrain *pTerrainEx)
	{
		//assert(pTerrainEx);
		m_pTerrainEx = pTerrainEx;
		m_pOutDoorMgr = NULL; //add by huangjinshou 2008-6-4
	}
	inline void SetOutDoorSpaceMgr(IEKG3DSceneOutDoorSpaceMgr* pOutDoorMgr)//add by huangjinshou 2008-6-4
	{
		m_pOutDoorMgr = pOutDoorMgr;
		m_pTerrainEx = NULL;
	}
	inline void SetSceneEditor(IEKG3DSceneSceneEditor *lpSceneEditor)
	{
		assert(lpSceneEditor);
		m_lpSceneEditor = lpSceneEditor;
	}
	inline void SetButtonType(const int &Type)
	{
		m_nButtonType = Type;
	}
	inline int GetButtonType()
	{
		return m_nButtonType;
	}
	inline void GetRect(RECT &rect)
	{
		GetClientRect(&rect);
	}
	inline void SetIsSelected(const bool &IsS)
	{
		m_bIsSelected = IsS;
	}
	inline BOOL IsSelected()
	{
		return m_bIsSelected;
	}
public:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);

	

private:
	IEKG3DSceneOutDoorSpaceMgr* m_pOutDoorMgr;//add by huangjinshou 2008-6-4
	IEKG3DTerrain	*m_pTerrainEx;
	IEKG3DSceneSceneEditor *m_lpSceneEditor;
	bool				m_bIsSelected;
    byte                m_byTexCoordIndex;    // 0--XZ  1--XY  2--YZ 

	CMenu	m_MenuTga;
	int		m_nLayer;
	bool	m_bIsHaveTexture;
	int		m_nButtonType;			//0 --GroundTexture   1---BrushMap
	int		m_nTextureIndex;

	std::string		m_TextureDir;

    afx_msg void OnAssociateGrass();
	afx_msg void OnLoadtexture();
	afx_msg void OnDeletetexture();
	afx_msg void OnChangetexture();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
    afx_msg void OnSelectTexCoordXZ();
    afx_msg void OnSelectTexCoordXY();
    afx_msg void OnSelectTexCoordYZ();
	afx_msg void OnSettexcoord();
};
