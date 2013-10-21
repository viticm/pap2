////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : itemimage.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2005-11-9 17:05:59
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_ITEMIMAGE_H_
#define _INCLUDE_ITEMIMAGE_H_


#include "KG3DEngine/KG3DInterface.h"
#include "./item.h"

//ItemNull占据低4位。
#define ITEM_IMAGE_NOT_FROM_UITEX	0x00010000	//不来自于uitex，比如jpg，场景等。
#define ITEM_IMAGE_RESOURCE_LOADED	0x00020000	//资源已经被加载了。

namespace UI
{

	// TODO: 使用三组可相互组合的枚举更好用些
enum 
{
	ITEMIMAGE_NORMAL = 0,

	ITEMIMAGE_PART_LEFT_TO_RIGHT,
	ITEMIMAGE_PART_RIGHT_TO_LEFT,
	ITEMIMAGE_PART_TOP_TO_BOTTOM,
	ITEMIMAGE_PART_BOTTOM_TO_TOP,
	ITEMIMAGE_PART_TIMER,

	ITEMIMAGE_ROTATE,

	ITEMIMAGE_FLIP_VERTICAL,
	ITEMIMAGE_FLIP_HORIZONTAL,
	ITEMIMAGE_FLIP_CENTRAL,

	ITEMIMAGE_NINE_PART,
	ITEMIMAGE_LEFT_CENTER_RIGHT,
	ITEMIMAGE_TOP_CENTER_BOTTOM,

	ITEMIMAGE_END,
};

struct KItemImageData : KItemNullData
{
    char szImagePath[MAX_PATH];
    int nFrame;
	int nPartImageType;

    KItemImageData(KWndWindow *pEOwner) : KItemNullData(pEOwner)
    {
        szImagePath[0] = '\0';

        nFrame = 0;
		nPartImageType = ITEMIMAGE_NORMAL;
    };
};


class KItemImage : public KItemNull
{
protected:
    KItemImage();
    virtual ~KItemImage();

public:
	void* operator new(size_t nSize, std::nothrow_t const&) throw();
	void operator delete(void* pData, std::nothrow_t const&) throw();

	void* operator new(size_t nSize);
	void operator delete(void* pData) throw();

protected:
	void* operator new[](std::size_t, std::nothrow_t const&) throw();
	void operator delete[](void*, std::nothrow_t const&) throw();

	void* operator new[](std::size_t) throw(std::bad_alloc);
	void operator delete[](void*) throw();

	void* operator new(size_t, void*) throw();
	void operator delete(void*, void*) throw();

	void* operator new[](std::size_t, void*) throw();
	void operator delete[](void*, void*) throw();

#ifdef KGUI_LUA_LOADER
public:
    static int  Create(IKUIObject **ppObj, Lua_State *pLua, int nTableIndex);
    virtual int Init(Lua_State *pLua, int nTableIndex);
#endif

public:
    static KItemImage *Create(KItemImageData &ItemImageData);
    virtual void Release();
    virtual int Init(KItemImageData &ItemImageData);

    virtual void Draw();

    virtual DWORD GetItemType();
    virtual int IsItemCanChangeTo(DWORD dwType);

    int AutoSize();
    int SetFrame(int nImageFrame);
	int GetFrame();
	int FromIconID(int nIconID);
	int FromUITex(LPCSTR szFile, int nImageFrame);
	int FromTextureFile(LPCSTR pcszFile, D3DXVECTOR2 &vMin, D3DXVECTOR2 &vMax);
	int FromScene(IKG3DScene* p3DScene);
	int FromImageID(DWORD dwImageID);
    int ToManagedImage();

	DWORD GetImageID() const;

	int SetImageType(int nType);
	int GetImageType() const;

	int SetPercentage(float fPercentage);
	float GetPercentage() const;

	int SetRotate(float fRotate);
	float GetRotate() const;

	int FormatDrawData();
	int UnloadSource();

	void DrawNinePart();
	void DrawLeftCenterRight();
	void DrawTopCenterBottom();
public:
    static  DWORD ms_dwItemType;

private:
	KG3DUIImagePart2 m_Image;
	int m_nImageInfoIndex;
	int m_nFrame;
	int m_nImageType;
	union
	{
		float m_fPercentage;
		float m_fRotate;
	};
	
};

} //namespace UI


#endif //_INCLUDE_ITEMIMAGE_H_
