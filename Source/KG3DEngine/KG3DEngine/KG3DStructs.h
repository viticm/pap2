
#ifndef _INCLUDE_KG3DSTRUCTS_H_
#define _INCLUDE_KG3DSTRUCTS_H_

////////////////////////////////////////////////////////////////////////////////
//这个文件属于接口的一部分（至少现在是），不要向里面加引擎里面的头文件
//在SceneEditor项目中被放到预编译头了，使用到的文件不要include它，让预编译头自己处理

/////////////////////////////////////////////////////
//structs
////////////////////////////////////////////////////
#include "GraphicsStruct.h"

#pragma message( "Compiling " __FILE__ ) 

namespace Misc
{

	typedef enum _Type_Condition
	{
		CONDITION_NONE             = 0,
		CONDITION_GET_MESSAGE      = 1,
		CONDITION_VARIABLE_COMPARE = 2,
		CONDITION_POSITION_INBOX   = 10,
		CONDITION_KEYDOWN          = 20,
	}TYPE_CONDITION;

#define TYPE_VARCOMPARE_CONST_EQUAL      1
#define TYPE_VARCOMPARE_CONST_UNEQUAL    2
#define TYPE_VARCOMPARE_CONST_LESS       3
#define TYPE_VARCOMPARE_CONST_LESSEQUAL  4
#define TYPE_VARCOMPARE_CONST_GREAT      5
#define TYPE_VARCOMPARE_CONST_GREATEQUAL 6

#define TYPE_VARCOMPARE_EQUAL      11
#define TYPE_VARCOMPARE_UNEQUAL    12
#define TYPE_VARCOMPARE_LESS       13
#define TYPE_VARCOMPARE_LESSEQUAL  14
#define TYPE_VARCOMPARE_GREAT      15
#define TYPE_VARCOMPARE_GREATEQUAL 16

#define TYPE_VAR_CONST_SET            1
#define TYPE_VAR_CONST_ADD            2
#define TYPE_VAR_CONST_SUBTRACT       3
#define TYPE_VAR_CONST_BSUBTRACT      4
#define TYPE_VAR_CONST_MULTIPLY       5
#define TYPE_VAR_CONST_DIVISION       6
#define TYPE_VAR_CONST_BDIVISION      7
#define TYPE_VAR_CONST_SET_PART       8

#define TYPE_VAR_SET                  10
#define TYPE_VAR_ADD                  11
#define TYPE_VAR_SUBTRACT             12
#define TYPE_VAR_MULTIPLY             13
#define TYPE_VAR_DIVISION             14

#define TYPE_VAR_Get_PART             20
#define TYPE_VAR_Set_PART             21
	///////////////////////////////////////////////////////////////////////////////////////////
	//type of variable operator for color
#define TYPE_VAR_ColorAdd                30
#define TYPE_VAR_ColorAdjustContrast     31
#define TYPE_VAR_ColorAdjustSaturation   32
#define TYPE_VAR_ColorLerp               33
#define TYPE_VAR_ColorModulate           34
#define TYPE_VAR_ColorNegative           35
#define TYPE_VAR_ColorScale              36
#define TYPE_VAR_ColorSubtract           37

#define TYPE_VAR_ColorGet_Red             300
#define TYPE_VAR_ColorGet_Blue            301
#define TYPE_VAR_ColorGet_Green           302
#define TYPE_VAR_ColorSet_Red             303
#define TYPE_VAR_ColorSet_Blue            304
#define TYPE_VAR_ColorSet_Green           305
	///////////////////////////////////////////////////////////////////////////////////////////
	//type of variable operator for plane
#define TYPE_VAR_PlaneDot                40
#define TYPE_VAR_PlaneDotCoord           41
#define TYPE_VAR_PlaneDotNormal          42
#define TYPE_VAR_PlaneIntersectLine      43
#define TYPE_VAR_PlaneFromPointNormal    44
#define TYPE_VAR_PlaneNormalize          45
#define TYPE_VAR_PlaneFromPoints         46
#define TYPE_VAR_PlaneTransform          47

#define TYPE_VAR_PlaneGet_A               310
#define TYPE_VAR_PlaneGet_B               311
#define TYPE_VAR_PlaneGet_C               312
#define TYPE_VAR_PlaneGet_D               313
#define TYPE_VAR_PlaneSet_A               314
#define TYPE_VAR_PlaneSet_B               315
#define TYPE_VAR_PlaneSet_C               316
#define TYPE_VAR_PlaneSet_D               317
	///////////////////////////////////////////////////////////////////////////////////////////
	//type of variable operator for Quaternion
#define TYPE_VAR_QuaternionBaryCentric                 50
#define TYPE_VAR_QuaternionConjugate                   51
#define TYPE_VAR_QuaternionDot                         52
#define TYPE_VAR_QuaternionExp                         53
#define TYPE_VAR_QuaternionIdentity                    54
#define TYPE_VAR_QuaternionInverse                     55
#define TYPE_VAR_QuaternionIsIdentity                  56
#define TYPE_VAR_QuaternionLength                      57
#define TYPE_VAR_QuaternionLengthSq                    58
#define TYPE_VAR_QuaternionLn                          59
#define TYPE_VAR_QuaternionMultiply                    60
#define TYPE_VAR_QuaternionNormalize                   61
#define TYPE_VAR_QuaternionRotationAxis                62
#define TYPE_VAR_QuaternionRotationMatrix              63
#define TYPE_VAR_QuaternionRotationYawPitchRoll        64
#define TYPE_VAR_QuaternionSlerp                       65
#define TYPE_VAR_QuaternionSquad                       66
#define TYPE_VAR_QuaternionToAxisAngle                 67

#define TYPE_VAR_QuaternionGet_X               320
#define TYPE_VAR_QuaternionGet_Y               321
#define TYPE_VAR_QuaternionGet_Z               322
#define TYPE_VAR_QuaternionGet_W               323
#define TYPE_VAR_QuaternionSet_X               324
#define TYPE_VAR_QuaternionSet_Y               325
#define TYPE_VAR_QuaternionSet_Z               326
#define TYPE_VAR_QuaternionSet_W               327
	///////////////////////////////////////////////////////////////////////////////////////////
	//type of variable operator for Vector2d
#define TYPE_VAR_Vec2Add              70
#define TYPE_VAR_Vec2BaryCentric      71
#define TYPE_VAR_Vec2CatmullRom       72
#define TYPE_VAR_Vec2CCW              73
#define TYPE_VAR_Vec2Dot              74
#define TYPE_VAR_Vec2Hermite          75
#define TYPE_VAR_Vec2Length           76
#define TYPE_VAR_Vec2LengthSq         77
#define TYPE_VAR_Vec2Lerp             78
#define TYPE_VAR_Vec2Maximize         79
#define TYPE_VAR_Vec2Minimize         80
#define TYPE_VAR_Vec2Normalize        81
#define TYPE_VAR_Vec2Scale            82
#define TYPE_VAR_Vec2Subtract         83
#define TYPE_VAR_Vec2Transform        84
#define TYPE_VAR_Vec2TransformCoord   85
#define TYPE_VAR_Vec2TransformNormal  86

#define TYPE_VAR_Vec2Get_X               330
#define TYPE_VAR_Vec2Get_Y               331
#define TYPE_VAR_Vec2Set_X               332
#define TYPE_VAR_Vec2Set_Y               333
	///////////////////////////////////////////////////////////////////////////////////////////
	//type of variable operator for Vector3d
#define TYPE_VAR_Vec3Add              90
#define TYPE_VAR_Vec3BaryCentric      91
#define TYPE_VAR_Vec3CatmullRom       92
#define TYPE_VAR_Vec3Cross            93
#define TYPE_VAR_Vec3Dot              94
#define TYPE_VAR_Vec3Hermite          95
#define TYPE_VAR_Vec3Length           96
#define TYPE_VAR_Vec3LengthSq         97
#define TYPE_VAR_Vec3Lerp             98
#define TYPE_VAR_Vec3Maximize         99
#define TYPE_VAR_Vec3Minimize         100
#define TYPE_VAR_Vec3Normalize        101
#define TYPE_VAR_Vec3Project          102
#define TYPE_VAR_Vec3Scale            103
#define TYPE_VAR_Vec3Subtract         104
#define TYPE_VAR_Vec3Transform        105
#define TYPE_VAR_Vec3TransformCoord   106
#define TYPE_VAR_Vec3TransformNormal  107
#define TYPE_VAR_Vec3Unproject        108

#define TYPE_VAR_Vec3Get_X            340
#define TYPE_VAR_Vec3Get_Y            341
#define TYPE_VAR_Vec3Get_Z            342
#define TYPE_VAR_Vec3Set_X            343
#define TYPE_VAR_Vec3Set_Y            344
#define TYPE_VAR_Vec3Set_Z            345
	///////////////////////////////////////////////////////////////////////////////////////////
	//type of variable operator for matrix
#define TYPE_VAR_MatrixAffineTransformation     110
#define TYPE_VAR_MatrixfDeterminant             111
#define TYPE_VAR_MatrixIdentity                 112
#define TYPE_VAR_MatrixInverse                  113
#define TYPE_VAR_MatrixIsIdentity               114
#define TYPE_VAR_MatrixLookAtRH                 115
#define TYPE_VAR_MatrixLookAtLH                 116
#define TYPE_VAR_MatrixMultiply                 117
#define TYPE_VAR_MatrixOrthoRH                  118
#define TYPE_VAR_MatrixOrthoLH                  119
#define TYPE_VAR_MatrixOrthoOffCenterRH         120
#define TYPE_VAR_MatrixOrthoOffCenterLH         121
#define TYPE_VAR_MatrixPerspectiveRH            122
#define TYPE_VAR_MatrixPerspectiveLH            123
#define TYPE_VAR_MatrixPerspectiveFovRH         124
#define TYPE_VAR_MatrixPerspectiveFovLH         125
#define TYPE_VAR_MatrixPerspectiveOffCenterRH   126
#define TYPE_VAR_MatrixPerspectiveOffCenterLH   127
#define TYPE_VAR_MatrixReflect                  128
#define TYPE_VAR_MatrixRotationAxis             129
#define TYPE_VAR_MatrixRotationQuaternion       130
#define TYPE_VAR_MatrixRotationX                131
#define TYPE_VAR_MatrixRotationY                132
#define TYPE_VAR_MatrixRotationYawPitchRoll     133
#define TYPE_VAR_MatrixRotationZ                134
#define TYPE_VAR_MatrixScaling                  135
#define TYPE_VAR_MatrixShadow                   136
#define TYPE_VAR_MatrixTransformation           137
#define TYPE_VAR_MatrixTranslation              138
#define TYPE_VAR_MatrixTranspose                139

#define TYPE_VAR_MatrixGet_11            350
#define TYPE_VAR_MatrixGet_12            351
#define TYPE_VAR_MatrixGet_13            352
#define TYPE_VAR_MatrixGet_14            353
#define TYPE_VAR_MatrixGet_21            354
#define TYPE_VAR_MatrixGet_22            355
#define TYPE_VAR_MatrixGet_23            356
#define TYPE_VAR_MatrixGet_24            357
#define TYPE_VAR_MatrixGet_31            358
#define TYPE_VAR_MatrixGet_32            359
#define TYPE_VAR_MatrixGet_33            360
#define TYPE_VAR_MatrixGet_34            361
#define TYPE_VAR_MatrixGet_41            362
#define TYPE_VAR_MatrixGet_42            363
#define TYPE_VAR_MatrixGet_43            364
#define TYPE_VAR_MatrixGet_44            365

#define TYPE_VAR_MatrixSet_11            366
#define TYPE_VAR_MatrixSet_12            367
#define TYPE_VAR_MatrixSet_13            368
#define TYPE_VAR_MatrixSet_14            369
#define TYPE_VAR_MatrixSet_21            370
#define TYPE_VAR_MatrixSet_22            371
#define TYPE_VAR_MatrixSet_23            372
#define TYPE_VAR_MatrixSet_24            373
#define TYPE_VAR_MatrixSet_31            374
#define TYPE_VAR_MatrixSet_32            375
#define TYPE_VAR_MatrixSet_33            376
#define TYPE_VAR_MatrixSet_34            377
#define TYPE_VAR_MatrixSet_41            378
#define TYPE_VAR_MatrixSet_42            379
#define TYPE_VAR_MatrixSet_43            380
#define TYPE_VAR_MatrixSet_44            381
	///////////////////////////////////////////////////////////////////////////////////////////
	//type of variable operator for vector4d
#define TYPE_VAR_Vec4Add              150
#define TYPE_VAR_Vec4BaryCentric      151
#define TYPE_VAR_Vec4CatmullRom       152
#define TYPE_VAR_Vec4Cross            153
#define TYPE_VAR_Vec4Dot              154
#define TYPE_VAR_Vec4Hermite          155
#define TYPE_VAR_Vec4Length           156
#define TYPE_VAR_Vec4LengthSq         157
#define TYPE_VAR_Vec4Lerp             158
#define TYPE_VAR_Vec4Maximize         159
#define TYPE_VAR_Vec4Minimize         160
#define TYPE_VAR_Vec4Normalize        161
#define TYPE_VAR_Vec4Scale            162
#define TYPE_VAR_Vec4Subtract         163
#define TYPE_VAR_Vec4Transform        164

#define TYPE_VAR_Vec4Get_X            390
#define TYPE_VAR_Vec4Get_Y            391
#define TYPE_VAR_Vec4Get_Z            392
#define TYPE_VAR_Vec4Get_W            393
#define TYPE_VAR_Vec4Set_X            394
#define TYPE_VAR_Vec4Set_Y            395
#define TYPE_VAR_Vec4Set_Z            396
#define TYPE_VAR_Vec4Set_W            397
	//////////////////////////////////////////////////////////////////////////////////////
	//TYPE OF TRANSFORMATION VARIABLES
#define TYPE_TRANSFORMATION_MODE                1
#define TYPE_TRANSFORMATION_TRANSLATION         2
#define TYPE_TRANSFORMATION_SCALINGROTATION     3
#define TYPE_TRANSFORMATION_SCALINGCENTER       4
#define TYPE_TRANSFORMATION_SCALING             5
#define TYPE_TRANSFORMATION_ROTATIONQUATERTION  6
#define TYPE_TRANSFORMATION_ROTATIONCENTER      7
#define TYPE_TRANSFORMATION_YAWPITCHROLL        8
#define TYPE_TRANSFORMATION_YPRSPEED            9
#define TYPE_TRANSFORMATION_YPRACCELERATE      10
#define TYPE_TRANSFORMATION_POSITION           11
#define TYPE_TRANSFORMATION_SPEED              12
#define TYPE_TRANSFORMATION_ACCELERATE         13
#define TYPE_TRANSFORMATION_CENTER             14
#define TYPE_TRANSFORMATION_MATRIX             15
#define TYPE_TRANSFORMATION_DEFFRONT           16
#define TYPE_TRANSFORMATION_DEFUP              17
#define TYPE_TRANSFORMATION_DEFLEFT            18
#define TYPE_TRANSFORMATION_DIRECTIONFRONT     19
#define TYPE_TRANSFORMATION_DIRECTIONUP        20
#define TYPE_TRANSFORMATION_DIRECTIONLEFT      21
#define TYPE_TRANSFORMATION_DIRECTIONYPR       22
#define TYPE_TRANSFORMATION_DIRECTIONYPRSPEED  23
#define TYPE_TRANSFORMATION_DIRECTIONYPRACCELERATE      24
#define TYPE_TRANSFORMATION_POSITIONFSU        25
#define TYPE_TRANSFORMATION_SPEEDFSU           26
#define TYPE_TRANSFORMATION_ACCELERATEFSU      27

#define TYPE_TRANSFORMATION_MODE_VAR                31
#define TYPE_TRANSFORMATION_TRANSLATION_VAR         32
#define TYPE_TRANSFORMATION_SCALINGROTATION_VAR     33
#define TYPE_TRANSFORMATION_SCALINGCENTER_VAR       34
#define TYPE_TRANSFORMATION_SCALING_VAR             35
#define TYPE_TRANSFORMATION_ROTATIONQUATERTION_VAR  36
#define TYPE_TRANSFORMATION_ROTATIONCENTER_VAR      37
#define TYPE_TRANSFORMATION_YAWPITCHROLL_VAR        38
#define TYPE_TRANSFORMATION_YPRSPEED_VAR            39
#define TYPE_TRANSFORMATION_YPRACCELERATE_VAR       40
#define TYPE_TRANSFORMATION_POSITION_VAR            41
#define TYPE_TRANSFORMATION_SPEED_VAR               42
#define TYPE_TRANSFORMATION_ACCELERATE_VAR          43
#define TYPE_TRANSFORMATION_CENTER_VAR              44
#define TYPE_TRANSFORMATION_MATRIX_VAR              45
#define TYPE_TRANSFORMATION_DEFFRONT_VAR            46
#define TYPE_TRANSFORMATION_DEFUP_VAR               47
#define TYPE_TRANSFORMATION_DEFLEFT_VAR             48
#define TYPE_TRANSFORMATION_DIRECTIONFRONT_VAR      49
#define TYPE_TRANSFORMATION_DIRECTIONUP_VAR         50
#define TYPE_TRANSFORMATION_DIRECTIONLEFT_VAR       51
#define TYPE_TRANSFORMATION_DIRECTIONYPR_VAR        52
#define TYPE_TRANSFORMATION_DIRECTIONYPRSPEED_VAR   53
#define TYPE_TRANSFORMATION_DIRECTIONYPRACCELERATE_VAR      54
#define TYPE_TRANSFORMATION_POSITIONFSU_VAR         55
#define TYPE_TRANSFORMATION_SPEEDFSU_VAR            56
#define TYPE_TRANSFORMATION_ACCELERATEFSU_VAR       57


//////////////////////////////////////////////////////////////////////////
#define BOX_A1 0
#define BOX_B1 1
#define BOX_C1 2
#define BOX_D1 3
#define BOX_A2 4
#define BOX_B2 5
#define BOX_C2 6
#define BOX_D2 7
//////////////////////////////////////////////////////////////////////////

	typedef enum _Type_Action
	{
		ACTION_NONE         = 0,
		ACTION_POST_MESSAGE = 1,
		ACTION_SET_VARIABLE = 2,
		ACTION_SET_TRANSFORMATION = 3,

		ACTION_GET_TRANSFORMATION = 13,
	}TYPE_ACTION;

	typedef enum _BoundType
	{
		TYPE_BOUND_POINT         = 1,
		TYPE_BOUND_LINE          = 2,
		TYPE_BOUND_RADIAL        = 3,
		TYPE_BOUND_BEELINE       = 4,
		TYPE_BOUND_TRIANGLE      = 5,
		TYPE_BOUND_PARALLELOGRAM = 6,
		TYPE_BOUND_PLANE         = 7,
		TYPE_BOUND_BOX           = 8,
		TYPE_BOUND_SPHERE        = 9,
		TYPE_BOUND_COLUMN        = 10,
	}BOUNDTYPE;

	typedef enum _ObjectType
	{
		TYPE_NONE        = 0,
		TYPE_OBJECT      = 1,
		TYPE_LAYER       = 2,
		TYPE_MESH        = 3,
		TYPE_FACES       = 4,
		TYPE_BoundingBox = 5,
		TYPE_BONE        = 6,
		TYPE_BONEPOINT   = 7,
		TYPE_SELECTEDBOX = 8,
		TYPE_TEXTURE     = 9,
		TYPE_XMESH       = 10,

		TYPE_SINGLEPOINT  = 21,
		TYPE_SINGLELINE   = 22,
		TYPE_SINGLERECT   = 23,
		TYPE_SINGLECIRCLE = 24,

		TYPE_PICKLINE     = 31,
		TYPE_TEXT2D       = 32,
		TYPE_TEXT3D       = 33,
		TYPE_MENU2D       = 34,
		TYPE_MENU3D       = 35,

		TYPE_MATERAL      = 40,
		TYPE_FACESTABLE   = 41,
		TYPE_MESHTABLE    = 42,
		TYPE_MATERALTABLE = 43,
		TYPE_TEXTURETABLE = 44,

	}OBJECTTYPE;

	typedef enum _VariableType
	{
		VARIABLE_NONE         = 0,
		VARIABLE_INTEGER      = 1,
		VARIABLE_WORD         = 2,
		VARIABLE_DWORD        = 4,
		VARIABLE_STRING       = 5,
		VARIABLE_PVOID        = 6,
		VARIABLE_FLOAT        = 7,
		VARIABLE_DOUBLE       = 8,

		VARIABLE_D3DXCOLOR    = 10,
		VARIABLE_D3DMATERIAL9 = 11,
		VARIABLE_D3DXMATRIX   = 12,
		VARIABLE_D3DXPLANE    = 13,
		VARIABLE_D3DXQUATERNION = 14,
		VARIABLE_D3DXVECTOR2    = 15,
		VARIABLE_D3DXVECTOR3    = 16,
		VARIABLE_D3DXVECTOR4    = 17,
		VARIABLE_BOUNDINGBOX    = 18,
		VARIABLE_BOUNDINGSPHERE = 19,

	}VARIABLETYPE;

	typedef enum _TextureType
	{
		TEXTURE_NORMAL    =  1,
		TEXTURE_MOVIES    =  2,
		TEXTURE_VEVTOR    =  3,
		TEXTURE_ANI       =  4,
		TEXTURE_1         =  5,
		TEXTURE_2         =  6,
	}TEXTURETYPE;

	typedef enum _MouseStates
	{
		MOUSE_CAMERAMOVE      =  1,
		MOUSE_CAMERAROT       =  2,
		MOUSE_FACESMOVE       =  3,
		MOUSE_SECLECTTRIANGLE =  4,
		MOUSE_AJUSTRTEXTURECA =  10,
		MOUSE_AJUSTRTEXTURECB =  11,
		MOUSE_AJUSTRTEXTURECC =  12,
	}MOUSESATES;

	typedef enum   _MessageConditionType
	{
		MSGCONDITION_NONE         = 0,
		MSGCONDITION_KEYDOWN      = 1,
		MSGCONDITION_KEYUP        = 2,
		MSGCONDITION_KEYSET       = 3,

		MSGCONDITION_MOUSEMOVE    = 11,
		MSGCONDITION_MOUSEWHEEL   = 12,
		MSGCONDITION_MOUSEKEYDOWN = 13,
		MSGCONDITION_MOUSEKEYUP   = 14,
	}MessageConditionType;

	typedef enum   _ActionType
	{
		//ACTION_NONE           = 0,
		ACTION_MODIFYVARIABLE = 1,
		ACTION_CALLFUNCTION   = 2,
	}ActionType;

	typedef enum   _DataTableType
	{
		DATATABLE_TEXT2D  = 0,
		DATATABLE_TEXT3D  = 1,
		DATATABLE_FACES   = 2,
		DATATABLE_MESHES  = 3,
		DATATABLE_LAYERS  = 4,
		DATATABLE_OBJECTS = 5,
		DATATABLE_TEXTURES = 6,
	}DataTableType;

	typedef enum _LoadListItemType
	{
		FILE_NONE     = 0,
		FILE_XMESH    = 1,
		FILE_TEXTURE  = 2,
		FILE_SOUND    = 3,
		FILE_MUSIC    = 4,
		FILE_MOVIE    = 5,
		FILE_MESSAGE  = 6,
		FILE_LOADLIST = 7,
		FILE_NFACESMESH = 8,
		FILE_NFACES    =9,
	}LOADLISTITEMTYPE;

	typedef struct _MessagePath
	{
		DWORD ID_Package;
		DWORD ID_Group;
		DWORD ID_Message;
		DWORD ID_CA;
		BOOL  ID_IsCondition;
	}MESSAGEPATH;

	typedef struct   _FacesDataType
	{
		DWORD    FVF;
		DWORD    FVFLength;
		DWORD    DataStructure;
		DWORD    FacesType;
	}FacesDateType;

};



namespace VFormat
{
	struct FONT2DVERTEX { D3DXVECTOR4 p;   DWORD color;     FLOAT tu, tv; };
	struct FONT3DVERTEX { D3DXVECTOR3 p;   D3DXVECTOR3 n; DWORD color;   FLOAT tu, tv; };

#define D3DFVF_FONT2DVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)
#define D3DFVF_FONT3DVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_NORMAL|D3DFVF_TEX1)
#define D3DFVF_POSITION_NORMAL_TEX1 (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)

	typedef struct _Faces_Normal_Texture0
	{
		D3DXVECTOR3 p;
		D3DXVECTOR3 Normal;

	}FACES_NORMAL_TEXTURE0;

	struct NormalDrawer
	{
		D3DXVECTOR3 p;
		DWORD dwColor;
		const static DWORD m_dwFVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
	};

	typedef struct _Faces_Normal_Texture1 
	{ 
		D3DXVECTOR3 p;
		D3DXVECTOR3 Normal;
		float tu1, tv1;
	}FACES_NORMAL_TEXTURE1;

	typedef struct _Faces_Normal_Texture2 
	{ 
		D3DXVECTOR3 p;
		D3DXVECTOR3 Normal;
		FLOAT tu1, tv1;
		FLOAT tu2, tv2;
	}FACES_NORMAL_TEXTURE2;

	typedef struct _Faces_Normal_Texture3 
	{ 
		D3DXVECTOR3 p;
		D3DXVECTOR3 Normal;
		FLOAT tu1, tv1;
		FLOAT tu2, tv2;
		FLOAT tu3, tv3;
	}FACES_NORMAL_TEXTURE3;

	typedef struct _Faces_Normal_Texture4
	{ 
		D3DXVECTOR3 p;
		D3DXVECTOR3 Normal;
		FLOAT tu1, tv1;
		FLOAT tu2, tv2;
		FLOAT tu3, tv3;
		FLOAT tu4, tv4;

	}FACES_NORMAL_TEXTURE4;

	typedef struct _Faces_Normal_Texture5
	{ 
		D3DXVECTOR3 p;
		D3DXVECTOR3 Normal;
		FLOAT tu1, tv1;
		FLOAT tu2, tv2;
		FLOAT tu3, tv3;
		FLOAT tu4, tv4;
		FLOAT tu5, tv5;

	}FACES_NORMAL_TEXTURE5;

	typedef struct _Faces_Normal_Texture6
	{ 
		D3DXVECTOR3 p;
		D3DXVECTOR3 Normal;
		FLOAT tu1, tv1;
		FLOAT tu2, tv2;
		FLOAT tu3, tv3;
		FLOAT tu4, tv4;
		FLOAT tu5, tv5;
		FLOAT tu6, tv6;

	}FACES_NORMAL_TEXTURE6;

	typedef struct _Faces_Normal_Texture7
	{ 
		D3DXVECTOR3 p;
		D3DXVECTOR3 Normal;
		FLOAT tu1, tv1;
		FLOAT tu2, tv2;
		FLOAT tu3, tv3;
		FLOAT tu4, tv4;
		FLOAT tu5, tv5;
		FLOAT tu6, tv6;
		FLOAT tu7, tv7;

	}FACES_NORMAL_TEXTURE7;
	//////////////////////////////////////////////////////////////////////////
	//
	typedef struct _Faces_Normal_Diffuse_Texture0
	{
		D3DXVECTOR3 p;
		D3DXVECTOR3 Normal;
		D3DCOLOR    diffuse;
	}FACES_NORMAL_DIFFUSE_TEXTURE0;


	typedef struct _Faces_Normal_Diffuse_Texture1 
	{ 
		D3DXVECTOR3 p;
		D3DXVECTOR3 Normal;
		D3DCOLOR    diffuse;
		FLOAT tu1, tv1;
	}FACES_NORMAL_DIFFUSE_TEXTURE1;

	typedef struct _Faces_Normal_Diffuse_Texture2 
	{ 
		D3DXVECTOR3 p;
		D3DXVECTOR3 Normal;
		D3DCOLOR    diffuse;
		FLOAT tu1, tv1;
		FLOAT tu2, tv2;
	}FACES_NORMAL_DIFFUSE_TEXTURE2;

	typedef struct _Faces_Normal_Texture2_Float3
	{ 
		D3DXVECTOR3 p;
		D3DXVECTOR3 Normal;
		//    D3DCOLOR    diffuse;
		D3DXVECTOR2 TexC1;
		D3DXVECTOR2 TexC2;
		D3DXVECTOR3 vec3;
		D3DXVECTOR3 vec32;
	}FACES_NORMAL_TEXTURE2_FLOAT3;

	typedef struct _Faces_Normal_Diffuse_Texture3 
	{ 
		D3DXVECTOR3 p;
		D3DXVECTOR3 Normal;
		D3DCOLOR    diffuse;
		FLOAT tu1, tv1;
		FLOAT tu2, tv2;
		FLOAT tu3, tv3;
	}FACES_NORMAL_DIFFUSE_TEXTURE3;
	//////////////////////////////////////////////////////////////////////////////////////////////////
	//
	typedef struct _Faces_Texture0
	{
		D3DXVECTOR3 p;
	}FACES_TEXTURE0;

	typedef struct _Faces_Texture1 
	{ 
		D3DXVECTOR3 p;
		FLOAT tu1, tv1;
	}FACES_TEXTURE1;

	typedef struct _Faces_Texture2 
	{ 
		D3DXVECTOR3 p;
		FLOAT tu1, tv1;
		FLOAT tu2, tv2;
	}FACES_TEXTURE2;

	typedef struct _Faces_Texture3 
	{ 
		D3DXVECTOR3 p;
		FLOAT tu1, tv1;
		FLOAT tu2, tv2;
		FLOAT tu3, tv3;
	}FACES_TEXTURE3;

	typedef struct _Texture1
	{
		float tu1,tv1;
	}TEXTURE1;

	typedef struct _Faces_Texture4
	{ 
		D3DXVECTOR3 p;
		FLOAT tu1, tv1;
		FLOAT tu2, tv2;
		FLOAT tu3, tv3;
		FLOAT tu4, tv4;

	}FACES_TEXTURE4;

	typedef struct _Faces_Texture5
	{ 
		D3DXVECTOR3 p;
		FLOAT tu1, tv1;
		FLOAT tu2, tv2;
		FLOAT tu3, tv3;
		FLOAT tu4, tv4;
		FLOAT tu5, tv5;

	}FACES_TEXTURE5;

	typedef struct _Faces_Texture6
	{ 
		D3DXVECTOR3 p;
		FLOAT tu1, tv1;
		FLOAT tu2, tv2;
		FLOAT tu3, tv3;
		FLOAT tu4, tv4;
		FLOAT tu5, tv5;
		FLOAT tu6, tv6;

	}FACES_TEXTURE6;

	typedef struct _Faces_Texture7
	{ 
		D3DXVECTOR3 p;
		FLOAT tu1, tv1;
		FLOAT tu2, tv2;
		FLOAT tu3, tv3;
		FLOAT tu4, tv4;
		FLOAT tu5, tv5;
		FLOAT tu6, tv6;
		FLOAT tu7, tv7;

	}FACES_TEXTURE7;


	//////////////////////////////////////////////////////////////////////////////////////////////////
	//
	typedef struct _Faces_Diffuse_Texture0
	{
		D3DXVECTOR3 p;
		D3DCOLOR    diffuse;
	}FACES_DIFFUSE_TEXTURE0;


	typedef struct _Faces_Diffuse_Texture1 
	{ 
		D3DXVECTOR3 p;
		D3DCOLOR    diffuse;
		FLOAT tu1, tv1;
	}FACES_DIFFUSE_TEXTURE1;

	typedef struct _Faces_Diffuse_Texture2 
	{ 
		D3DXVECTOR3 p;
		D3DCOLOR    diffuse;
		FLOAT tu1, tv1;
		FLOAT tu2, tv2;
	}FACES_DIFFUSE_TEXTURE2;

	typedef struct _Faces_Diffuse_Texture3 
	{ 
		D3DXVECTOR3 p;
		D3DCOLOR    diffuse;
		FLOAT tu1, tv1;
		FLOAT tu2, tv2;
		FLOAT tu3, tv3;
	}FACES_DIFFUSE_TEXTURE3;

	typedef struct _Faces_Diffuse_Texture4
	{ 
		D3DXVECTOR3 p;
		D3DCOLOR    diffuse;
		FLOAT tu1, tv1;
		FLOAT tu2, tv2;
		FLOAT tu3, tv3;
		FLOAT tu4, tv4;

	}FACES_DIFFUSE_TEXTURE4;

	typedef struct _Faces_Diffuse_Texture5
	{ 
		D3DXVECTOR3 p;
		D3DCOLOR    diffuse;
		FLOAT tu1, tv1;
		FLOAT tu2, tv2;
		FLOAT tu3, tv3;
		FLOAT tu4, tv4;
		FLOAT tu5, tv5;

	}FACES_DIFFUSE_TEXTURE5;

	typedef struct _Faces_Diffuse_Texture6
	{ 
		D3DXVECTOR3 p;
		D3DCOLOR    diffuse;
		FLOAT tu1, tv1;
		FLOAT tu2, tv2;
		FLOAT tu3, tv3;
		FLOAT tu4, tv4;
		FLOAT tu5, tv5;
		FLOAT tu6, tv6;

	}FACES_DIFFUSE_TEXTURE6;

	typedef struct _Faces_Diffuse_Texture7
	{ 
		D3DXVECTOR3 p;
		D3DCOLOR    diffuse;
		FLOAT tu1, tv1;
		FLOAT tu2, tv2;
		FLOAT tu3, tv3;
		FLOAT tu4, tv4;
		FLOAT tu5, tv5;
		FLOAT tu6, tv6;
		FLOAT tu7, tv7;

	}FACES_DIFFUSE_TEXTURE7;


	struct CUSTOMVERTEXL
	{
		D3DXVECTOR3 position; // The 3D position for the vertex
		D3DXVECTOR3 normal;   // The surface normal for the vertex
	};
	struct CUSTOMVERTEX
	{
		D3DXVECTOR3 position; // The position
		D3DCOLOR    color;    // The color
		FLOAT       tu, tv;   // The texture coordinates

	};
	struct _Tangent_Binormal
	{
		D3DXVECTOR3 tangent;
		D3DXVECTOR3 binormal;
	};

#define D3DFVF_CUSTOMVERTEXL (D3DFVF_XYZ|D3DFVF_NORMAL)


#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1 )

#define D3DFVF_Faces_Normal_Texture0      (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX0)
#define D3DFVF_Faces_Normal_Texture1      (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)
#define D3DFVF_Faces_Normal_Texture2      (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX2)
#define D3DFVF_Faces_Normal_Texture3      (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX3)
#define D3DFVF_Faces_Normal_Texture4      (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX4)
#define D3DFVF_Faces_Normal_Texture5      (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX5)
#define D3DFVF_Faces_Normal_Texture6      (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX6)
#define D3DFVF_Faces_Normal_Texture7      (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX7)

#define D3DFVF_Faces_Texture0             (D3DFVF_XYZ|D3DFVF_TEX0)
#define D3DFVF_Faces_Texture1             (D3DFVF_XYZ|D3DFVF_TEX1)
#define D3DFVF_Faces_Texture2             (D3DFVF_XYZ|D3DFVF_TEX2)
#define D3DFVF_Faces_Texture3             (D3DFVF_XYZ|D3DFVF_TEX3)
#define D3DFVF_Faces_Texture4             (D3DFVF_XYZ|D3DFVF_TEX4)
#define D3DFVF_Faces_Texture5             (D3DFVF_XYZ|D3DFVF_TEX5)
#define D3DFVF_Faces_Texture6             (D3DFVF_XYZ|D3DFVF_TEX6)
#define D3DFVF_Faces_Texture7             (D3DFVF_XYZ|D3DFVF_TEX7)

#define D3DFVF_Faces_Diffuse_Texture0      (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX0)
#define D3DFVF_Faces_Diffuse_Texture1      (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)
#define D3DFVF_Faces_Diffuse_Texture2      (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX2)
#define D3DFVF_Faces_Diffuse_Texture3      (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX3)
#define D3DFVF_Faces_Diffuse_Texture4      (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX4)
#define D3DFVF_Faces_Diffuse_Texture5      (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX5)
#define D3DFVF_Faces_Diffuse_Texture6      (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX6)
#define D3DFVF_Faces_Diffuse_Texture7      (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX7)

#define D3DFVF_LEN_Faces_Normal_Texture0    (sizeof(_Faces_Normal_Texture0))
#define D3DFVF_LEN_Faces_Normal_Texture1    (sizeof(_Faces_Normal_Texture1))
#define D3DFVF_LEN_Faces_Normal_Texture2    (sizeof(_Faces_Normal_Texture2))
#define D3DFVF_LEN_Faces_Normal_Texture3    (sizeof(_Faces_Normal_Texture3))
#define D3DFVF_LEN_Faces_Normal_Texture4    (sizeof(_Faces_Normal_Texture4))
#define D3DFVF_LEN_Faces_Normal_Texture5    (sizeof(_Faces_Normal_Texture5))
#define D3DFVF_LEN_Faces_Normal_Texture6    (sizeof(_Faces_Normal_Texture6))
#define D3DFVF_LEN_Faces_Normal_Texture7    (sizeof(_Faces_Normal_Texture7))

#define D3DFVF_LEN_Faces_Texture0    (sizeof(_Faces_Texture0))
#define D3DFVF_LEN_Faces_Texture1    (sizeof(_Faces_Texture1))
#define D3DFVF_LEN_Faces_Texture2    (sizeof(_Faces_Texture2))
#define D3DFVF_LEN_Faces_Texture3    (sizeof(_Faces_Texture3))
#define D3DFVF_LEN_Faces_Texture4    (sizeof(_Faces_Texture4))
#define D3DFVF_LEN_Faces_Texture5    (sizeof(_Faces_Texture5))
#define D3DFVF_LEN_Faces_Texture6    (sizeof(_Faces_Texture6))
#define D3DFVF_LEN_Faces_Texture7    (sizeof(_Faces_Texture7))

#define DATA_NorText0    1
#define DATA_NorText1    2
#define DATA_NorText2    3
#define DATA_NorText3    4
#define DATA_NorText4    5
#define DATA_NorText5    6
#define DATA_NorText6    7
#define DATA_NorText7    8

#define DATA_Text0       10
#define DATA_Text1       11
#define DATA_Text2       12
#define DATA_Text3       13
#define DATA_Text4       14
#define DATA_Text5       15
#define DATA_Text6       16
#define DATA_Text7       17

#define DATA_DifText0       20
#define DATA_DifText1       21
#define DATA_DifText2       22
#define DATA_DifText3       23
#define DATA_DifText4       24
#define DATA_DifText5       25
#define DATA_DifText6       26
#define DATA_DifText7       27
};



namespace GraphicsStruct
{

	struct _Command{
		float X,Y;
		float XS,YS;
		DWORD ActionID;
		DWORD TextureID;
		TCHAR TextureFileName[256];
		WORD  State;
		_Command ()
		{
			X =0;Y=0;
			XS = 0.01f;YS=0.01f;
			ActionID = 0;
			TextureID = 0;
			wsprintf(TextureFileName,"");
			State = 0;
		}
		void Create(float Xpos,float Ypos,float XScal,float YScal,DWORD ActID,DWORD TextID,LPSTR TextFile,WORD wState/* State = 0*/ )
		{
			X = Xpos;
			Y = Ypos;
			XS = XScal;
			YS = YScal;
			ActionID = ActID;
			TextureID = TextID;
			wsprintf(TextureFileName,"%s",TextFile);
			State = wState;
		}
	};

	struct _BoundingRect
	{
		D3DXVECTOR2 P1;
		D3DXVECTOR2 P2;
	};

	struct _BoundingBox
	{
		D3DXVECTOR3    PositionA;
		D3DXVECTOR3    PositionB;
		_BoundingBox()
		{
			PositionA = D3DXVECTOR3(0,0,0);
			PositionB = D3DXVECTOR3(0,0,0);
		}
		_BoundingBox(D3DXVECTOR3 A,D3DXVECTOR3 B)
		{
			PositionA = A;
			PositionB = B;
		}
		_BoundingBox(D3DXVECTOR3* Pos,int nNum)
		{
			//int t =0;
			for(int i=0;i<nNum;i++)
			{
				if(i==0)
				{
					PositionA = PositionB = Pos[0];
				}
				else
				{
					if (PositionA.x > Pos[i].x) PositionA.x = Pos[i].x;
					if (PositionA.y > Pos[i].y) PositionA.y = Pos[i].y;
					if (PositionA.z > Pos[i].z) PositionA.z = Pos[i].z;

					if (PositionB.x < Pos[i].x) PositionB.x = Pos[i].x;
					if (PositionB.y < Pos[i].y) PositionB.y = Pos[i].y;
					if (PositionB.z < Pos[i].z) PositionB.z = Pos[i].z;
				}
			}
		}
	};


	typedef struct _BoundingSphere
	{
		D3DXVECTOR3  Center;
		FLOAT        Radius;
	}BOUNDING_SPHERE;
	struct _EngineOpition
	{
		bool DrawSurroundBox;
		bool DrawGround;
	};


	struct _BoneInfo
	{
		DWORD  ID;
		LPSTR  Name;
		DWORD  m_cVertices;
		DWORD* VerticesIndex;
		float* VerticesWeight;
		DWORD  ParentID;
		DWORD  NumSubBone;
		DWORD* SubBoneID;
		D3DXMATRIX m_MatOffset;
		LPD3DXANIMATIONCONTROLLER   pKeyFrameInterpolators;
	};

	struct _Force
	{
		DWORD Type;
		D3DXVECTOR3 Position;
		D3DXVECTOR3 Force;
		float Time;
		float Distance;
	};


	//-----------------------------------------------------------------------------
	// Name: enum CULLSTATE
	// Desc: Represents the result of the culling calculation on an object.
	//-----------------------------------------------------------------------------
	enum CULLSTATE
	{
		CS_UNKNOWN = 0,      // cull state not yet computed
		CS_INSIDE = 1,       // object bounding box is at least partly inside the frustum
		CS_OUTSIDE = 2,      // object bounding box is outside the frustum
		CS_INSIDE_SLOW = 3,  // OBB is inside frustum, but it took extensive testing to determine this
		CS_OUTSIDE_SLOW = 4, // OBB is outside frustum, but it took extensive testing to determine this
	};


	//-----------------------------------------------------------------------------
	// Name: struct CULLINFO
	// Desc: Stores information that will be used when culling objects.  It needs
	//       to be recomputed whenever the view matrix or projection matrix changes.
	//-----------------------------------------------------------------------------
	struct CULLINFO
	{
		D3DXVECTOR3 vecFrustum[8];    // corners of the view frustum
		D3DXPLANE planeFrustum[6];    // planes of the view frustum
		D3DXPLANE m_WaterReflectFrustum[6];
	};

};


// define message
#define WM_RENDERFINISH     WM_USER + 5
#define MSG_ROLLCAMERA      WM_USER + 6  

#define WM_DEVICELOST       WM_USER + 10
#define WM_DEVICERESTORED   WM_USER + 11

#define WM_STARTSENCE       WM_USER + 20
#define WM_STARTSENCEFAILE  WM_USER + 21

#define WM_DI_KEYDOWN       WM_USER + 31
#define WM_DI_KEYUP         WM_USER + 32
#define WM_DI_MOUSEDOWN     WM_USER + 33
#define WM_DI_MOUSEUP       WM_USER + 34
#define WM_DI_MOUSEMOVE     WM_USER + 35

#define WM_EDIT_HIDE_ALL          WM_USER + 50
#define WM_EDIT_SHOW_ALL          WM_USER + 51
#define WM_EDIT_SHOW_FACES_ONLY   WM_USER + 53
// define faces attribution

#define  FACES_LOADED               0x00000001
#define  FACES_DELETED              0x00000002
#define  FACES_INDEXED              0x00000004
#define  FACES_DRAWSURROUNDBOX      0x00000008
#define  FACES_SELECTTRIANGLE       0x0000000F
#define  FACES_SELECTVERTICE        0x00000010

// define error value return

#define  E_DeviceNoExist           -1000;
#define  E_DataNoExist             -1001;
#define  E_CannotDoThis            -1002;

#define  E_NewOperFailed           -1004;

// define states 
#define STATE_GAMERUNNING               0

#define STATE_EDIT_CAMERA_ARC_MOVE      100
#define STATE_EDIT_CAMERA_MOVE          101
#define STATE_EDIT_CAMERA_PAN           102
#define STATE_EDIT_CAMERA_PROJECTION    103

#define STATE_EDIT_SELECT_OBJECT        2
#define STATE_EDIT_SELECT_LAYER         3
#define STATE_EDIT_SELECT_MESH          4
#define STATE_EDIT_SELECT_MATERIAL      5
#define STATE_EDIT_SELECT_ELEMENT       6
#define STATE_EDIT_SELECT_FACE          7
#define STATE_EDIT_SELECT_TEXTURE       9
#define STATE_EDIT_MUTI_SELECT          10

#define STATE_EDIT_MESH_SELECT_VERTICES 20 
#define STATE_EDIT_MESH_SELECT_FACES    21
#define STATE_EDIT_MESH_EDIT_MATERIAL   22
#define STATE_EDIT_MESH_MOVE_MATERIAL   23
#define STATE_EDIT_MESH_EDIT_TEXTURE    24
#define STATE_EDIT_MESH_MOVE_TEXTURE    25

#define STATE_EDIT_MESH_SET_CENTER      26
#define STATE_EDIT_MESH_SET_SCALE       27
#define STATE_EDIT_MESH_MOVE            28
#define STATE_EDIT_MESH_ROTALE          29
#define STATE_EDIT_MESH_SCALE           30
#define STATE_EDIT_MESH_UNIT            31
#define STATE_EDIT_MESH_SPLIT           32

#define STATE_EDIT_SELECT_VERTICES      40
#define STATE_EDIT_VERTICES_MOVE        41

#define STATE_EDIT_OBJECT_MOVE          50
#define STATE_EDIT_OBJECT_ROTATE        51
#define STATE_EDIT_OBJECT_SCALE         52
#define STATE_EDIT_OBJECT_OVERVIEW      53

#define STATE_EDIT_BONEPOINT_CREATE     60
#define STATE_EDIT_BONEPOINT_SELECT     61
#define STATE_EDIT_BONEPOINT_MOVE       62

#define STATE_EDIT_BONE_CREATE          65
#define STATE_EDIT_BONE_SELECT          66
#define STATE_EDIT_BONE_LINKSELECTEDVERTICES   67
#define STATE_EDIT_BONE_ADJUSTVECTOR    68
#define STATE_EDIT_BONE_MOVE            69

#define STATE_PAUSE                     0XFFFF
////////////////////////////////////////////////////////////////////////////////////

#define SELECTIONSTATE_RECT_NONE         0
#define SELECTIONSTATE_RECT_NEWSELECTION 1
#define SELECTIONSTATE_RECT_SECONDCORNER 2
#define SELECTIONSTATE_RECT_CONFIRM      3

#define SELECTPOSITION_NONE              0
#define SELECTPOSITION_SELECT_Y_PLANE    1
#define SELECTPOSITION_SELECT_XZ         2
#define SELECTPOSITION_CONFIRM           3


struct LogicCellBBox
{
	BBox Box;
	DWORD dwType;
};

////////////////////////////////////////////////////////////////////////////////
// 注意:此结构只能追加的成员只能追加在后面!!
#ifndef tstring
#if defined(UNICODE) | defined(_UNICODE)
#define tstring wstring
#else
#define tstring string
#endif
#endif
typedef struct _KG3DOBJProperty
{
	// =========================显示相关(Display)===========================
	BOOL    bAutoProduceObstacle;
	BOOL    bObscatleCamera;
	BOOL    bAffectByTime;

	//
	BOOL bInitallized;

	_KG3DOBJProperty()
	{
		ZeroMemory(this, sizeof(_KG3DOBJProperty));
		bAutoProduceObstacle = TRUE;
        bObscatleCamera = true;
	}
}KG3DOBJProperty;

#define defFILE_TAGLEN			8
#define defFILE_DESCLEN			64
#define defFILE_COMMONTAG		"KSword3D"	//文件头部标志
#define defFILE_COPYRIGHTDESC	"Copyright 1988-2004 Kingsoft Corporation. All rights Reserved"		//版权信息（默认填入文件中）
struct TFileHeader//剑网三2进制文件标准头 
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

/*
   octree container
   author : Chiyer
 */


struct int3
{
    int3(int x = 0, int y = 0, int z = 0)
        : _x(x), _y(y), _z(z)
    {
    }

    int3& operator*= (const int3& oth)
    {
        _x *= oth._x;
        _y *= oth._y;
        _z *= oth._z;

        return *this;
    }

    int3& operator/= (const int3& oth)
    {
        _x /= oth._x;
        _y /= oth._y;
        _z /= oth._z;

        return *this;
    }


    bool operator == (const int3& oth) const
    {
        return _x == oth._x && _y == oth._y && _z == oth._z;
    }

    int _x;
    int _y;
    int _z;
};

typedef int3 node_shape;
typedef int3 node_pos;
#define node_pos_all  node_pos(0xff, 0xff, 0xff)
#define node_pos_none node_pos(0xef, 0xef, 0xef)

template<typename type>
struct octree_node
{
    octree_node()
    {
        memset(sub, 0, sizeof(sub));
        shape = int3(1, 1, 1);
        level = 0;
    }

    octree_node(const type& data)
    {
        memset(sub, 0, sizeof(sub));
        shape = int3(1, 1, 1);
        this->data = data;
        level = 0;
    }

    ~octree_node()
    {
        for (int x = 0; x < 2; ++x)
            for (int y = 0; y < 2; ++y)
                for (int z = 0; z < 2; ++z)
                    SAFE_DELETE(sub[x][y][z]);
    }

    bool is_leaf() const
    {
        for (int x = 0; x < 2; ++x)
            for (int y = 0; y < 2; ++y)
                for (int z = 0; z < 2; ++z)
                    if (sub[x][y][z])
                        return false;
        return true;
    }

    octree_node* find_sub(node_pos pos)
    {
        pos /= shape;
        return sub[pos._x][pos._y][pos._z];
    }

    template<typename type_func, typename type_param>
    int subsection(type_func subfunc, type_param param)
    {
        int num = 1;
        if (subfunc(this, param))
        {
            for (int x = 0; x < 2; ++x)
                for (int y = 0; y < 2; ++y)
                    for (int z = 0; z < 2; ++z)
                        if (sub[x][y][z])
                        {
                            sub[x][y][z]->level = level + 1;
                            num += sub[x][y][z]->subsection(subfunc, param);
                        }
        }
        else
        {
            for (int x = 0; x < 2; ++x)
                for (int y = 0; y < 2; ++y)
                    for (int z = 0; z < 2; ++z)
                        if (sub[x][y][z])
                            sub[x][y][z]->level = level + 1;
        }
        return num;
    }

    template<typename type_func, typename type_param, typename type_res>
    bool travel(type_func travelfunc, type_param param, type_res& res)
    {
        node_pos subpos = node_pos_none;

        if (!travelfunc(this, param, res, subpos))
        {
            if (subpos == node_pos_none || is_leaf())
                return false;

            if (subpos == node_pos_all)
            {
                bool res = false;
                for (int x = 0; x < 2; ++x)
                    for (int y = 0; y < 2; ++y)
                        for (int z = 0; z < 2; ++z)
                            if (sub[x][y][z])
                                res |= sub[x][y][z]->travel(travelfunc, param, res);
                return res;
            }

            octree_node* p = find_sub(subpos);

            if (p)
                return p->travel(travelfunc, param, res);
            return false;
        }

        return true;
    }

    octree_node* sub[2][2][2];
    node_shape shape;
    int level;
    type data;
};

template<typename type>
struct octree
{
    octree()
        : pRoot(NULL)
    {
    }

    ~octree()
    {
        clear();
    }

    /* init octree, will create the root node whose value is data */
    void init(const type& data) 
    {
        clear();
        pRoot = new octree_node<type>(data);
    }

    /* subsetion the octree use the subfunc, and the param will
    be passed to the subfunc, the form of subfunc like this :

    bool subfunc(octree_node* curr_node, type_param param)
    {
    curr_node.pSub[xx] = new node();
    return ture;
    }

    return true if need to subsection the new node, return
    false to stop subsection, the curr_node is the node be
    subsection now.
    */
    template<typename type_func, typename type_param>
    int subsection(type_func subfunc, type_param param)
    {
        if (pRoot)
            return pRoot->subsection(subfunc, param);
        return 0;
    }

    /* traversing the octree to do something to any node, 
    the form of travelfunc like this :

    bool travelfunc(octree_node<int>* node, int param, bool& res, int& subdir)
    {
    do someting to node with the param,
    the res is result which return to caller,
    the subdir is the sub travel direction
    }

    return true to stop travel, false to continue
    */
    template<typename type_func, typename type_param, typename type_res>
    bool travel(type_func travelfunc, type_param param, type_res& res)
    {
        if (pRoot)
            return pRoot->travel(travelfunc, param, res);
        return false;
    }

    void clear()
    {
        SAFE_DELETE(pRoot);
    }

    octree_node<type>* pRoot;
};

struct SkyBoxVertex
{
	D3DXVECTOR3 Pos;
	D3DXVECTOR3 Nor;
	D3DXVECTOR3 Tex;
};

struct SphereNormalVertex
{
	D3DXVECTOR3 vec3Pos;
	D3DXVECTOR3 vec3Normal;
};



#endif //_INCLUDE_KG3DSTRUCTS_H_
