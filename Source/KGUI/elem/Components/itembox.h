////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : itembox.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-2-24 10:51:23
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_ITEMBOX_H_
#define _INCLUDE_ITEMBOX_H_

////////////////////////////////////////////////////////////////////////////////
#include "./item.h"
#include "KG3DEngine/KG3DInterface.h"

////////////////////////////////////////////////////////////////////////////////

namespace UI
{

#define ITEM_BOX_HAVE_OBJECT		    0x00000001
#define ITEM_BOX_OBJECT_DISABLE			0x00000002
#define ITEM_BOX_OBJECT_UNEQUIPABLE		0x00000004
#define ITEM_BOX_OBJECT_COOLINGDOWN		0x00000008
#define ITEM_BOX_OBJECT_SPARKING   		0x00000010
#define ITEM_BOX_OBJECT_SELECTED		0x00000020
#define ITEM_BOX_OBJECT_MOUSEOVER		0x00000040
#define ITEM_BOX_OBJECT_PRESSED			0x00000080
#define ITEM_BOX_OBJECT_INUSE			0x00000100
#define ITEM_BOX_OBJECT_STARING			0x00000200

#define ITEM_BOX_OVER_OBJECT_TEXT_COUNT 2

#define MAX_BOX_OBJECT_DATA_COUNT 6

#define INVALID_OBJECT_TYPE		-1
#define INVALID_OBJECT_INDEX	-1

struct  KItemBoxData : KItemNullData
{
	int nIndex;

	KItemBoxData(KWndWindow *pEOwner) : KItemNullData(pEOwner)
	{
		nIndex = -1;
	}

};


struct ITEM_BOX_OBJECT_DATA
{
	double fData[MAX_BOX_OBJECT_DATA_COUNT];

	ITEM_BOX_OBJECT_DATA()
	{
		Reset();
	}

	void Reset()
	{
		for (int i = 0; i < MAX_BOX_OBJECT_DATA_COUNT; ++i)
		{
			fData[i] = -1;
		}
	}
};

class KItemBox : public KItemNull
{
private:
	struct KITEMBOXOVERTEXT 
	{
		int nPosition;
		int nFontScheme;
		KG3DUIText2 OverText;

		KITEMBOXOVERTEXT()
		{
			nPosition = POSITION_RIGHT_BOTTOM;
			nFontScheme = 0;
			OverText.wszText[0] = L'\0';
		};
	};
protected:
	KItemBox();
	virtual ~KItemBox();

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
	static KItemBox *Create(KItemBoxData &ItemBoxData);
	virtual void    Release();
	virtual int     Init(KItemBoxData &ItemBoxData);

	virtual void    Draw();
	virtual DWORD   GetItemType();
	virtual int     IsItemCanChangeTo(DWORD dwType);

	virtual int SetRelPos(float fRelX, float fRelY);
	virtual int SetAbsPos(float fAbsX, float fAbsY);
	virtual int AbsMove(float fDeltaX, float fDeltaY);

	virtual int     Scale(float fScaleX, float fScaleY);

	int SetIndex(int nIndex);
	int GetIndex();

	int SetObject(int nObjectType, const ITEM_BOX_OBJECT_DATA &cObjectData);
	int GetObject(int &nObjectType, ITEM_BOX_OBJECT_DATA &ObjectData);
	int GetObjectType();
	int GetObjectData(ITEM_BOX_OBJECT_DATA &ObjectData);
	int ClearObject();
	int IsEmpty();

	int EnableObject(int nEnable);
	int IsObjectEnable();

	int EnableObjectEquip(int nEnable);
	int IsObjectEquipable();
	
	int SetObjectCoolDown(int nCoolDown);
	int IsObjectCoolDown();

	int SetObjectSparking(int nSparking);
	int SetObjectInUse(int nInUse);
	int SetObjectStaring(int nStaring);

	int SetObjectSelected(int nSelect);
	int IsObjectSelected();

	int SetObjectMouseOver(int nMouseOver);
	int IsObjectMouseOver();

	int SetObjectPressed(int nPressed);
	int IsObjectPressed();

	int SetCoolDownPercentage(float fPercentage);
	float GetCoolDownPercentage();

	int SetObjectIcon(int nIconID);
	int GetObjectIcon();
	int ClearObjectIcon();

	int SetOverText(int nOverTextIndex, LPCWSTR wszText);
	LPCWSTR GetOverText(int nOverTextIndex) const;
	int SetOverTextFontScheme(int nOverTextIndex, int nFontScheme);
	int GetOverTextFontScheme(int nOverTextIndex);
	int SetOverTextPosition(int nOverTextIndex, int nPosType);
	int GetOverTextPosition(int nOverTextIndex);

private:
	int FormatIconDrawData();
	int FormatOverText(int nOverTextIndex);

public:
	static  DWORD ms_dwItemType;
private:

	DWORD m_dwStatus;
	int m_nIndex;	//box的序列号

	int m_nObjectType; 
	ITEM_BOX_OBJECT_DATA m_ObjectData;

	float m_fCoolingPercentage;

	KG3DUIImage2 m_Image;	//绘制数据

	int m_nIconID;
	int m_nSparkingAnimateIndex;
	int m_nInUseAnimateIndex;
	int m_nStaringAnimateIndex;
	KITEMBOXOVERTEXT m_aOverText[ITEM_BOX_OVER_OBJECT_TEXT_COUNT];
};

} //namespace UI


#endif //_INCLUDE_ITEMBOX_H_
