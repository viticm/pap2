// SFXBase.h: interface for the KPropertyBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SFXBASE_H__1C7FA33A_511F_4282_A96E_D0A570122EEB__INCLUDED_)
#define AFX_SFXBASE_H__1C7FA33A_511F_4282_A96E_D0A570122EEB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <VECTOR>


//#include "XMesh.h"
class KModel;
class KSceneModelEditor;
class KSceneSceneEdit;
class KModelTerrain;
namespace ModelSFX
{
	class KTimePointColor;
	class KTimePoint;
};

namespace PropertyBase
{

typedef struct  _VarProperty
{ 
	enum _VarType
	{
		VAR_NONE = 0,
		VAR_INT  = 1,//整数

		VAR_FLOAT  = 3,//浮点数
		VAR_STRING = 4,//字符串,TCHAR
		VAR_ENUM   = 5,//枚举类型
		VAR_BOOL   = 6,//布儿
		VAR_STLSTRING = 7,//STL string
		VAR_COLOR   = 8,//D3DCOLOR
		VAR_COLORVALUE = 9,//D3DCOLORVALUE

		VAR_VECTOR2 = 20,
		VAR_VACTOR3 = 21,
		VAR_VACTOR4 = 22,
		VAR_QUATERNION = 23,
		VAR_POSITION   = 24,
		VAR_DIRECTION  = 25,

		VAR_MTLLIST = 100,
		VAR_ANILIST = 101,
		VAR_MTLTEX  = 102,
		VAR_MTLOPTOIN = 103,
		VAR_MTLSTATE = 104,
		VAR_MTLEFFECT = 105,

		VAR_MODELEDITORSFXNEW = 200,//模型编辑器新建特效的界面
		VAR_SCENEEDITOROBJECT = 201,//场景编辑器物件放置的界面
		VAR_TERRAIN_GROUND_TEXTURE = 202,
		VAR_PARTICLE = 300,
		VAR_TIMEPOINT = 400,
		VAR_TIMEPOINT_COLOR = 401,
		FORECDWORD = 0xFFFFFFFF,
	};

	string    scName;
	_VarType  Type;
	PVOID     Address;
	PVOID     DateStore;

	struct _StoreInt
	{
		int Value;
		int Max;
		int Min;
		_StoreInt()
		{
			Value = 0;
			Max = 100;
			Min = -100;
		}
		~_StoreInt()
		{
			Value = 0;
			Max = 100;
			Min = -100;
		}
	};

	struct _StoreFloat
	{
		float Value;
		float Max;
		float Min;
		float Step;

		_StoreFloat()
		{
			Value = 0;
			Max = 100;
			Min = -100;
			Step = 0.01f;
		}
		~_StoreFloat()
		{
			Value = 0;
			Max = 100;
			Min = -100;
			Step = 0.01f;
		}
	};

	/*struct _StoreString
	{
		LPSTR  pStr;
		string StringValue;
		_StoreString()
		{
			pStr = NULL;
		}
		~_StoreString()
		{
			pStr = NULL;
			StringValue.clear();
		}
	};*/

	struct _StoreBool
	{
		BOOL   bValue;
		_StoreBool()
		{
			bValue = FALSE;
		}
	};

	struct _EnumItem
	{
		int    Value;
		string Text;
		_EnumItem(int nValue,LPSTR pStr)
		{
			Value = nValue;
			Text = pStr;
		}
	};	

	struct _StoreEnum
	{
		int    nValue;


		vector<_EnumItem>vecEnumItem;
		_StoreEnum()
		{
			nValue = 0;
		}
		~_StoreEnum()
		{
			nValue = 0;
			vecEnumItem.clear();
		}
	};
	////////////////////////////////////////////////////////////////////////////////
	
	//Enum
	_VarProperty(LPSTR Name,int* nIndex,int NumEnum,int* EnumIndex,LPSTR* EnumName)
	{
		scName = Name;
		Type = VAR_ENUM;
		Address = nIndex;
		if((nIndex)&&(NumEnum))
		{
			_StoreEnum* pStoreEnum = new _StoreEnum;
			pStoreEnum->nValue = *nIndex;
			
			for(int i=0;i<NumEnum;i++)
			{
				_EnumItem EnumItem(EnumIndex[i],EnumName[i]);
				pStoreEnum->vecEnumItem.push_back(EnumItem);
			}
			DateStore = pStoreEnum;
		}
	}
	
	//D3DXVECTOR3
	_VarProperty(LPSTR Name,D3DXVECTOR3* pVec)
	{
		scName = Name;
		Type = VAR_POSITION;
		Address = pVec;
		DateStore = NULL;
	}

	//BOOL
	_VarProperty(LPSTR Name,BOOL *pBool)
	{
		scName = Name;
		Type = VAR_BOOL;
		Address = pBool;
		if(pBool)
		{
			_StoreBool* pStoreBool = new _StoreBool;
			pStoreBool->bValue = *pBool;
			DateStore = pStoreBool;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	//TimePoint
	_VarProperty(LPSTR Name, ModelSFX::KTimePoint* pTimePoint)
	{
		scName = Name;
		Type = VAR_TIMEPOINT;
		Address = pTimePoint;
		DateStore = pTimePoint;
	}
	//TimePoint Color
	_VarProperty(LPSTR Name, ModelSFX::KTimePointColor* pTimePoint)
	{
		scName = Name;
		Type = VAR_TIMEPOINT_COLOR;
		Address = pTimePoint;
		DateStore = pTimePoint;
	}
	//////////////////////////////////////////////////////////////////////////
	
	//float 
	_VarProperty(LPSTR Name,float* pFloat,float Min,float Max,float Step)
	{
		scName = Name;
		Type = VAR_FLOAT;
		Address = pFloat;
		if(pFloat)
		{
			_StoreFloat* pStoreFloat = new _StoreFloat;
			pStoreFloat->Max = Max;
			pStoreFloat->Min = Min;
			pStoreFloat->Value = *pFloat;
			pStoreFloat->Step = Step;
			DateStore = pStoreFloat;
		}
	}

	//int
	_VarProperty(LPSTR Name,int* pInt,int Min,int Max)
	{
		scName = Name;
		Type = VAR_INT;
		Address = pInt;
		if(pInt)
		{
			_StoreInt* pStoreInt = new _StoreInt;
			pStoreInt->Max = Max;
			pStoreInt->Min = Min;
			pStoreInt->Value = *pInt;
			DateStore = pStoreInt;
		}
	}

	//string(LPSTR)
	_VarProperty(LPSTR Name,LPSTR pStr)
	{
		scName = Name;
		Type = VAR_STRING;
		Address = pStr;
		DateStore = NULL;
	}	
	//string(STL string)
	_VarProperty(LPSTR Name,string* Str)
	{
		scName = Name;
		Type = VAR_STLSTRING;
		Address = Str;
		DateStore = NULL;
	}

	//color
	_VarProperty(LPSTR Name,D3DCOLOR* pColor)
	{
		scName = Name;
		Type = VAR_COLOR;
		Address = pColor;
		DateStore = NULL;
	}	
	_VarProperty(LPSTR Name,D3DCOLORVALUE* pColor)
	{
		scName = Name;
		Type = VAR_COLORVALUE;
		Address = pColor;
		DateStore = NULL;
	}	

	//Material list for model
	_VarProperty(LPSTR Name,KModel* pModel)
	{
		scName = Name;
		Type = VAR_MTLLIST;
		Address = pModel;
		DateStore = NULL;
	}	


	//Material list for model
	/*_VarProperty(LPSTR Name,LPMODEL pModel,KModel::LPANIMATION* pAni)
	{
		scName = Name;
		Type = VAR_ANILIST;
		Address = pModel;
		DateStore = NULL;
	}*/	
	//////////////////////////////////////////////////////////////////////////////////////////////
	//模型编辑器用来创建特效界面
	_VarProperty(LPSTR Name,KSceneModelEditor* pModel)
	{
		scName = Name;
		Type = VAR_MTLLIST;
		Address = pModel;
		DateStore = NULL;
	}

	_VarProperty(LPSTR Name,KSceneSceneEdit* pModel)
	{
		scName = Name;
		Type = VAR_SCENEEDITOROBJECT;
		Address = pModel;
		DateStore = NULL;
	}
	//////////////////////////////////////////////////////////////////////////

	_VarProperty()
	{
		Type = VAR_NONE;
		Address = NULL;
		DateStore = NULL;
	}
	~_VarProperty()
	{
		if ( Type != VAR_TIMEPOINT && Type != VAR_TIMEPOINT_COLOR)
		{
			Type = VAR_NONE;
			Address = NULL;
			SAFE_DELETE(DateStore);
		}
		else
		{
			Type = VAR_NONE;
			Address = 0;
			DateStore = 0;
		}
	}
}*LPVARPROPERTY,VARPROPERTY;

typedef struct  _VarMethod
{
	TCHAR  Name[256];
	DWORD  Command;
}*LPVARMETHOD,VARMETHOD;

typedef struct  _VarItem
{
	TCHAR  Name[256];
	DWORD  Type;
	BOOL   Extracted;
	vector<LPVARPROPERTY>PropertyVector;
	vector<LPVARMETHOD>MethodVector;
	_VarItem()
	{
		wsprintf(Name,"");
		Type =0;
		Extracted = FALSE;
	}
}*LPVARITEM,VARITEM;

typedef struct _PropertyGroup
{
	DWORD  dwType;
	string scGroupName;
	BOOL   bExtracted;
	vector<LPVARITEM>VarItemVector;

	vector<LPVARPROPERTY>PropertyVector;
	vector<LPVARMETHOD>MethodVector;	


	_PropertyGroup(DWORD Type,LPSTR Name,BOOL Extracted)
	{
		dwType = Type;
		bExtracted = Extracted;
		scGroupName = Name;
	}

	_PropertyGroup()
	{
		dwType   = 0;
		bExtracted = FALSE;
	}

	HRESULT Clear()
	{
		for(DWORD i=0;i<PropertyVector.size();i++)
		{
			_VarProperty* pPro = PropertyVector[i];
			SAFE_DELETE(pPro);
		}
		PropertyVector.clear();
		return S_OK;
	}

	void AddPropertyInt(LPSTR Name,int* pInt,int Min,int Max)
	{
		_VarProperty* pPro = new _VarProperty(Name,pInt,Min,Max);
		PropertyVector.push_back(pPro);
	}
	void AddPropertyBool(LPSTR Name,BOOL* pBool)
	{
		_VarProperty* pPro = new _VarProperty(Name,pBool);
		PropertyVector.push_back(pPro);
	}
	void AddPropertyFloat(LPSTR Name,float* pFloat,float Min,float Max,float Step = 0.01f)
	{
		_VarProperty* pPro = new _VarProperty(Name,pFloat,Min,Max,Step);
		PropertyVector.push_back(pPro);
	}
	
	//////////////////////////////////////////////////////////////////////////
	void AddPropertyTimePoint(LPSTR	Name, ModelSFX::KTimePoint* pTimePoint)
	{
		_VarProperty* pPro = new _VarProperty(Name, pTimePoint);
		PropertyVector.push_back(pPro);
	}


	void AddPropertyTimePointColor(LPSTR Name, ModelSFX::KTimePointColor* pTimePointColor)
	{
		_VarProperty* pPro = new _VarProperty(Name,pTimePointColor);
		pPro->Type = _VarProperty::VAR_TIMEPOINT_COLOR;
		PropertyVector.push_back(pPro);
	}
	//////////////////////////////////////////////////////////////////////////
	
	void AddPropertyString(LPSTR Name,LPSTR Str)
	{
		_VarProperty* pPro = new _VarProperty(Name,Str);
		PropertyVector.push_back(pPro);
	}
	void AddPropertyString(LPSTR Name,string* Str)
	{
		_VarProperty* pPro = new _VarProperty(Name,Str);
		PropertyVector.push_back(pPro);
	}
	void AddPropertyEnum(LPSTR Name,int* pEnum,int NumEnum,int* EnumValue,LPSTR* EnumName)
	{
		_VarProperty* pPro = new _VarProperty(Name,pEnum,NumEnum,EnumValue,EnumName);
		PropertyVector.push_back(pPro);
	}
	void AddPropertyColor(LPSTR Name,D3DCOLOR* pColor)
	{
		_VarProperty* pPro = new _VarProperty(Name,pColor);
		PropertyVector.push_back(pPro);
	}
	void AddPropertyColor(LPSTR Name,D3DCOLORVALUE* pColor)
	{
		_VarProperty* pPro = new _VarProperty(Name,pColor);
		PropertyVector.push_back(pPro);
	}
	void AddPropertyMtlList(LPSTR Name,KModel* pModel)
	{
		_VarProperty* pPro = new _VarProperty(Name,pModel);
		pPro->Type = _VarProperty::VAR_MTLLIST;
		PropertyVector.push_back(pPro);
	}
	void AddPropertyMtlTex(LPSTR Name,KModel* pModel)
	{
		_VarProperty* pPro = new _VarProperty(Name,pModel);
		pPro->Type = _VarProperty::VAR_MTLTEX;
		PropertyVector.push_back(pPro);
	}
	void AddPropertyMtlOption(LPSTR Name,KModel* pModel)
	{
		_VarProperty* pPro = new _VarProperty(Name,pModel);
		pPro->Type = _VarProperty::VAR_MTLOPTOIN;
		PropertyVector.push_back(pPro);
	}
	void AddPropertyMtlState(LPSTR Name,KModel* pModel)
	{
		_VarProperty* pPro = new _VarProperty(Name,pModel);
		pPro->Type = _VarProperty::VAR_MTLSTATE;
		PropertyVector.push_back(pPro);
	}
	void AddPropertyMtlEffect(LPSTR Name,KModel* pModel)
	{
		_VarProperty* pPro = new _VarProperty(Name,pModel);
		pPro->Type = _VarProperty::VAR_MTLEFFECT;
		PropertyVector.push_back(pPro);
	}
	void AddPropertyAni(LPSTR Name,KModel* pModel)
	{
		_VarProperty* pPro = new _VarProperty(Name,pModel);
		pPro->Type = _VarProperty::VAR_ANILIST;
		PropertyVector.push_back(pPro);
	}

	void AddPropertySFXNew(LPSTR Name,KSceneModelEditor* pModelEditor)
	{
		_VarProperty* pPro = new _VarProperty(Name,pModelEditor);
		pPro->Type = _VarProperty::VAR_MODELEDITORSFXNEW;
		PropertyVector.push_back(pPro);
	}

	void AddPropertySceneObjectTable(LPSTR Name,KSceneSceneEdit* pEditor)
	{
		_VarProperty* pPro = new _VarProperty(Name,pEditor);
		pPro->Type = _VarProperty::VAR_SCENEEDITOROBJECT;
		PropertyVector.push_back(pPro);
	}


	void AddPropertyPosition(LPSTR Name,D3DXVECTOR3* pVec)
	{
		_VarProperty* pPro = new _VarProperty(Name,pVec);
		PropertyVector.push_back(pPro);
	}

	void AddPropertyParticle(LPSTR Name,KModel* pModel)
	{
		_VarProperty* pPro = new _VarProperty(Name,pModel);
		pPro->Type = _VarProperty::VAR_PARTICLE;
		PropertyVector.push_back(pPro);
	}

	void AddPropertyTerrainGroundTexture(LPSTR Name,KSceneSceneEdit* pEditor)
	{
		_VarProperty* pPro = new _VarProperty(Name,pEditor);
		pPro->Type = _VarProperty::VAR_TERRAIN_GROUND_TEXTURE;
		PropertyVector.push_back(pPro);
	}

}*LPVARGROUP,VARGROUP;

typedef struct _PropertyPage
{
	string scName;
	vector<_PropertyGroup>vecGroup;
	
	int AddGroup(_PropertyGroup& Group)
	{
		vecGroup.push_back(Group);
		return ((int)vecGroup.size()-1);
	}

	_PropertyGroup* FindGroup(int GroupIndex)
	{
		if((GroupIndex>=0)&&(GroupIndex<(int)vecGroup.size()))
		{
			return &vecGroup[GroupIndex];
		}
		return NULL;
	}

	_PropertyPage()
	{
		;
	}
	_PropertyPage(LPSTR Name)
	{
		scName = Name;
	}

	HRESULT Clear()
	{
		for(DWORD i=0;i<vecGroup.size();i++)
		{
			_PropertyGroup* pGroup = &vecGroup[i];
			pGroup->Clear();
		}
		vecGroup.clear();
		return S_OK;
	}
}*LPPROPAGE;

class KPropertyBase  
{ 
public:
	BOOL  m_bNeedReAcquire;
	BOOL  m_bNeedUpdateUI;
	vector<LPVARGROUP>m_PropertyGroupVector;
	
	vector<_PropertyPage>m_vecPage;

public:
	virtual HRESULT Acquire();
	virtual HRESULT OnRefresh();

	_PropertyPage* FindPage(LPSTR Name);
	_PropertyPage* FindPage(int Index);
	int AddPage(LPSTR Name);


	HRESULT AddItemVarProperty(int GroupIndex,int ItemIndex,LPSTR Name,_VarProperty::_VarType Type,PVOID Address);

	HRESULT AddVarProperty(int GroupIndex, LPSTR Name,_VarProperty::_VarType Type,PVOID Address);
	
	int AddVarItem(int GroupIndex,LPSTR Name);
	int AddVerGroup(LPSTR Name,DWORD Type);
	DWORD GetNumVarGroup();
	
	KPropertyBase();
	virtual ~KPropertyBase();

	HRESULT Clear(void);
};
};

#endif // !defined(AFX_SFXBASE_H__1C7FA33A_511F_4282_A96E_D0A570122EEB__INCLUDED_)
