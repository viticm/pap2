// XMesh.h: interface for the KModel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XMESH_H__078CD29E_6320_426E_ABAE_C83A352F390F__INCLUDED_)
#define AFX_XMESH_H__078CD29E_6320_426E_ABAE_C83A352F390F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <map>
#include <string>
#include "d3dx9.h"

#define MODELTYPE_DEFAULT           0
#define MODELTYPE_TERRAIN           1
#define MODELTYPE_STRIP             2
#define MODELTYPE_GROUP             3
#define MODELTYPE_SKYBOX            4
#define MODELTYPE_LIGHTFLARE        5
#define MODELTYPE_MAINTOOL          6
#define MODELTYPE_MOVEGRID          7
#define MODELTYPE_ROTATEGRID        8
#define MODELTYPE_OBJECTTABLETOOL   9
#define MODELTYPE_MATERIALTOOL      10
#define MODELTYPE_COLORPICK         11
#define MODELTYPE_SLIDEBAR          12
#define MODELTYPE_COMMANDBAR        13
#define MODELTYPE_EDITCTRL          14
#define MODELTYPE_TREECTRL          15
#define MODELTYPE_LISTCTRL          16
#define MODELTYPE_ITEM              17
#define MODELTYPE_LISTBOX           18
#define MODELTYPE_ANIMATIONTOOL     19
#define MODELTYPE_MESHTOOL          20
#define MODELTYPE_COMBOBOX          21
#define MODELTYPE_MESHSELECT        22
#define MODELTYPE_BILLBOARD         23
#define MODELTYPE_PROPERTYSET       24
#define MODELTYPE_BLADE             25
#define MODELTYPE_SCREENSFX         26
#define MODELTYPE_HALO              27
#define MODELTYPE_LINEGRASS         28
#define MODELTYPE_PARTICLESYSTEM    29
#define MODELTYPE_WINBASE           30
#define MODELTYPE_BUTTON            31
#define MODELTYPE_TERRAINLOD        50
#define MODELTYPE_LAYEREDVOLUMETRICFOG 51
#define MODELTYPE_DYNAMICLIGHTNING 52
#define MODELTYPE_SPEEDTREE 53
#define MODELTYPE_SCENESFX  54

#define MODELTYPE_SUBDEV            60
#define MODELTYPE_WATER             61
#define MODELTYPE_BELT              62

#define MESH_DELETED       1
#define MESH_OPTIMIZED     1<<1
#define MESH_SKINED        1<<2
#define MESH_BILLBOARD     1<<3
#define MESH_BIND          1<<4
#define MESH_NOMATERAIL    1<<5
#define MESH_PROGRESS      1<<6

#define  MATERIAL_OPTION_HIDE              1

#define  MATERIAL_OPTION_ZBUFFER_FALSE     (1<<1)
#define  MATERIAL_OPTION_ZBUFFER_TRUE      (1<<2)
#define  MATERIAL_OPTION_ZBUFFER_USEW      (1<<3)

#define  MATERIAL_OPTION_FILL_POINT        (1<<4)
#define  MATERIAL_OPTION_FILL_WIREFRAME    (1<<5)
#define  MATERIAL_OPTION_FILL_SOLID        (1<<6)

#define  MATERIAL_OPTION_SHADE_FLAT        (1<<7)
#define  MATERIAL_OPTION_SHADE_GOURAUD     (1<<8)
#define  MATERIAL_OPTION_SHADE_PHONG       (1<<9)

#define  MATERIAL_OPTION_CULL_NONE         (1<<10)
#define  MATERIAL_OPTION_CULL_CW           (1<<11)
#define  MATERIAL_OPTION_CULL_CCW          (1<<12)

#define  MATERIAL_OPTION_SPECULARENABLE    (1<<13)
#define  MATERIAL_OPTION_LIGHTING          (1<<14)

#define  MATERIAL_OPTION_SELECTED          (1<<15)
#define  MATERIAL_OPTION_ZWRITEDISABLE     (1<<16)


#define  ANIMATION_NONE              0
#define  ANIMATION_VERTICES          1
#define  ANIMATION_VERTICES_RELATIVE 2
#define  ANIMATION_BONE              10
#define  ANIMATION_BONE_RELATIVE     11
#define  ANIMATION_BONE_16           12   //低精度骨骼
#define  ANIMATION_BONE_RTS          13
#define  ANIMATION_BONE_KEYONLY      14

#define  ANIMATION_BLADE_LENGTH      100


#define  PLAY_NONE     0
#define  PLAY_CIRCLE   1
#define  PLAY_PINGPANG 2
#define  PLAY_HOLD     3

#define  OPTION_MATERIAL_LIGHTING        100
#define  OPTION_MATERIAL_CULLING         101
#define  OPTION_MATERIAL_FILLMODE        102
#define  OPTION_MATERIAL_MUTIPASS        103
#define  OPTION_MATERIAL_ALPHATEST       104
#define  OPTION_MATERIAL_ALPHABLEND      105
#define  OPTION_MATERIAL_ZBUFFER         106
#define  OPTION_MATERIAL_SHADER          200
#define  OPTION_MATERIAL_PIXELSHADER     201
#define  OPTION_MATERIAL_ALPHAOPERATION  202
#define  OPTION_MATERIAL_MSEFFECT        203


#define  OPTION_TEXTURE_OPERATION    1000
#define  OPTION_TEXTURE_MAPMETHOD    1001
#define  OPTION_TEXTURE_TRANSFROM    1002
#define  OPTION_TEXTURE_ANIMATION    1003
#define  OPTION_TEXTURE_OPERATIONEX  1004

#define defFILE_TAGLEN			8
#define defFILE_DESCLEN			64
#define defFILE_COMMONTAG		"KSword3D"	//文件头部标志
#define defFILE_COPYRIGHTDESC	"Copyright 1988-2004 Kingsoft Corporation. All rights Reserved"		//版权信息（默认填入文件中）

#define BILLBOARD_NONE             0
#define BILLBOARD_POSITION_ONLY    1
#define BILLBOARD_AWAYSUP          2
#define BILLBOARD_NORMAL           3


#define MSEFFECT_WORLD           1
#define MSEFFECT_WORLDI          2
#define MSEFFECT_WORLDIT         3
#define MSEFFECT_VIEW		     4
#define MSEFFECT_VIEWI		     5
#define MSEFFECT_VIEWIT          6
#define MSEFFECT_PROJECTION      7
#define MSEFFECT_PROJECTIONI     8
#define MSEFFECT_PROJECTIONIT    9
#define MSEFFECT_WORLDVIEW       10
#define MSEFFECT_WORLDVIEWI      11
#define MSEFFECT_WORLDVIEWIT     12
#define MSEFFECT_WORLDVIEWPROJ   13
#define MSEFFECT_WORLDVIEWPROJI  14
#define MSEFFECT_WORLDVIEWPROJIT 15
#define MSEFFECT_SUNDIRMATRIX    16

#define MSEFFECT_POSITION        20
#define MSEFFECT_DIRECTION       21
#define MSEFFECT_COLORRGBA       22
#define MSEFFECT_STRING          23

#define MSEFFECT_VOID            30
#define MSEFFECT_BOOL            31
#define MSEFFECT_INT             32
#define MSEFFECT_FLOAT           33

#define MSEFFECT_FLOAT2          34
#define MSEFFECT_FLOAT3          35
#define MSEFFECT_FLOAT4          36

#define MSEFFECT_TEXTURE         50
#define MSEFFECT_TEXTURE1D       51
#define MSEFFECT_TEXTURE2D       52
#define MSEFFECT_TEXTURE3D       53
#define MSEFFECT_TEXTURECUBE     54

#define MSEFFECT_MATERIALAMBIENT   60
#define MSEFFECT_MATERIALDIFFUSE   61
#define MSEFFECT_MATERIALSPECULAR  62
#define MSEFFECT_MATERIALEMISSIVE  63
#define MSEFFECT_MATERIALPOWER     64

#define MSEFFECT_LOCALBBOXMIN     80
#define MSEFFECT_LOCALBBOXMAX     81
#define MSEFFECT_LOCALBBOXSIZE    82

#define MAX_PCA_VECTORS      24
#define MAX_SH_ORDER         6
#define MAX_LIGHTS           3

#define MAX_NUM_CHANNELS     3

#define FVF_NORMAL D3DFVF_XYZ|D3DFVF_DIFFUSE

#define BONE_SCALE_MAGIC_NUMBER 0xffefafcf
#define SEQU       0x53455155


//////////////////////////////////////////////////////////////////////////
//MeshHead Extend Block usage
#define BLEND_MESH_SAVED 1
//MeshHead Extend Block usage end
//////////////////////////////////////////////////////////////////////////

//#define KEY_FRAME_INTERPOLATION_BUG

//////////////////////////////////////////////////////////////////////////
//Add 6-17-2005 Sequence play option definitions
#define SEQU_PLAY_LOOP            1
#define SEQU_PLAY_BACKWARDS  (1<<1) //Backwards is pretty complex, so now is not implemented
//Add 6-17-2005 Sequence play option definitions ends
//////////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>
#include "ResourceMgrEffect.h"
#include "KRope.h"
#include "sfxbase.h"
#include<d3dx9mesh.h>

class KSequence;

typedef class KModel : 	public PropertyBase::KPropertyBase  

{
protected:
	D3DXMATRIX m_MatrixSave;

public:
	
	struct _NormalPoint
	{
		D3DXVECTOR3 vPosition;
		DWORD color;
	};

	struct TFileHeader
	{
		char szCommonTag[defFILE_TAGLEN];
		char szModuleTag[defFILE_TAGLEN];
		__int32 byVersion;
		char szDescription[defFILE_DESCLEN];

		TFileHeader(char* pModuleTag = NULL, __int32 byVer = 0, char* pDescription = NULL)
		{
			Set(pModuleTag, byVer, pDescription);
		}
		void Set(char* pModuleTag = NULL, __int32 byVer = 0, char* pDescription = NULL)
		{
			memset(this, 0, sizeof(TFileHeader));
			strncpy(szCommonTag, defFILE_COMMONTAG, defFILE_TAGLEN);

			if (pModuleTag)
				strncpy(szModuleTag, pModuleTag, defFILE_TAGLEN);

			byVersion = byVer;

			if (pDescription)
				strncpy(szDescription, pDescription, defFILE_DESCLEN);
			else
				strncpy(szDescription, defFILE_COPYRIGHTDESC, defFILE_DESCLEN);
		}

	};

	//////////////////////////////////////////////////////////////////////////
	//Add 6-1-2005 Blend mesh save 
private:
	struct BlendMeshHeader
	{
		DWORD BlockLength;
		DWORD NumVertices;
		DWORD NumFaces;
		DWORD Options;
		DWORD NumAtt;
		DWORD IndexBufferSize;
		DWORD VertexBufferSize;
		D3DVERTEXELEMENT9 decl[MAX_FVF_DECL_SIZE];
		DWORD BoneCombinSize;
		DWORD MaxFaceInfl;
	};
    typedef BlendMeshHeader* LPBLENDMESHHEADER;
	DWORD* m_pBondID;
public:
	HRESULT SaveBlendedMesh(ID3DXMesh* pMesh, FILE* pFile);
	HRESULT LoadBlendMesh(FILE* pFile, LPD3DXMESH* ppMesh);
	HRESULT LoadBlendMesh(void* pBuffer, LPD3DXMESH* ppMesh);
	//Add 6-1-2005 Blend mesh save 
	//////////////////////////////////////////////////////////////////////////




public:

	struct _MeshHead
	{
		DWORD  NumVertices;
		DWORD  NumFaces;
		DWORD  NumSubset;
		DWORD  PositionBlock;
		DWORD  NormalBlock;
		DWORD  DiffuseBlock;
		DWORD  TextureUVW1Block;
		DWORD  TextureUVW2Block;
		DWORD  TextureUVW3Block;
		DWORD  FacesIndexBlock;
		DWORD  SubsetIndexBlock;
		DWORD  SkinInfoBlock;
		DWORD  LODInfoBlock;
		DWORD  ExtendBlock[20];

		_MeshHead()
		{
			NumVertices      = 0;
			NumFaces         = 0;
			NumSubset        = 0;
			PositionBlock    = 0;
			NormalBlock      = 0;
			DiffuseBlock     = 0;
			TextureUVW1Block = 0;
			TextureUVW2Block = 0;
			TextureUVW3Block = 0;
			FacesIndexBlock  = 0;
			SubsetIndexBlock = 0;
			SkinInfoBlock    = 0;
			LODInfoBlock     = 0;
			ZeroMemory(ExtendBlock,sizeof(DWORD)*20);
		}
	};

	struct _MeshFileHead
	{
		TFileHeader KSFileHeader;
		DWORD       FileMask;
		DWORD       BlockLength;
		DWORD       MaterialBlock;
		DWORD       AnimationBlock;
		DWORD       ExtendBlock[10];
		DWORD       MeshCount;

		_MeshFileHead()
		{
			FileMask       = 0x4D455348;
			BlockLength    = 0;
			MaterialBlock  = 0;
			AnimationBlock = 0;
			ZeroMemory(ExtendBlock,sizeof(DWORD)*10);
			MeshCount      = 1;
		};
	};
	typedef struct _MatCull 
	{
		DWORD CullMode;
		DWORD BakCullMode;
	}*LPMATCULL;
	typedef struct _MatBlend
	{
		DWORD SrcBlend;
		DWORD DestBlend;
		DWORD BakSrcBlend;
		DWORD BakDestBlend;
		_MatBlend()
		{
			SrcBlend  = D3DBLEND_SRCALPHA;
			DestBlend = D3DBLEND_INVSRCALPHA;
		}
	}*LPMATBLEND;
	
	//////////////////////////////////////////////////////////////////////////
	//高级的ALPHA操作设置
	typedef struct MTLALPHAOPERATION
	{
		DWORD EnableAlphaBlend;
		DWORD SrcBlend;
		DWORD DestBlend;
		DWORD BlendFactor;
		DWORD BlendOperator;

		DWORD EnableSeparateAlphaBlend;
		DWORD BlendOperatorAlpha;
		DWORD SrcBlendAlpha;
		DWORD DestBlendAlpha;

		DWORD EnableAlphaTest;
		DWORD AlphaTestRef;
		DWORD AlphaFunction;

		DWORD BackEnableAlphaBlend;
		DWORD BackSrcBlend;
		DWORD BackDestBlend;
		DWORD BackBlendFactor;
		DWORD BackBlendOperator;

		DWORD BackEnableSeparateAlphaBlend;
		DWORD BackBlendOperatorAlpha;
		DWORD BackSrcBlendAlpha;
		DWORD BackDestBlendAlpha;

		DWORD BackEnableAlphaTest;
		DWORD BackAlphaTestRef;
		DWORD BackAlphaFunction;

		MTLALPHAOPERATION()
		{
			EnableAlphaBlend = TRUE;
			SrcBlend = D3DBLEND_SRCALPHA;
			DestBlend = D3DBLEND_INVSRCALPHA;
			BlendFactor = 0xffffffff;
			BlendOperator = D3DBLENDOP_ADD;

			EnableSeparateAlphaBlend = FALSE;
			BlendOperatorAlpha = D3DBLENDOP_ADD;
			SrcBlendAlpha = D3DBLEND_ONE;
			DestBlendAlpha = D3DBLEND_ZERO;

			EnableAlphaTest = TRUE;
			AlphaTestRef = 0x20;
			AlphaFunction = D3DCMP_GREATEREQUAL;
		}

	}*LPMTLALPHAOPERATION;
	typedef struct _TextureMap
	{
		DWORD TextureMap;
		DWORD BakTextureMap;
		_TextureMap()
		{
			TextureMap = D3DTSS_TCI_PASSTHRU;
		}
	}*LPTEXTUREMAP;
	typedef struct _TextureOp
	{
		DWORD ColorOp;
		DWORD ColorArg1;
		DWORD ColorArg2;
		DWORD AlphaOp;
		DWORD AlphaArg1;
		DWORD AlphaArg2;
		DWORD TFactor;
		DWORD BakColorOp;
		DWORD BakColorArg1;
		DWORD BakColorArg2;
		DWORD BakAlphaOp;
		DWORD BakAlphaArg1;
		DWORD BakAlphaArg2;
		DWORD BakTFactor;

		_TextureOp()
		{
			ColorOp = D3DTOP_SELECTARG1;
			AlphaOp = D3DTOP_DISABLE;	
			AlphaArg1 = D3DTA_TEXTURE;
			AlphaArg2 = D3DTA_CURRENT;
			ColorArg1 = D3DTA_TEXTURE;
			ColorArg2 = D3DTA_CURRENT;	
			TFactor = 0;
		}

	}*LPTEXTUREOP;

	//////////////////////////////////////////////////////////////////////////
	//高级的图层操作
	typedef struct _TextureOpEx
	{
		DWORD ColorOp;
		DWORD ColorArg1;
		DWORD ColorArg2;
		DWORD ColorArg0;
		DWORD Result;
		DWORD AlphaOp;
		DWORD AlphaArg1;
		DWORD AlphaArg2;
		DWORD AlphaArg0;
		DWORD AlphaResult;
		DWORD TFactor;
		DWORD Constant;

		DWORD BakColorOp;
		DWORD BakColorArg1;
		DWORD BakColorArg2;
		DWORD BakColorArg0;
		DWORD BakResult;
		DWORD BakAlphaOp;
		DWORD BakAlphaArg1;
		DWORD BakAlphaArg2;
		DWORD BakAlphaArg0;
		DWORD BakTFactor;
		DWORD BakConstant;

		_TextureOpEx()
		{
			ColorOp = D3DTOP_SELECTARG1;
			AlphaOp = D3DTOP_DISABLE;	
			AlphaArg1 = D3DTA_TEXTURE;
			AlphaArg2 = D3DTA_CURRENT;
			ColorArg1 = D3DTA_TEXTURE;
			ColorArg2 = D3DTA_CURRENT;	

			ColorArg0 = D3DTA_CURRENT;
			AlphaArg0 = D3DTA_CURRENT;
			Result = D3DTA_CURRENT;
			DWORD TFactor =0;
			Constant = D3DTA_CONSTANT;
		}

	}*LPTEXTUREOPEX;	

	typedef struct _TextureTf
	{
		D3DXVECTOR3 Scaling;
		D3DXVECTOR3 Rotation;
		D3DXVECTOR3 Position;
		D3DXVECTOR3 Speed_Scaling;
		D3DXVECTOR3 Speed_Rotation;
		D3DXVECTOR3 Speed_Position;
	}*LPTEXTURETF;



	typedef struct SHADERCONSTANT
	{
		DWORD  dwType;
		string scName;
		PVOID  pData;
		D3DXHANDLE m_hParameter;

		SHADERCONSTANT()
		{
			dwType = 0;
			pData  = NULL;
		}
		
		SHADERCONSTANT(DWORD Type,string Name,PVOID Data,D3DXHANDLE hParameter)
		{
			dwType = Type;
			scName = Name;
			pData = Data;
			m_hParameter = hParameter;
		}

		HRESULT Clear();
		HRESULT GetDesc(LPSTR pStr);
		HRESULT GetParameter(LPSTR pStr);

		bool operator == (SHADERCONSTANT& Const)
		{
			if ((dwType == Const.dwType)&&(scName==Const.scName))
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}*LPSHADERCONSTANT;
	
	typedef struct MTLSHADEROPTION
	{
		vector<SHADERCONSTANT>VertexShaderConstant;
		vector<SHADERCONSTANT>PixelShaderConstant;
		string VertexShaderFile;
		string PixelShaderFile;
		
	}*LPMTLSHADEROPTION;

	struct MATERIAL;

	typedef struct _TextureData
	{
		string FileName;
		DWORD  TextureID;

		_TextureData()
		{
			TextureID = 0;
		}

		~_TextureData()
		{
			FileName.clear();
			TextureID = 0;
		}
	}*LPTEXTUREDATA;

	typedef struct MTLMSEFFECT
	{


		vector<SHADERCONSTANT>InputConstant;
		KResourceHandleEffect* m_HEffect;

		int m_nCurTechnique;

		MTLMSEFFECT()
		{
			m_nCurTechnique = -1;
			m_HEffect = NULL;
		}

		virtual ~MTLMSEFFECT();

		virtual HRESULT OnLoad();
		virtual HRESULT SetParameter(MATERIAL* pMaterial,KModel* pModel);
		virtual HRESULT SetParameter(KModel* pModel);

	}*LPMTLMSEFFECT;

	typedef struct _Option
	{
		DWORD Type;
		PVOID pData;
		_Option()
		{
			Type  =  0;
			pData = NULL;
		}
		~_Option()
		{
			Type  =  0;
			SAFE_DELETE(pData);
		}
	}* LPOPTION,OPTION;

	typedef struct MATERIAL
	{
		DWORD            m_dwNumFaces;
		D3DMATERIAL9     m_sMaterial9;
		DWORD            m_dTextureIDs[8];
		string           m_scTextureName[8];
		string           m_scTextureType[8];
		DWORD            m_lpTextureNumOption[8];
		vector<LPOPTION> m_lpTextureOptions[8];
		DWORD            m_dOption;
		DWORD            m_dwNumOptions;
		vector<LPOPTION> m_lpMatOptions;

		LPMTLMSEFFECT    m_lpEffect;//非NULL表示使用effect 
		
		MATERIAL()
		{
			for(int i=0;i<8;i++)
			{
				m_scTextureName[i].empty();
				m_scTextureType[i].empty();
				m_lpTextureNumOption[i] =0;
			}
			m_dwNumOptions = 0;
			m_dOption = 0;
			m_lpEffect = NULL;
			m_dwNumFaces = 0;
		}
		~MATERIAL()
		{
			//MessageBox(NULL,"Material disconsturct","Note",0);
			for(int i=0;i<8;i++)
			{
				//m_scTextureName[i].empty();
				//m_scTextureType[i].empty();

				for(int j=0;j<(int)m_lpTextureOptions[i].size();j++)
				{
					LPOPTION pOption= m_lpTextureOptions[i][j];
					SAFE_DELETE(pOption);
				}
				m_lpTextureNumOption[i] = 0;
				m_lpTextureOptions[i].clear();
			}
			for(i=0;i<(int)m_lpMatOptions.size();i++)
			{
				LPOPTION pOption = m_lpMatOptions[i];
				SAFE_DELETE(pOption);
			}
			m_dwNumOptions = 0;
			m_lpMatOptions.clear();
			SAFE_DELETE(m_lpEffect);
		}
		
		LPMTLALPHAOPERATION GetAlphaOperation()
		{
			DWORD NunOption = m_dwNumOptions;

			LPOPTION pOp = NULL;
			for(DWORD i=0;i<NunOption;i++)
			{
				pOp = m_lpMatOptions[i];
				if (pOp->Type == OPTION_MATERIAL_ALPHAOPERATION)
				{
					KModel::LPMTLALPHAOPERATION pAlpha = (KModel::LPMTLALPHAOPERATION) pOp->pData;
					return pAlpha;
				}
			}

			pOp = new KModel::OPTION;
			KModel::LPMTLALPHAOPERATION pAlpha = new KModel::MTLALPHAOPERATION;
			pOp->pData = pAlpha;
			pOp->Type = OPTION_MATERIAL_ALPHAOPERATION;
			m_lpMatOptions.push_back(pOp);
			m_dwNumOptions++;
			return pAlpha;
		}

		LPTEXTUREOPEX GetTextureStageOperation(DWORD TextIndex)
		{
			if(TextIndex>=8)
				return NULL;

			DWORD NunOption = m_lpTextureOptions[TextIndex].size();

			KModel::LPOPTION pOp = NULL;
			for(DWORD i=0;i<NunOption;i++)
			{
				pOp = m_lpTextureOptions[TextIndex][i];
				if (pOp->Type == OPTION_TEXTURE_OPERATIONEX)
				{
					KModel::LPTEXTUREOPEX pTextureOp = (KModel::LPTEXTUREOPEX) pOp->pData;
					return pTextureOp;
				}
			}

			pOp = new KModel::OPTION;
			KModel::LPTEXTUREOPEX pTextOp = new KModel::_TextureOpEx;
			pOp->pData = pTextOp;
			pOp->Type = OPTION_TEXTURE_OPERATIONEX;
			m_lpTextureOptions[TextIndex].push_back(pOp);
			m_lpTextureNumOption[TextIndex]++;
			return pTextOp;
		}

	}*LPMATERIAL;

	struct _SKinSubsetInfo
	{
		int AttribID;
		int NumBlend;
		_SKinSubsetInfo()
		{
			AttribID = 0;
			NumBlend = 0;
		}
	};
	typedef struct _SKinSubset//为带骨骼的网格整理一下索引值
	{
		vector<_SKinSubsetInfo>SubsetIndex;
		~_SKinSubset()
		{
			SubsetIndex.clear();
		}
	}*LPSKINSUBSET;

	vector<_SKinSubset>m_vecSkinSubset;
	
	struct RTS
	{
		D3DXVECTOR3 Translation;
		D3DXVECTOR3 Scale;
		D3DXQUATERNION Rotation;
		float Sign;
		D3DXQUATERNION SRotation;
	};
	typedef RTS* LPRTS;

	typedef struct BONEINFO
	{
		string      scName;
		string      scParentName;
		
		vector<string>ChildNameVector;
		vector<int>   ChildIDVector;
		int           nParentIndex;

		DWORD       NumVerticesRef;
		DWORD*      VerticesIndexRef;
		float*      VerticesWeightRef;
		D3DXMATRIX  MatrixOffset;
		D3DXMATRIX  MatrixOffsetBase;
		D3DXMATRIX  CurMatrix;
		D3DXMATRIX  RelativeMatrix;

		float       BoneLength;
		D3DXVECTOR3 BonePosition;//骨骼当前位置
		D3DXVECTOR3 BaseBonePosition;//骨骼初试位置
		D3DXVECTOR3 BonePositionOffset;              
////////////////柔体部分新定义的数据结构///////////////////
		D3DXMATRIX  LocalWorldCurMatrix; //骨骼在本身世界中的位置矩阵

/////////////////////////////////////////////////////////////

		D3DXVECTOR3 BoneScale;
		D3DXVECTOR3 BaseBoneScale;

		D3DXMATRIX  BaseRotation;
		D3DXMATRIX  Rotation;

		BOOL        bFlexibleBone;
		BONEINFO()
		{
			scName.empty();
			scParentName.empty();
			NumVerticesRef = 0;
			VerticesIndexRef = NULL;
			VerticesWeightRef = 0;
			BoneLength = 0;
			bFlexibleBone = FALSE;
			nParentIndex = -1;
		}
		~BONEINFO()
		{
			scName.empty();
			scParentName.empty();
			ChildNameVector.clear();
			ChildIDVector.clear();

			NumVerticesRef = 0;
			SAFE_DELETE_ARRAY(VerticesIndexRef);
			SAFE_DELETE_ARRAY(VerticesWeightRef);
			BoneLength = 0;
		}
	public:
		void UpdateCurMatrix();

	}* LPBONEINFO;

	
	typedef struct _BoneBlendInfo
	{
		LPSTR       Name;
		D3DXMATRIX  MatrixBaseOffset;
		float       BoneLength;
		_BoneBlendInfo()
		{
			Name = NULL;
			D3DXMatrixIdentity(&MatrixBaseOffset);
			BoneLength = 0;
		}

		_BoneBlendInfo(LPSTR pName,D3DXMATRIX* pMat)
		{
			Name = new TCHAR[256];
			wsprintf(Name,"%s",pName);
			MatrixBaseOffset = *pMat;
			
			D3DXVECTOR3 S;
			S.x = MatrixBaseOffset._41;
			S.y = MatrixBaseOffset._42;
			S.z = MatrixBaseOffset._43;
				
			BoneLength = D3DXVec3LengthSq(&S);
		}

		void Create(LPSTR pName,D3DXMATRIX* pMat)
		{
			if (!Name)
				Name = new TCHAR[256];
			wsprintf(Name,"%s",pName);
			MatrixBaseOffset = *pMat;

			D3DXVECTOR3 S;
			S.x = MatrixBaseOffset._41;
			S.y = MatrixBaseOffset._42;
			S.z = MatrixBaseOffset._43;
				
			BoneLength = D3DXVec3LengthSq(&S);
		}

		~_BoneBlendInfo()
		{
			SAFE_DELETE_ARRAY(Name);
		}
	}* LPBONEBLENDINFO,BONEBLENDINFO;

	typedef struct _BoneBlendSet
	{
		TCHAR           SetName[256];
		DWORD           NumBones;
		LPBONEBLENDINFO BoneBlendInfos;
		_BoneBlendSet()
		{
			NumBones = 0;
			BoneBlendInfos = NULL;
		}
		~_BoneBlendSet()
		{
			NumBones = 0;
			SAFE_DELETE_ARRAY(BoneBlendInfos);
		}
	}* LPBONEBLENDSET,BONEBLENDSET;

	typedef struct _EdgeVertex
	{
		VFormat::FACES_NORMAL_TEXTURE1  Vertex;
		int ParentFace[2];///每一个边点都关联两个三角形
		WORD Index;

		_EdgeVertex()
		{
			ParentFace[0] = -1;
			ParentFace[1] = -1;
			Index = -1;
		}
		//DWORD	ParentNeighbor[3]; 
	}* LPEdgeVertex,EdgeVertex;

	typedef struct _OriginEdgeVer
	{
		BOOL EdgeVer;
		int VerIndex;//由于每个边点会给访问两次，所以第二次可以根据这个值方便找到上次放入的位置
		_OriginEdgeVer()
		{
			EdgeVer=FALSE;
			VerIndex= -1;
		}
	}* LPOriginEdgeVer, OriginEdgeVer;
    
	typedef struct _Socket
	{
		string      scName;
		string      scParentBoneName;
		D3DXMATRIX  MatrixBase;
		D3DXMATRIX  MatrixOffset;//偏移矩阵
		D3DXMATRIX  MatrixOffsetBack;//偏移矩阵
		D3DXMATRIX  CurMatrix;

		D3DXVECTOR3 CurPosition;
		D3DXVECTOR3 CurDirectionPosition;

		D3DXVECTOR3 YawPitchRoll;
		_Socket()
		{
			scName.empty();
			scParentBoneName.empty();
			YawPitchRoll = D3DXVECTOR3(0,0,0);
		}
		~_Socket()
		{
			scName.empty();
			scParentBoneName.empty();
		}

		void GetCurPosition()
		{
			CurPosition.x = CurMatrix._41;
			CurPosition.y = CurMatrix._42;
			CurPosition.z = CurMatrix._43;
		}
		void GetCurDirectionPosition()
		{
			GetCurPosition();

			D3DXVECTOR3 Y(0,1,0);
			D3DXVECTOR3 Dir(0,0,0);

			D3DXVec3TransformNormal(&Dir,&Y,&MatrixBase);
			CurDirectionPosition = CurPosition + Dir * 100;
		}
	}*LPSOCKET,SOCKET;

	typedef struct _Animation
	{
		DWORD   Type;          //动画类型
		LPSTR   Name;          //动画名称
		DWORD   NumFrames;     //动画包含的桢数
		float   FrameLength;   //动画桢之间的时间间隔
		//////////////////////////////////////////////////////////////////////
		//顶点动画
		DWORD        NumAnimatedVertice;      //动画顶点的数目
		DWORD*       AnimatedVerticesIndex;   //动画顶点索引
		D3DXVECTOR3* AnimatedVerticesPosition;//动画顶点的位置信息
		//////////////////////////////////////////////////////////////////////
		//骨骼动画
		DWORD        NumAnimatedBone;         //动画骨骼的数目
		LPSTR*       AnimatedBoneName;        //动画骨骼的名称
		D3DXMATRIX*  AnimatedBoneMatrix;      //动画骨骼的变换（4X4矩阵）
		D3DXVECTOR3*     BoneScaling;
		D3DXQUATERNION * BoneRotation;
		D3DXVECTOR3*     BoneTranslation;
		RTS*         AnimatedBoneRTS;//Bone's rotation, translation & Scale
		DWORD*       Offset;//非等距帧（关键帧方式）在整个动画数组中的偏移信息
		BOOL*        Static;
		//////////////////////////////////////////////////////////////////////////
		//刀光动画
		PVOID            BladeData;
		DWORD            NumBlade;

		      
		struct _Blade_ShowLength
		{
			float fLength;
			BOOL  bShow;
		};

		_Animation()
		{
			Type = 0;
			Name = NULL;
			NumFrames = 0;
			FrameLength = 0;
			NumAnimatedVertice = 0;
			AnimatedVerticesIndex = NULL;
			AnimatedVerticesPosition = NULL;
			NumAnimatedBone = 0;
			AnimatedBoneName = NULL;
			AnimatedBoneMatrix = NULL;
			BoneScaling = NULL;
			BoneRotation = NULL;
			BoneTranslation = NULL;
			BladeData = NULL;
			AnimatedBoneRTS = NULL;
			NumBlade = 0;
			Offset = NULL;
			Static = NULL;
		}
		~_Animation()
		{
			SAFE_DELETE_ARRAY(AnimatedBoneMatrix);
			SAFE_DELETE_ARRAY(AnimatedBoneRTS);
			for(DWORD i=0;i<NumAnimatedBone;i++)
			{
				SAFE_DELETE_ARRAY(AnimatedBoneName[i])
			}
			SAFE_DELETE_ARRAY(AnimatedBoneName);
			
			SAFE_DELETE_ARRAY(AnimatedVerticesIndex);
			SAFE_DELETE_ARRAY(AnimatedVerticesPosition);
			SAFE_DELETE_ARRAY(Name);
			SAFE_DELETE_ARRAY(BladeData);
			SAFE_DELETE_ARRAY(Offset);
			SAFE_DELETE_ARRAY(Static);
			NumBlade = 0;
		}
	}*LPANIMATION,ANIMATION;
	
	struct _ShadowVolume
	{
		D3DXVECTOR3* EdageVertices;
		DWORD        NumVertices;
		WORD*        Edages;
	}m_ShadowVolume;

	int               m_nBillBoardMethod;
	int               m_nNumCurBlendSet;

	vector<LPBONEBLENDSET>m_lpBoneBlendSet;

	DWORD             m_dwBoneBlendsetID[10];
	float             m_fBoneBlendsetWeight[10];

	D3DXMATRIX   m_Matrix;
	D3DXMATRIX   m_CurMatrix;
	KModel*      m_pBindMesh;
	LPSTR        m_pBindSocketName;

	DWORD        m_dwType;
	DWORD        ID;
	DWORD        m_dOption;
	DWORD        m_dNumMaterial;
	DWORD        m_dNumBone;
	DWORD        m_dNumSocket;
	DWORD        m_dNumAnimation;
	DWORD        m_MaxFaceInfl;
	DWORD        m_NumAttributeGroups;
	DWORD        m_iAttributeSW;

	//////////////////////////////////////////////////////

	DWORD        m_dwShadow ;
	float        m_fPlanarShadowHeight;

	D3DXVECTOR3  m_BBox_A;
	D3DXVECTOR3  m_BBox_B;

	int          m_nCurAnimationIndex;
	DWORD        m_dCurAnimationMethod;
	float        m_fCurAnimationSpeed;
	float        m_fCurAnimationStart;

	vector<LPANIMATION>m_lpAnimations;

	LPSOCKET     m_lpSockets;
	D3DXMATRIX*  m_pBoneMatrices;
	LPMATERIAL   m_lpMaterial;
	D3DXVECTOR3* m_lpStaticVertexPos;
	LPD3DXBUFFER m_pBoneCombinationTable;
	LPD3DXBUFFER m_pVertexRemap;

	LPDIRECT3DVERTEXSHADER9        m_pCartoonShader ;
	LPDIRECT3DVERTEXSHADER9        m_pFrameShader   ;
	LPDIRECT3DVERTEXDECLARATION9   g_pVertexDeclaration;

	LPD3DXMESH        m_pMeshOpt;	
	LPD3DXMESH        m_pBlendMesh;	//Skined blend mesh
	LPD3DXMESH        m_pMeshSys;   //Mesh in system memery,slow
	LPD3DXMESH        m_pMeshPRT;   //预选染辐射度所使用模型
	LPD3DXPMESH      m_pMeshProgress;
	LPD3DXMESH        m_pMeshBeforeProgress;//lod之前的mesh

	ID3DXMesh*            m_pMeshEnhanced; // vid mem version of mesh that is enhanced
	int                             m_nNumCurFacesLOD;
	
	float*                         m_pVerticesLodWeight;//LOD顶点优化权重
    //////////////////////////////////////////////////////////////////////////
	//PRT TEST DRAW NORMAL
	LPDIRECT3DVERTEXBUFFER9 m_pVBNormal;
	
	LPD3DXSKININFO    m_pSkinInfo;

	struct _FlexibleBone
	{
		int			Index;
		float		fWaveDegree;
		D3DXMATRIX	MatrixFlexWold; //每根柔体骨骼的局部坐标系 
		D3DXVECTOR3 PrePosition; //前一帧的位置
		D3DXVECTOR3 MoveForce;
//		D3DXVECTOR3 PreMoveForce;
		Water::RopeSimulation* ropeSimulation;
		_FlexibleBone()
		{
			fWaveDegree=1.0f;
			D3DXMatrixIdentity(&MatrixFlexWold);
			MoveForce = D3DXVECTOR3( 0.0f, 0.0f ,0.0f );
//			PreMoveForce = D3DXVECTOR3( 0.0f, 0.0f ,0.0f );	
		}


		~_FlexibleBone()
		{
//			ropeSimulation->release();												// Release The ropeSimulation
//			delete(ropeSimulation);													// Delete The ropeSimulation
//			ropeSimulation = NULL;	
		}
	};

	vector<_FlexibleBone>m_vecStaticFlexibleBoneIndex;//柔体骨骼固定点的索引

	LPBONEINFO        m_lpBoneInfo;
	vector<int>       m_nBaseBoneID;

	TCHAR    m_szMeshFileName[256];
	TCHAR    m_szMtlFileName[256];
	TCHAR    m_szAniFileName[256];


	//LPDIRECT3DTEXTURE9 m_pSnapTexture;
	BOOL				    m_bRecaculateNormal;
	D3DXVECTOR3             m_vDefaultLightDir[MAX_LIGHTS];
	D3DXVECTOR3             m_vLightDir[MAX_LIGHTS];
	D3DXMATRIXA16           m_mLightRot[MAX_LIGHTS];
	D3DXMATRIXA16           m_mLightRotSnapshot[MAX_LIGHTS];
	float                   m_fLightScale;
	int                     m_nNumActiveLights;
	int                     m_nActiveLight;

//////////////////////////////////////////////////////////////////////////
//PRT'S STARTS HERE

//////////////////////////////////////////////////////////////////////////
//For 9.0b
//#if D3D_SDK_VERSION == 31 
	ID3DXBuffer*            m_pSHPRTBuffer;
	LPD3DXEFFECT      m_pSHPRTEffect;
	CRITICAL_SECTION        m_cs;
	BOOL                    m_bSHPRTReadyToRender;
	BOOL					m_bDrawNormal;
	bool                    m_bPRTSimulationComplete;
	bool                    m_bPRTSimulationFailed;
	bool                    m_bSHPRTTooManyConstants;
	HANDLE                  m_hPRTSimulationThreadId;
	DWORD                   m_dwPRTSimulationThreadId;
	float                   m_bStopSimulator;
	float                   m_fPercentDone;


	// Simulation options
	//TCHAR                   m_strInitialDir[MAX_PATH];
	//TCHAR                   m_strInputMesh[MAX_PATH];
	DWORD                   m_dwOrder;
	DWORD                   m_dwNumRays;
	DWORD                   m_dwNumBounces;
	DWORD                   m_dwNumClusters;
	DWORD                   m_dwNumPCAVectors;
	BOOL                    m_bSubsurfaceScattering;
	float                   m_fLengthScale;
	BOOL                    m_bSpectral;
	DWORD                   m_dwPredefinedMatIndex;
	D3DXCOLOR               m_DiffuseReflectance;
	D3DXCOLOR               m_Absoption;
	D3DXCOLOR               m_ReducedScattering;
	float                   m_fRelativeIndexOfRefraction;
	//TCHAR                   m_strResultsFile[MAX_PATH];


	// The basis buffer is a large array of floats where 
	// Call D3DXSHPRTCompExtractBasis() to extract the basis 
	// for every cluster.  The basis for a cluster is an array of
	// (NumPCAVectors+1)*(NumChannels*Order^2) floats. 
	// The "1+" is for the cluster mean.
	float* m_aClusterBases;

	// m_fClusteredPCA stores the incident radiance dotted with the transfer function.
	// Each cluster has an array of floats which is the size of 
	// 4+MAX_NUM_CHANNELS*NUM_PCA_VECTORS. This number comes from: there can 
	// be up to 3 channels (R,G,B), and each channel can 
	// have up to NUM_PCA_VECTORS of PCA vectors.  Each cluster also has 
	// a mean PCA vector which is described with 4 floats (and hence the +4).
	float* m_aClusteredPCA;
public:
#if D3D_SDK_VERSION == 31	
	/*HRESULT PRT_CompressData();
	HRESULT PRT_SetupVBWithPCAData( ID3DXMesh* pMesh, ID3DXBuffer* pCPCABuffer,D3DXSHPRTBUFFER_DESC desc,
		                            DWORD dwNumClusters, DWORD dwNumPCAVectors );
	HRESULT	PRT_LoadResult();
	HRESULT PRT_LaunchSimulation();
	HRESULT PRT_ReadBufferFromFile( TCHAR* strFile, ID3DXBuffer** ppd3dxBuffer );
	HRESULT PRT_StopSimulation();
	static DWORD WINAPI PRT_StaticPRTSimulationThreadProc( LPVOID lpParameter );
	
	HRESULT PRT_RenderMeshWithSHPRT();
	HRESULT PRT_AdjustMeshDecl( ID3DXMesh** ppMesh );
	HRESULT PRT_UpdateLightDir();
	HRESULT PRT_EvalLightsAndSetConstants();
	HRESULT PRT_SetShaderConstants( float* pfRLC, float* pfGLC, float* pfBLC );
	HRESULT PRT_LoadSHPRTEffect();
	HRESULT PRT_SaveSHPRTBufferToFile();*/
#endif
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//For 9.0c
#if D3D_SDK_VERSION == 32
private:
	struct RELOAD_STATE
	{
		bool  bUseReloadState;
		bool  bLoadCompressed;
		TCHAR strPRTBufferFileName[MAX_PATH];
		D3DXSHCOMPRESSQUALITYTYPE quality;
		UINT dwNumClusters;
		UINT dwNumPCA;
	} m_PRTReloadState;
	LPD3DXPRTBUFFER m_pPRTBuffer;
	LPD3DXPRTCOMPBUFFER m_pPRTCompBuffer;
	LPD3DXEFFECT        m_pPRTEffect;       
	LPD3DXEFFECT        m_pSHIrradEnvMapEffect;       
	LPD3DXEFFECT        m_pNDotLEffect;
	float*				m_aPRTConstants;
	
public:
	float m_fPRTLightScale;
	float m_fPRTConeRadius;
	D3DXCOLOR m_PRTLightColor;
	HRESULT PRT_GetAlbedoTextures();
	UINT PRT_GetOrderFromNumCoeffs(UINT dwNumCoeffs);
	void PRT_RenderWithSHIrradEnvMap(bool bRenderWithAlbedo);
	void PRT_RenderWithNdotL(bool bRenderWithAlbedo, int nNumLights, float fLightScale);
	void PRT_RenderWithPRT(bool bRenderWithAlbedo);
	void PRT_ComputeShaderConstants(float* pSHCoeffsRed, float* pSHCoeffsGreen, float* pSHCoeffsBlue, DWORD dwNumCoeffsPerChannel);
	void PRT_ComputeSHIrradEnvMapConstants(float* pSHCoeffsRed, float* pSHCoeffsGreen, float* pSHCoeffsBlue);
	HRESULT PRT_ExtractCompressedDataForPRTShader();
	void PRT_SetPRTBuffer(ID3DXPRTBuffer* pPRTBuffer, TCHAR* strFile);
	void PRT_CompressBuffer( D3DXSHCOMPRESSQUALITYTYPE Quality, UINT NumClusters, UINT NumPCA);
	HRESULT PRT_LoadCompPRTBufferFromFile(TCHAR* strFile);
	HRESULT PRT_LoadPRTBufferFromFile( TCHAR* strFile);
	
	HRESULT PRT_Initialize();
	HRESULT PRT_LoadEffects();
	LPD3DXPRTCOMPBUFFER PRT_GetCompBuffer() { return m_pPRTCompBuffer;}
	vector<IDirect3DTexture9*> m_pAlbedoTextures;
	DWORD   PRT_GetOrder() { return m_dwOrder; }
	HRESULT PRT_AdjustMeshDecl();
	void PRT_UpdateLightingEnvironment();
	IDirect3DTexture9* GetAlbedoTexture() { return m_pAlbedoTextures[0]; }

#endif
//////////////////////////////////////////////////////////////////////////
//PRT End Here	
//////////////////////////////////////////////////////////////////////////
public:
	virtual HRESULT Copy(KModel** ppNewModel);

	HRESULT SaveSeclectedSubset2Mesh(BOOL* SelectedID,LPSTR pMeshFileName,LPSTR pMtlFileName,DWORD Option);//将选择中的ID保存成一个新的模型文件

	HRESULT ClearnAnimations();

	virtual HRESULT RenderWithEffect(DWORD Option,PVOID pData,D3DXMATRIX* pMatParent);
	LPMATERIAL GetFirstSelectedMtl();

	HRESULT Acquire();//暴露属性

	HRESULT DrawMeshSubset(LPD3DXMESH pMesh,LPMATERIAL pMaterial,DWORD SubsetID);
	HRESULT DrawSkinMeshSubset(LPD3DXMESH pMesh,DWORD MaterialID,DWORD AttribID,DWORD NumBoneBlend);
	HRESULT SortSkinSubset();

	///模体切割
	int   CullByPlane(KModel** ppmod_Top,KModel** ppmod_Bottom,D3DXPLANE* pPlane);
    ///三角剖分
	int TripTriangulation(const vector <EdgeVertex> &vec_EdgeVertex, int nNumEdgeVertex, WORD **ppwEdgeIndex, int *pnRetNumEdgeFace);
	

	HRESULT RestoreMatrix();
	HRESULT SetMatrix();
	HRESULT UpdataRelativeBoneMatrix();

	HRESULT UpdataRelativeBoneMatrix(int BoneIndex,D3DXMATRIX Matrix);
	HRESULT BoneBlendProcess();
	HRESULT LoadBoneBlendSetFromMeshFile(LPSTR pFileName);

	//////////////////////////////////////////////////////////////////////////
	//体积阴影相关函数
	HRESULT ShadowVolumeDraw();
	//绘制体积阴影，由ShadowVolumeRender调用。

	HRESULT ShadowVolumeRender();
	//渲染该网格的体积阴影，所有网格的本函数应该在DrawShadowVolumeMask被调用，
	//否则将看不到体积阴影

	HRESULT ShadowVolumeCreate();
	//创建默认的体积阴影

	virtual HRESULT FrameMove();
	//网格每桢的更新入口

	HRESULT Absolute_Move(D3DXVECTOR3* pPos);
	//在网格坐标系下，移动顶点位置
	
	virtual HRESULT PlaneShadowRender();
	//渲染平面阴影

	HRESULT DrawMeshNormal();
	//绘制网格的法向量

	HRESULT SetShader();
	//设置 SHADER
	HRESULT LoadShader();
	//测试 载入SHADER文件
	HRESULT DrawBBox();
	//绘制包围合

	//////////////////////////////////////////////////////////////////////////
	//材质相关函数
	
	
	HRESULT MoveTextureFile(LPSTR pDirName);
	//移动帖图文件，参数为目标目录名称，该函数自动创建该目录然后将材质中所有帖图和
	//SHADER文件移动到该目录下。同时会刷新材质中对应的信息
	
	HRESULT SaveOption(LPOPTION pOption,FILE* pFile); 
	//将材质选项保存到文件

	HRESULT LoadOption(LPOPTION pOption,FILE* pFile,LPMATERIAL pMtl); 
	//载入材质选项

	HRESULT SaveMaterial(LPSTR pFileName); 
	//保存材质到文件

	HRESULT RestoreMaterial(DWORD MaterialID); 
	//恢复设置过的材质，以保证绘制前绘制后的渲染状态相同。这里的渲染状态不包括帖图

	HRESULT SetOption(DWORD TextureStage,LPOPTION pOption);
	//设置材质选项

	HRESULT RestoreOption(DWORD TextureStage,LPOPTION pOption);
	//恢复设置过的材质选项

	HRESULT CheckUpMaterial();
	//检查并合并相同的材质

	HRESULT SaveAnimation(LPSTR pFileName);
	//保存动画文件

	HRESULT SaveMesh(LPSTR pFileName, BOOL bSaveBlendMesh = FALSE);
	//保存网格到文件

	HRESULT OptimizeUnSkinedMesh();
	//为没有使用骨骼系统的网格建立优化网格
	
	HRESULT CreateSkinBlendedMesh(BOOL bLoad);
	//建立骨骼蒙皮网格

	virtual HRESULT CleanUp();
	//清除数据

	//////////////////////////////////////////////////////////////////////////
	//Socket Functions
	virtual HRESULT UnBind();
	//取消绑定

	virtual HRESULT Bind(KModel* pXMesh,LPSTR SocketName);
	//绑定到另一网格的插槽上
	
	HRESULT DeleteSocket(LPCSTR SocketName);
	//删除插槽
	HRESULT DrawSocket();
	//绘制插槽
	HRESULT AddSocket(LPSTR pParentBoneName,LPSTR pSocket);
	//增加插槽

	//////////////////////////////////////////////////////////////////////////
	//Animation Fuctions
	HRESULT PlayAnimation(LPSTR AnimationName,DWORD PlayMethod,float Speed);
	HRESULT PlayAnimation(int AniIndex, DWORD PlayMethod, float Speed);
	//播放动画

	BOOL IsCurAnimationEnd();
	//检查当前动画是否播放完成

	virtual HRESULT UpdateAnimation();
	//根据动画数据更新网格

	HRESULT LoadAnimation(LPSTR pFileName);
	//从文件中载入动画数据

	int FindAnimation(LPCSTR pAnimationName);
	//查找动画
	
	HRESULT DeleteAnimation(LPSTR pAnimationName);

	int FindBone(LPCSTR pBoneName);
	//查找骨骼

	int FindSocket(LPCSTR pSocketName);
	//查找插槽
	
	virtual HRESULT LoadMaterial(LPSTR pFileName);
	//载入材质信息
	
	virtual HRESULT LoadMesh(LPSTR pFileName);
	//载入网格文件

	void Initialize(PVOID pXMeshTable);
	//初始化

	virtual    HRESULT      Render();
	//渲染

	virtual    HRESULT      LoadFromX(LPSTR pFileName);
	//从X文件中载入MESH

	virtual HRESULT SaveMesh(FILE* pFile, BOOL bSaveBlendMesh = FALSE);
	//保存网格到文件

	HRESULT TakeSnap(LPDIRECT3DTEXTURE9* ppTexture);
	//为网格照一张缩略图

	HRESULT DrawSnap(void);
	//将网格缩略图绘制到屏幕固定位置，主要用来检测

	HRESULT SaveSnap(LPSTR pFileName);
	//将网格缩略图保存为文件，目前为24位PNG文件

	KModel();
	virtual ~KModel();
	
	HRESULT SetMaterial(DWORD MaterialID);

	void    GetCurFrame(int* pnFrame,int* pnFrameTo,float* S);
	HRESULT DrawBones();
	HRESULT GenerateEnhancedMesh( UINT dwNewNumSegs,LPD3DXMESH pMesh );
protected:

	HRESULT DrawMeshSubset(LPD3DXMESH pMesh,DWORD SubsetID);
	HRESULT VertexProcessSkin();
	HRESULT DrawMesh(LPD3DXMESH pMesh);
	HRESULT SoftSkinning();
	HRESULT CreateSkinInfo(BOOL bLoad);

private:

	void    ShadowVolumeAddEdage(WORD* pTemp_Edages,DWORD* pNumEdages,WORD A,WORD B);

public:
	HRESULT InterpolateRTSBoneKeyFrame(LPBONEINFO& pInfo, RTS& FrameA, RTS& FrameB, float fWeight);
	HRESULT AnimateTo(LPANIMATION pFrom, LPANIMATION pTo, float fTimeSpan, float CurrentTime);
	BOOL    UpdateFlexibleBone(LPBONEINFO pInfo,int i,int i_Flex);
	BOOL    UpdateFlexibleBone0(LPBONEINFO pInfo,int i,int i_Flex);
	BOOL    UpdateFlexibleBone1(LPBONEINFO pInfo,int i,int i_Flex);
	BOOL    UpdateFlexibleBone2(LPBONEINFO pInfo,int i,int i_Flex);
	BOOL FlexCollsion(LPBONEINFO &pInfo);
	virtual HRESULT InvalidateDeviceObjects(void);
	virtual HRESULT RestoreDeviceObjects(void);
	HRESULT Save2XFile(LPSTR pFileName);
	HRESULT RenderShadow(void);

	HRESULT ModifySubsetID(DWORD* pSubsetID);
	HRESULT DrawNormal(void);
	void GetFaceNormal(D3DXVECTOR3* vec3StartPoint, D3DXVECTOR3* vec3Normal);

	HRESULT CreateProgressMesh(LPD3DXMESH pMesh);
	
	HRESULT SaveBoneScale(const TCHAR* strFileName);
	
	HRESULT LoadBoneScale(const TCHAR* strFileName);

	HRESULT DrawBoneTrace(const TCHAR* strBoneName);

	HRESULT GetTransformAtFrame(int nFrame, D3DXMATRIX* pMatrix);

	HRESULT UpdataRelativeBoneMatrix(int BoneIndex,D3DXMATRIX Matrix, D3DXMATRIX* pSrc, D3DXMATRIX* pDest);

	HRESULT InitializeTrace(TCHAR* strAnimationName);

	void ClearAnimationTracer();

	//////////////////////////////////////////////////////////////////////////
	//Sequence
	BOOL m_bPlaySequence;
	KSequence* m_pCurSeq;
	void PlaySequence( TCHAR* strSeqName );
//////////////////////////////////////////////////////////////////////////
//Add 5-23-2005 Bone Tracer
//Now we can render a bone's trace for a specified animation
//used to check animation
private:
	struct BoneScaleHeader
	{
		DWORD dwMagicNumber;
		DWORD dwNumBones;
	};
	struct BoneScaleData
	{
		TCHAR strBoneName[30];//Here we are sure bone's name is with 30 characters
		D3DXVECTOR3 vec3Scale;
	};

	typedef std::map<std::string, D3DXMATRIX**> AnimationTracer;
	typedef std::pair<std::string, D3DXMATRIX**> AnimationTracerData;
	typedef std::map<std::string, LPDIRECT3DVERTEXBUFFER9> BoneVertexBuffer;
	//typedef std::pair<std::string, LPDIRECT3DVERTEXBUFFER9*> BoneVertexBufferData;
	AnimationTracer m_AnimationTracer;
	BoneVertexBuffer m_BoneTracer;
	
	string m_strTracedBoneName;
public:
	BOOL m_bDrawBoneTrace;
	string GetTraceBoneName() { return m_strTracedBoneName;}
	void SetTraceBoneName(const string& strBoneName) { m_strTracedBoneName = strBoneName;}
//Add 5-23-2005 Bone Tracer end
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//Add 5-26-2005 Interpolation bug fix
//Note: An expensive way way to solve the problem of trembling bone interpolation
#ifdef KEY_FRAME_INTERPOLATION_BUG

private:
	D3DXMATRIX* m_pFrameA;
	D3DXMATRIX* m_pFrameB;
	int         m_nFrameA;
	int         m_nFrameB;
	int         m_nFrameAnimationIndex;
#endif
//Add 5-26-2005 Interpolation bug fix end
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//Add 5-28-2005 Break a big animation file into separater motion
public:
	HRESULT SaveIndividualAnimation();
	HRESULT FixVertexWeight();
//Add 5-28-2005 Break a big animation file into separater motion
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//Add 7-25-2005 Select Bone Start
	class KBoneIndicator
	{
	public:
		KBoneIndicator();
		~KBoneIndicator();
		HRESULT LinkToBone( KModel* pModel, const TCHAR* pBoneName );
		HRESULT DrawBoneIndicator(  );
	private:
		struct BoneIndicatorVertex
		{
			D3DXVECTOR3 vec3Pos;
			DWORD dwColor;
			const static DWORD FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
		};
		HRESULT InitBoneIndicator( );
		void UpdateBoneIndicator();
		void ClearBoneIndicator();
		BOOL m_bHide;
		KModel* m_pModel;
        int m_nBoneIndex;
		int m_nParentBoneIndex;
		float m_fBoneLength;
		LPDIRECT3DVERTEXBUFFER9 m_lpBoneIndicator;
		LPDIRECT3DINDEXBUFFER9 m_lpBoneIndicatorIB;
		const static float m_fWidth;
	};
	KBoneIndicator m_SelectedBone;
//Add 7-25-2005 Select Bone End
/////////////////////////////////////////////////////////////////////////	


}* LPMODEL;



//////////////////////////////////////////////////////////////////////////
//Add 7-19-2005 Animation Trace start
//Track的作用是用来做两个或者多个动画(Sequence)的部分骨骼选取播放的， Track就像是Sequence的容器,
//不同的Track的混合就是对不同骨骼的混合
const int MAX_MOTION_RESERVE  = 20;
typedef TCHAR* BONE_ID;
typedef TCHAR* ANIMATION_SEQU_ID;
typedef TCHAR* TRACK_IDENTIFIER;
class KMotion
{
public:
	BOOL TrackIDCmp( TRACK_IDENTIFIER ID1, TRACK_IDENTIFIER ID2 )
	{
		BOOL bReturn = FALSE;
		if ( !strcmp( ID1, ID2 ) )
			bReturn = TRUE;
		return bReturn;
	}
private:
	struct BoneWeightTableItem
	{
		BONE_ID BoneID;
		float Weight;//Maybe here could be like key frame thing
	};

	struct BoneWeightTable
	{
		DWORD dwNumBones;
	#ifndef CLIENT_EDITION
		list<BoneWeightTableItem*> BoneWeights;
	#else
		BoneWeightTableItem* BoneWeights;
	#endif
	};

	struct Track
	{
		TRACK_IDENTIFIER TrackID;
		ANIMATION_SEQU_ID SequenceID;
		BoneWeightTable BWTable;
		float fStartTime;
	};

	struct Header
	{
		TCHAR* m_strMotionName;
		int m_nNumTrack;
		DWORD dwSize;
		DWORD dwExtendBlock[MAX_MOTION_RESERVE];
	};

	TCHAR* m_strMotionName;
	int m_nNumTrack;
#ifndef CLIENT_EDTION
	list<Track*> m_Tracks;
#else
	Track* m_pTracks;
#endif
	float m_fStartTime;//Play motion start time;
	float m_fCurTime;//Current time;
	BOOL m_bPlay;//Playing state
public:
	HRESULT CheckBWTable(TCHAR** ppErrBones );//Check if the bone weight tables combination sum is 1.0f;
	HRESULT SaveMotion( const TCHAR* strMotionName );
	HRESULT LoadMotion( const TCHAR* strMotionName );
	void PlayMotion( float fSpeed, int nType );
	void Pause();
	void Stop();
//For Editor
	
	HRESULT AttachSequence( ANIMATION_SEQU_ID SeqID, BoneWeightTable* pBoneWeightTable, float fStartTime = 0.0f, BOOL bWarpInRange = FALSE );
	HRESULT WarpInRange( TRACK_IDENTIFIER TrackDest, TRACK_IDENTIFIER TrackSrc );
private:
	Track* FindTrack( TRACK_IDENTIFIER ID ); 
	HRESULT AddSequence( ANIMATION_SEQU_ID SeqID );

};
//Add 7-19-2005 Animation Trace end
//////////////////////////////////////////////////////////////////////////


#endif // !defined(AFX_XMESH_H__078CD29E_6320_426E_ABAE_C83A352F390F__INCLUDED_)
