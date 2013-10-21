//////////////////////////////////////////////////////////////////////////
// File : KG3DSFXSelectTexture.h
// Creator : FengBo
// Date : 2007-10-10
// Desc : 为地表和物件增加一层贴图纹理，例如：可用于增加NPC选择框，
//			 或者爆炸产生的大范围贴图。
//////////////////////////////////////////////////////////////////////////

#ifndef __KG3DSFX_SELECT_TEXTURE_H__
#define __KG3DSFX_SELECT_TEXTURE_H__

#include "KG3DTexture.h"
#include "IEKG3DSFX.h"
enum KGFOOTPRINT
{
	DEFAULT_FOOT,
	LEFT_FOOT,
	RIGHT_FOOT
};
class KG3DSFX;
class KG3DSFXSelectTexture : public IEKG3DSFXSelectTexture 
{
public:
	KG3DSFXSelectTexture();
	KG3DSFXSelectTexture(KG3DSFX* pSFX);
	~KG3DSFXSelectTexture();
public:
	/**
	* @func : Update
	* @param : pVecPosCenter [in] 要增加的贴图中心点坐标
	* @return : HRESULT---S_OK or E_FAIL，是否更新成功
	* @brief : 此函数以贴图中心坐标为参数，计算贴图覆盖范围，使
	*			  覆盖范围内的物件、地表等均增加一层贴图
	*/
    void Update();

	HRESULT ShutDown();

	/**
	* @func : GetTexture
	* @param : none
	* @return : 当前使用的贴图纹理
	*/
	IEKG3DTexture* GetTexture();

	/**
	* @func : LoadTexture
	* @param : szTextureName [in] 读取纹理文件的文件名称
	* @return : HRESULT S_OK or E_FAIL，是否读取成功
	* @brief: 此函数从外部文件加载一张纹理，作为此次贴图的纹理
	*/
	HRESULT LoadTexture(const char* szTextureFileName);

	/**
	* @func : SetTexture
	* @param : pTexture [in] 
	* @return : HRESULT S_OK
	* @brief : 设置一张已存在的KG3DTexture类型的纹理为本纹理
	*/
	HRESULT SetTexture(IEKG3DTexture* pTexture);

	/**
	* @func :SetTexturePos
	* @param : pNewPos [in] 新的坐标位置
	* @return : HRESULT S_OK or E_FAIL
	* @brief : 手工指定当前贴图的位置坐标
	*/
	HRESULT SetTexturePos(D3DXVECTOR3* pNewPos);

	/**
	* @func :getTexturePos
	* @param : pPos [out] 当前纹理中心所在的坐标位置
	* @return : HRESULT S_OK or E_FAIL
	* @brief : none
	*/
	HRESULT GetTexturePos(D3DXVECTOR3* pPos);

	/**
	* @func :GetTextureWidth
	* @param : none
	* @return : UINT 
	* @brief : 返回当前使用贴图的宽度
	*/
	DWORD GetTextureWidth();
	void  SetTextureWidth(DWORD dwWidth);
	/**
	* @func :GetTextureHeight
	* @param : none
	* @return : UINT 
	* @brief : 返回当前使用贴图的高度
	*/
	DWORD GetTextureHeight();
	void  SetTextureHeight(DWORD dwHeight);

	HRESULT GetRect(D3DXVECTOR2& vLeftBottom,D3DXVECTOR2& vRightTop);
public:
	float GetAlpha();
	void  SetAlpha(float fAlpha);

	HRESULT SetSFX(KG3DSFX* pSFX);
private:
	/**
	* @func :GetPickPos
	* @param : pVecPos [Out] 当前鼠标和3D世界的交点(第一个)
	* @return : HRESULT S_OK or E_FAIL 
	* @brief : 交点这里只取第一个
	*/
	//HRESULT GetPickPos(OUT D3DXVECTOR3* pVecPos);

private:
	KG3DTexture* m_pTexture;		// 当前使用的纹理贴图
	KG3DSFX*     m_pSFX;            // 指向所在的特效

private:
	float         m_fAlpha;
	DWORD         m_nTextureWidth;
	DWORD		  m_nTextureHeight;
	D3DXVECTOR3   m_vec3PosCenter;
public:
	KGFOOTPRINT   m_enFoot;
	D3DXMATRIX    m_Matrix;
public:
	//vector<POINT> m_vecIncludeRegionIndex; //记录当前投影贴图所在Region的索引.
};

//////////////////////////////////////////////////////////////////////////
// Inline Functions
//////////////////////////////////////////////////////////////////////////

class KG3DSFXSelectMark : public IEKG3DSFXSelectMark,public KG3DTransformation
{
public:
	//KGFOOTPRINT   m_enFoot;                 //区分是左脚，右脚，还是不分左右脚
	DWORD         m_dwTerraType;            //所在地表类型。暂时地表还没有类型。默认为0
private:
	DWORD         m_nTextureWidth;
	DWORD		  m_nTextureHeight;
	DWORD         m_dwLastTime;
	KG3DTexture*  m_pTexture;		// 当前使用的纹理贴图

	LPDIRECT3DVERTEXBUFFER9 m_lpVB;
	LPDIRECT3DINDEXBUFFER9  m_lpIB;
	//D3DXVECTOR3   m_vDirection;
	D3DXMATRIX m_matSaveSFXWorld;
public:
	KG3DSFXSelectMark(KG3DSFX* pSFX);
	~KG3DSFXSelectMark();

private:
	HRESULT CreatVertexBuffer();
	void FillVertexBuffer(VFormat::_Faces_Diffuse_Texture1* pData);
public:
	HRESULT RenderTexture();
	
	HRESULT LoadTexture(const char* szTextureFileName);
	IEKG3DTexture* GetTexture();
//	HRESULT AddProTexture(KG3DSFXSelectTexture** pSelTexture);//暂时没使用
public:
	virtual HRESULT OnLostDevice();
	virtual HRESULT OnResetDevice();
	HRESULT GetCurPosWidthHeight(D3DXVECTOR3* pPos,DWORD* pWidth,DWORD* pHeight);
	D3DXVECTOR3 GetCurPos();
	DWORD GetTextureWidth();
	void  SetTextureWidth(DWORD dwWidth);
	DWORD GetTextureHeight();
	void  SetTextureHeight(DWORD dwHeight);
	DWORD GetWidth(); // 获取乘以所在SFX特效的缩放参数后宽度
	DWORD GetHeight();//获取乘以所在SFX特效的缩放参数后高度
	//DWORD GetenFoot();                   //暂时没使用
	//void  SetenFoot(DWORD& dwVal );//暂时没使用
	//std::string GetFootprintType();//暂时没使用
	//void  SetFootprintType(std::string& scType);//暂时没使用
	KG3DSFX*      m_pSFX;            // 指向所在的特效

};
#endif //:-)