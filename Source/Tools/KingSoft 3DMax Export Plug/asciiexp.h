//************************************************************************** 
//* Asciiexp.h	- Ascii File Exporter
//* 
//* By Christer Janson
//* Kinetix Development
//*
//* January 20, 1997 CCJ Initial coding
//*
//* Class definition 
//*
//* Copyright (c) 1997, All Rights Reserved. 
//***************************************************************************

#ifndef __ASCIIEXP__H
#define __ASCIIEXP__H


extern ClassDesc* GetAsciiExpDesc();
extern TCHAR *GetString(int id);
extern HINSTANCE hInstance;

#define VERSION			200			// Version number * 100
//#define FLOAT_OUTPUT	_T("%4.4f")	// Float precision for output
#define CFGFILENAME		_T("ASCIIEXP.CFG")	// Configuration file

class MtlKeeper {
public:
	BOOL	AddMtl(Mtl* mtl);
	int		GetMtlID(Mtl* mtl);
	int		Count();
	Mtl*	GetMtl(int id);

	Tab<Mtl*> mtlTab;
};
void Matrix3ToMatrix4(CMesh::LPMATRIX pMat4,Matrix3* pMat3);
Matrix3 ExchangeMatrixYZ(Matrix3* pMat);
// This is the main class for the exporter.

typedef struct _KG_3D_EXPORT_VERTEX
{
    DWORD           dwOrignIndex;
    Point3          pt3Pos;
    Point3          pt3TextureCoord[3];
    CMesh::COLOR4   Diffuse;
    Point3          pt3Normal;

} KG_3D_EXPORT_VERTEX;

class IEditNormalsMod;
typedef struct _KG_3D_EXPORT_FACE
{
    DWORD               dwIndex[3];
    KG_3D_EXPORT_VERTEX Vertex[3];
    DWORD               dwMaterialID;
} KG_3D_EXPORT_FACE;

typedef struct _KG_3D_EXPORT_BONE_VERTEX
{
    int   nIndex;
    float fWeight;

} KG_3D_EXPORT_BONE_VERTEX;

typedef struct _KG_3D_EXPORT_BONE
{
    INode *pThis;
    INode *pParent;
    vector<INode *> Children;
    vector<KG_3D_EXPORT_BONE_VERTEX> Vertices;

    //offset from org in obj space
    CMesh::MATRIX  MatrixOffset;
    //offset from parent bone
    CMesh::MATRIX  BaseBoneMatrix;

    int clear()     // equal memset 0
    { 
        pThis = NULL;
        pParent = NULL;
        Children.clear();
        Vertices.clear();
        memset(&MatrixOffset, 0, sizeof(MatrixOffset));
        memset(&BaseBoneMatrix, 0, sizeof(BaseBoneMatrix));

        return true;
    } ;
} KG_3D_EXPORT_BONE;

typedef vector<KG_3D_EXPORT_BONE> KG_3D_EXPORT_BONE_VECTOR;


class KG_3D_EXPORT_ERROR
{
public:
    KG_3D_EXPORT_ERROR();

    HRESULT Init(HWND hMainWnd, const CStr &cstrFileName);
    HRESULT UnInit();

    HRESULT AddError(const CStr &cstrError);
      
private:
    typedef vector<CStr> KG_STRING_VECTOR;
    
    HWND                m_hMainWnd;
    KG_STRING_VECTOR    m_Strings;
    CStr                m_strFileName;
};

typedef set<INode *> KG_BONE_CONTAINER;

typedef map<INode *, int> KG_BONE_CONTAINER_TABLE;

typedef vector<CMesh::VERTICESSKININFO> KG_VERTICESSKININFO_VECTOR;

class AsciiExp : public SceneExport {
public:
	BOOL  m_bInch2CM;

//////////////////////////////////////////////////////////////////////////
//Add 5-31-2005 Bone reconnection fix
//Note: There might be some bone not animated in the animation, so they are not
//	    bound to the ISkin modifier, thus they will not be added to the bone list
//      when exporting the skin information. And As they will never do any relative
//      movements with it's parent bone, there is no need to export them. What
//      needs to be done is relink their direct child( if there is any) to their parent
//      node.
private:
	std::map<INode*, INode*> m_relink;
	std::map<INode*, std::vector<INode*>* > m_NumrelinkChild;
	void AdjustSkeleton(INode** ppNodeList, ISkin* pSkin, BOOL bForearmAdj);
	BOOL IsBone(INode* pBoneNode, INode** ppNodeList, DWORD dwNumBones);//Help function
//Add 5-31-2005 Bone reconnection fix end
//////////////////////////////////////////////////////////////////////////

public:
	INode* ValidateParentNode(INode* pNode,
		const KG_BONE_CONTAINER_TABLE& crBoneContainerTable);
	HRESULT FixSkinInfo(INode* node, LPMESH pMesh);
	BOOL ChangeSuffix(TCHAR* strSource, const TCHAR* strSourceSuffix, const TCHAR* strDestSuffix);
	BOOL CheckNodeSuffix(INode* pNode, const TCHAR* strSuffix);
	INode* FindBaseBone(INode* pBone);
	void PreFindBones();
	INode* FindBoneByName(const TCHAR* strBoneName, CMesh::LPBONEINFO pBoneInfo, DWORD dwNumBones);
	
    void FindAllBones(ISkin* pSkin, INode** pBoneNodeList, DWORD& NumBones);
    INode* FindBoneByName(const TCHAR* strBoneName, INode** ppNodeList, DWORD dwNumBones);
	void AddBone(INode* pBoneNode,INode** pBoneNodeList,DWORD& NumBones);


    HRESULT KG_AddBone(KG_BONE_CONTAINER_TABLE *pBoneContainerTable, INode *pBoneNode);
    INode *KG_FindBoneByName(const KG_BONE_CONTAINER_TABLE &cBoneContainerTable, const char cszBoneName[]);
    HRESULT KG_AddAllBone(KG_BONE_CONTAINER_TABLE *pBoneContainerTable);

	
	//HRESULT ExchangeParticleArrayAnimation(LPMESH pDestMesh,INode* node,Mesh* pSrcMesh,PArrayParticle* pArray,int ParticleIndex);
	//HRESULT ExchangeParticleArrayData(CMeshTable* pDestMeshTable,INode* node,TimeValue t);

    HRESULT _ExchangeMeshData_GetVertexPos(
        const Mesh *pcSrcMesh, 
        Point3 *pDestVertices, 
        const Matrix3 &cmTranform, 
        TimeValue t,
        INode *pNode
    );
    HRESULT _ExchangeMeshData_GetTextureCoord_0(
        int nFaceIndex,
        KG_3D_EXPORT_FACE *pExportFace, 
        INode *pNode,
        Mesh *pSrcMesh, 
        int VertexOrder[3]
    );
    HRESULT _ExchangeMeshData_GetTextureCoord(
        int nFaceIndex,
        KG_3D_EXPORT_FACE *pExportFace, 
        Mesh *pSrcMesh, 
        DWORD dwChannelID,
        int VertexOrder[3]
    );

    HRESULT _ExchangeMeshData_Normal(
        int nFaceIndex,
		IEditNormalsMod *pEditNormalMod,
		Matrix3 TM,
        KG_3D_EXPORT_FACE *pExportFace, 
        Mesh *pSrcMesh, 
        int VertexOrder[3]
    );
    
    HRESULT _ExchangeMeshData_SetDestMesh(
        int nFaceNum,
        KG_3D_EXPORT_FACE   *pExportFace, 
        KG_3D_EXPORT_VERTEX *pNewVertex,
        int nNewVertex,
        LPMESH pDestMesh
    );
    
    
    
	HRESULT ExchangeMeshData(LPMESH pDestMesh,INode* node,Mesh* pSrcMesh,Mesh* pSrcTextMesh,TimeValue t,Matrix3 tm);
	
	HRESULT ExportBoneAnimation(INode* node,LPMESH pMesh);
	HRESULT ExportRelateBoneAnimation(INode *node, LPMESH pMesh);
	HRESULT ExportRTSBoneAnimation(INode *node, LPMESH pMesh);
	HRESULT ExportRelateBoneAnimationKeyOnly(INode *node, LPMESH pMesh);
	
	AsciiExp();
	~AsciiExp();

	HRESULT DoTest(Interface* pIp, const TCHAR* strFileName);
	// SceneExport methods
	int    ExtCount();     // Number of extensions supported 
	const TCHAR * Ext(int n);     // Extension #n (i.e. "ASC")
	const TCHAR * LongDesc();     // Long ASCII description (i.e. "Ascii Export") 
	const TCHAR * ShortDesc();    // Short ASCII description (i.e. "Ascii")
	const TCHAR * AuthorName();    // ASCII Author name
	const TCHAR * CopyrightMessage();   // ASCII Copyright message 
	const TCHAR * OtherMessage1();   // Other message #1
	const TCHAR * OtherMessage2();   // Other message #2
	unsigned int Version();     // Version number * 100 (i.e. v3.01 = 301) 
	void	ShowAbout(HWND hWnd);  // Show DLL's "About..." box
	int		DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0); // Export	file
	BOOL	SupportsOptions(int ext, DWORD options);

	// Other methods

	// Node enumeration
	BOOL	nodeEnum(INode* node, int indentLevel);
	INode*  FindRootBone(INode* pNode);
	void	PreProcess(INode* node, int& nodeCount);

	// High level export
	void	ExportGlobalInfo();
	void	ExportMaterialList();
	HRESULT ExportGeomObject(INode* node, int indentLevel); 
	void	ExportLightObject(INode* node, int indentLevel); 
	void	ExportCameraObject(INode* node, int indentLevel); 
	void	ExportShapeObject(INode* node, int indentLevel); 
	void	ExportHelperObject(INode* node, int indentLevel);

	// Mid level export
	void	ExportSkinInfo(INode* node, LPMESH pMesh); 

    HRESULT ExportSkinInfo1_Skin(
        INode *pNode, 
        KG_VERTICESSKININFO_VECTOR  *pVerticesSkinInfoVector,
        KG_BONE_CONTAINER_TABLE     *pBoneContainerTable
    );
    HRESULT ExportSkinInfo1_ProcessBoneBaseInfo(
        const KG_BONE_CONTAINER_TABLE &crBoneContainerTable,
        KG_3D_EXPORT_BONE_VECTOR *pBoneVector
    );

    HRESULT ExportSkinInfo1_TransVertexWeightToBone(
        LPMESH pMesh,               
        const KG_VERTICESSKININFO_VECTOR  &crVerticesSkinInfoVector,
        const KG_BONE_CONTAINER_TABLE     &crBoneContainerTable,
        KG_3D_EXPORT_BONE_VECTOR          *pExportBoneVector
    );

    HRESULT ExportSkinInfo1_ExportSocketToMesh(
        LPMESH lpMesh,
        const KG_3D_EXPORT_BONE_VECTOR &crExportBoneVector
    );

    HRESULT ExportSkinInfo1_SetDestMesh(
        LPMESH pMesh,
        const KG_3D_EXPORT_BONE_VECTOR &crExportBoneVector
    );



	HRESULT	ExportSkinInfo1(INode* node, LPMESH pMesh);
	HRESULT	ExportMesh(INode* node, LPMESH pMesh,TimeValue t, int indentLevel); 
	void	ExportAnimKeys(INode* node, int indentLevel);
	void	ExportMaterial(INode* node,LPMESH pMesh, int indentLevel); 
	void	ExportAnimMesh(INode* node, LPMESH pMesh, int indentLevel); 
	void	ExportIKJoints(INode* node, int indentLevel);
	void	ExportNodeTM(INode* node, int indentLevel);
	void	ExportNodeHeader(INode* node, TCHAR* type, int indentLevel);
	void	ExportCameraSettings(CameraState* cs, CameraObject* cam, TimeValue t, int indentLevel);
	void	ExportLightSettings(LightState* ls, GenLight* light, TimeValue t, int indentLevel);

	// Low level export
	void	DumpPoly(PolyLine* line, Matrix3 tm, int indentLevel);
	void	DumpMatrix3(Matrix3* m, int indentLevel);
	void	DumpMaterial(Mtl* mtl, int mtlID, int subNo, int indentLevel);
	void	DumpTexture(Texmap* tex, Class_ID cid, int subNo, float amt, int	indentLevel);
	void	DumpJointParams(JointParams* joint, int indentLevel);
	void	DumpUVGen(StdUVGen* uvGen, int indentLevel); 
	void	DumpPosSample(INode* node, int indentLevel); 
	void	DumpRotSample(INode* node, int indentLevel); 
	void	DumpScaleSample(INode* node, int indentLevel); 
	void	DumpPoint3Keys(Control* cont, int indentLevel);
	void	DumpFloatKeys(Control* cont, int indentLevel);
	void	DumpPosKeys(Control* cont, int indentLevel);
	void	DumpRotKeys(Control* cont, int indentLevel);
	void	DumpScaleKeys(Control* cont, int indentLevel);

	// Misc methods
	TCHAR*	GetMapID(Class_ID cid, int subNo);
	Point3	GetVertexNormal(Mesh* mesh, int faceNo, RVertex* rv);
	BOOL	CheckForAndExportFaceMap(Mtl* mtl, Mesh* mesh, int indentLevel); 
	void	make_face_uv(Face *f, Point3 *tv);
	BOOL	TMNegParity(Matrix3 &m);
	TSTR	GetIndent(int indentLevel);
	TCHAR*	FixupName(TCHAR* name);
	void	CommaScan(TCHAR* buf);
	BOOL	CheckForAnimation(INode* node, BOOL& pos, BOOL& rot, BOOL& scale);
	TriObject*	GetTriObjectFromNode(INode *node, TimeValue t, int &deleteIt);
	BOOL	IsKnownController(Control* cont);

	// A collection of overloaded value to string converters.
	TSTR	Format(int value);
	TSTR	Format(float value);
	TSTR	Format(Color value);
	TSTR	Format(Point3 value); 
	TSTR	Format(AngAxis value); 
	TSTR	Format(Quat value);
	TSTR	Format(ScaleValue value);
	BOOL    m_bForearm;
    
	// Configuration methods
	TSTR	GetCfgFilename();
	BOOL	ReadConfig();
	void	WriteConfig();
	
    static BOOL GetStdPlugsDir(LPSTR val);

	inline BOOL	GetSaveVerticesPosition(){ return bVerticesPosition; }
	inline BOOL GetSaveAsMatrix()       { return bSaveAsMatrix; }
	inline BOOL	GetSaveVerticesNormal()	{ return bVerticesNormal; }
	inline BOOL	GetSaveVerticesDiffuse(){ return bVerticesDiffuse; }
	inline BOOL	GetSaveTextureUVW1()	{ return bTextureUVW1; }
	inline BOOL	GetSaveTextureUVW2()	{ return bTextureUVW2; }
	inline BOOL	GetSaveTextureUVW3()	{ return bTextureUVW3; }
	inline BOOL	GetSaveMapDiffuse()		{ return bMapDiffuse; }
	inline BOOL	GetSaveMapBump()		{ return bMapBump; }
	inline BOOL	GetSaveMapSpecular()	{ return bMapSpecular; }
	inline BOOL	GetSaveMapReflection()	{ return bMapReflection; }
	// Interface to member variables
	inline BOOL	GetSaveMesh()			{ return bSaveMesh; }
	inline BOOL	GetSaveAnimation()		{ return bSaveAnimation; }
	inline BOOL	GetSaveMaterial()		{ return bSaveMaterial; }
	inline BOOL	GetUnitModel()			{ return bUnitModel; }
	inline BOOL	GetExchangeYZ()		    { return bExchangeYZ; }
	inline BOOL	GetDiffuseOnly()		{ return bDiffuseOnly; }
	inline BOOL	GetMoveTextureFile()	{ return bMoveTextureFile; }
	inline LPSTR GetTextureDir()	    { return scTextureDir; }
	inline DWORD GetAnimationType()	    { return dwAnimationType; }
	inline LPSTR GetAnimationName()	    { return scAnimationName; }
	inline BOOL GetNormalizeSocket()    { return bNormalizeSocketSize;}

	inline BOOL	GetIncludeMesh()			{ return bIncludeMesh; }
	inline BOOL GetIncludeUVAni()           { return bIncludeUVAni; }
	inline BOOL	GetIncludeAnim()			{ return bIncludeAnim; }
	inline BOOL	GetIncludeMtl()				{ return bIncludeMtl; }
	inline BOOL	GetIncludeMeshAnim()		{ return bIncludeMeshAnim; }
	inline BOOL	GetIncludeCamLightAnim()	{ return bIncludeCamLightAnim; }
	inline BOOL	GetIncludeIKJoints()		{ return bIncludeIKJoints; }
	inline BOOL	GetIncludeNormals()			{ return bIncludeNormals; }
	inline BOOL	GetIncludeTextureCoords()	{ return bIncludeTextureCoords; }
	inline BOOL	GetIncludeVertexColors()	{ return bIncludeVertexColors; }
	inline BOOL	GetIncludeObjGeom()			{ return bIncludeObjGeom; }
	inline BOOL	GetIncludeObjShape()		{ return bIncludeObjShape; }
	inline BOOL	GetIncludeObjCamera()		{ return bIncludeObjCamera; }
	inline BOOL	GetIncludeObjLight()		{ return bIncludeObjLight; }
	inline BOOL	GetIncludeObjHelper()		{ return bIncludeObjHelper; }
	inline BOOL	GetAlwaysSample()			{ return bAlwaysSample; }
	inline int	GetMeshFrameStep()			{ return nMeshFrameStep; }
	inline int	GetKeyFrameStep()			{ return nKeyFrameStep; }
	inline int	GetPrecision()				{ return nPrecision; }
	inline TimeValue GetStaticFrame()		{ return nStaticFrame; }
	inline Interface*	GetInterface()		{ return m_ip; }


	inline void	SetSaveVerticesPosition(BOOL val){ bVerticesPosition = val; }
	inline void	SetSaveVerticesNormal(BOOL val)  { bVerticesNormal = val; }
	inline void	SetSaveVerticesDiffuse(BOOL val) { bVerticesDiffuse = val; }
	inline void	SetSaveTextureUVW1(BOOL val)	 { bTextureUVW1 = val; }
	inline void	SetSaveTextureUVW2(BOOL val)	 { bTextureUVW2 = val; }
	inline void	SetSaveTextureUVW3(BOOL val)	 { bTextureUVW3 = val; }
	inline void	SetSaveMapDiffuse(BOOL val)		 { bMapDiffuse = val; }
	inline void	SetSaveMapBump(BOOL val)		 { bMapBump = val; }
	inline void	SetSaveMapSpecular(BOOL val)	 { bMapSpecular = val; }
	inline void	SetSaveMapReflection(BOOL val)	 { bMapReflection = val; }

	inline void	SetIncludeMesh(BOOL val)			{ bIncludeMesh = val; }
	inline void SetIncludeUVAni(BOOL val)			{ bIncludeUVAni = val; }
	inline void	SetIncludeAnim(BOOL val)			{ bIncludeAnim = val; }
	inline void	SetIncludeMtl(BOOL val)				{ bIncludeMtl = val; }
	inline void	SetIncludeMeshAnim(BOOL val)		{ bIncludeMeshAnim = val; }
	inline void	SetIncludeCamLightAnim(BOOL val)	{ bIncludeCamLightAnim = val; }
	inline void	SetIncludeIKJoints(BOOL val)		{ bIncludeIKJoints = val; }
	inline void	SetIncludeNormals(BOOL val)			{ bIncludeNormals = val; }
	inline void	SetIncludeTextureCoords(BOOL val)	{ bIncludeTextureCoords = val; }
	inline void	SetIncludeVertexColors(BOOL val)	{ bIncludeVertexColors = val; }
	inline void	SetIncludeObjGeom(BOOL val)			{ bIncludeObjGeom = val; }
	inline void	SetIncludeObjShape(BOOL val)		{ bIncludeObjShape = val; }
	inline void	SetIncludeObjCamera(BOOL val)		{ bIncludeObjCamera = val; }
	inline void	SetIncludeObjLight(BOOL val)		{ bIncludeObjLight = val; }
	inline void	SetIncludeObjHelper(BOOL val)		{ bIncludeObjHelper = val; }
	inline void	SetAlwaysSample(BOOL val)			{ bAlwaysSample = val; }
	inline void SetMeshFrameStep(int val)			{ nMeshFrameStep = val; }
	inline void SetKeyFrameStep(int val)			{ nKeyFrameStep = val; }
	inline void SetPrecision(int val)				{ nPrecision = val; }
	inline void SetStaticFrame(TimeValue val)		{ nStaticFrame = val; }

	inline void	SetSaveMesh(BOOL val)	 			{ bSaveMesh = val; }
	inline void	SetSaveAnimation(BOOL val)			{ bSaveAnimation = val; }
	inline void	SetSaveMaterial(BOOL val)			{ bSaveMaterial = val; }
	inline void	SetUnitModel(BOOL val)				{ bUnitModel = val; }
	inline void	SetExchangeYZ(BOOL val)				{ bExchangeYZ = val; }
	inline void SetNormalizeSocketSize(BOOL val)    { bNormalizeSocketSize = val;}
	inline void	SetDiffuseOnly(BOOL val)			{ bDiffuseOnly = val; }
	inline void	SetMoveTextureFile(BOOL val)		{ bMoveTextureFile = val; }
	inline void SetTextureDir(LPSTR val)			{ wsprintf(scTextureDir,"%s",val); }
	inline void SetAnimationType(DWORD val)			{ dwAnimationType = val; }
	inline void SetAnimationName(LPSTR val)			{ wsprintf(scAnimationName,"%s",val); }

private:
	///////////////////////////////////////////////////////////////////////////////////////
	//our bool set
	BOOL    bSaveMaterial;
	BOOL    bSaveAnimation;
	BOOL    bNormalizeSocketSize;
	BOOL    bSaveMesh;
	BOOL    bUnitModel;
	BOOL    bExchangeYZ;
	BOOL    bDiffuseOnly;
	BOOL    bMoveTextureFile;
	BOOL    bSaveAsMatrix;//是否要把骨骼动画保存成Matrix，如果不是则保存成旋转、缩放、平移
	TCHAR   scTextureDir[256];
	DWORD   dwAnimationType;
	TCHAR   scAnimationName[256];

	//add 2004/2/9
	BOOL    bVerticesPosition;
	BOOL    bVerticesNormal;
	BOOL    bVerticesDiffuse;
	BOOL    bTextureUVW1;
	BOOL    bTextureUVW2;
	BOOL    bTextureUVW3;

	BOOL    bMapDiffuse;
	BOOL    bMapBump;
	BOOL    bMapSpecular;
	BOOL    bMapReflection;
	BOOL    bCopyTextureFiles;
	TCHAR   scTextureDirectory[256];
	//2004/2/9 end
	//////////////////////////////////////////////////////////////////////////
	
	BOOL	bIncludeMesh;
	BOOL    bIncludeUVAni;
	BOOL	bIncludeAnim;
	BOOL	bIncludeMtl;
	BOOL	bIncludeMeshAnim;
	BOOL	bIncludeCamLightAnim;
	BOOL	bIncludeIKJoints;
	BOOL	bIncludeNormals;
	BOOL	bIncludeTextureCoords;
	BOOL	bIncludeObjGeom;
	BOOL	bIncludeObjShape;
	BOOL	bIncludeObjCamera;
	BOOL	bIncludeObjLight;
	BOOL	bIncludeObjHelper;
	BOOL	bAlwaysSample;
	BOOL	bIncludeVertexColors;
	int		nMeshFrameStep;
	int		nKeyFrameStep;
	int		nPrecision;
 	TimeValue	nStaticFrame;

	Interface*	m_ip;
	//FILE*		pStream;
	int			nTotalNodeCount;
	int			nCurNode;
	TCHAR		szFmtStr[16];

	MtlKeeper	mtlList;
	
	CMeshTable  m_cMeshTable;//mesh table to exchange date and save

	//////////////////////////////////////////////////////////////////////////
	KG_BONE_CONTAINER m_SelectedNode;
	KG_BONE_CONTAINER m_Bones;
	BOOL    nodeEnum1(INode* node);
	TCHAR m_strDefaultTextureDir[MAX_PATH];
	//////////////////////////////////////////////////////////////////////////


    KG_3D_EXPORT_ERROR m_ExportError;
};

extern Matrix3 g_mat3Inch2CMScale;

#endif // __ASCIIEXP__H

