//
//#include "SimpObj.h"
//#include "suprprts.h"
//#include "stdmat.h"
//
//#define BASER 2
//
//
//typedef struct{
// float Vsz,Ts,Ts0,LamTs,A,LamA,To;
// float M,Dis,Fo,Inf,Mltvar,pvar;
// int themtl,showframe,gennum,frommesh,SpVar;
// TimeValue L,DL,persist;
// MtlID pmtl;
// Point3 wbb[8];
// Point3	V,Ve,W,RV,tv,start;
//} SavePt;
//
//typedef struct {
// Point3 Axis;
// TimeValue persist;
// float axisvar,div,pvar; 
// int axisentered;
//}VelDir2;
//typedef struct {
// float Size,VSz,VSpin,Phase,VPhase;
// float bstr,bstrvar,Speed,VSpeed;
// float ToAmp,VToAmp;
// float ToPhase,VToPhase,VToPeriod;
// TimeValue Spin,ToPeriod,Life,Vl;
//} VelDir;
//static Class_ID PArray_CLASS_ID(0xe3c25b5, 0x109d1659);
//
//class PickOperand;
//class PArrayParticle;
//
//// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
////        WARNING - a copy of this class description is in maxscrpt\maxnode.cpp
//// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//class PArrayParticleDraw : public CustomParticleDisplay {
//	public:
////		float size,VSz,
//		BOOL firstpart;
//		PArrayParticle *obj;
//		int disptype,ptype,bb,anifr,anioff;
//		boxlst *bboxpt;
//		TimeValue t;
//		InDirInfo indir;
//
//		PArrayParticleDraw() {obj=NULL;}
//		BOOL DrawParticle(GraphicsWindow *gw,ParticleSys &parts,int i);
//	};
//
//typedef struct{
//float M,Vsz;
//Point3 Ve,vel,pos;
//Point3 wbb[8];
//}CacheDataType;
//
//class PArrayParticle : public SimpleParticle, IParamArray {
//	public:
//		PArrayParticleDraw thePArrayDraw;
//		PArrayParticle();
//		~PArrayParticle();
//		static IParamMap *pmapParam;
//		static IParamMap *pmapPGen;
//		static IParamMap *pmapPType;
//		static IParamMap *pmapPSpin;
//		static IParamMap *pmapEmitV;
//		static IParamMap *pmapBubl;
//		static IParamMap *pmapSpawn;
//		int stepSize,size,oldtvnum,lastrnd,emitrnd;
//		static custsettings;
//		BOOL cancelled,wasmulti,storernd;
//		static BOOL creating;
//		BOOL fromgeom;
//		INode *distnode,*custnode,*cnode;
//		CacheDataType *storefrag;
//		TSTR distname,custname;
//		ULONG dflags;
//		int fragflags;
//		BOOL doTVs;
//		static AName *NameLst;
//		static HWND hParams2,hptype,hgen,hparam,hrot,hbubl,spawn;
//		static PickOperand pickCB;
//		static ICustEdit *custCtrlEdit;
//		Mtl *origmtl;
//
//		// to fix #182223 & 270224, add these members
//		Matrix3 lastTM;
//		TimeValue lastTMTime;
//
//		BOOL GenerateNotGeom(TimeValue t,TimeValue lastt,int c,int counter,INode *distnode,int type,Matrix3 tm,Matrix3 nxttm);
//		void GetInfoFromObject(float thick,int *c,INode *distnode,TimeValue t,TimeValue lastt);
//		void GetLocalBoundBox(TimeValue t, INode *inode,ViewExp* vpt,  Box3& box); 
//		void GetWorldBoundBox(TimeValue t, INode *inode, ViewExp* vpt, Box3& box);
//		void RendGeom(Mesh *pm,Matrix3 itm,int maxmtl,int maptype,BOOL eitmtl,float mval,PArrayParticleDraw thePArrayDraw,TVFace defface,BOOL notrend);
//
//		Tab<int> nmtls;
//		void DoSpawn(int j,int spmult,SpawnVars spvars,TimeValue lvar,BOOL emits,int &oldcnt);
//		void SetUpList();
//		void AddToList(INode *newnode,int i,BOOL add);
//		void DeleteFromList(int nnum,BOOL all);
//		Tab<INode*> nlist;
//		Tab<int> llist;
//		int deftime;
//		int maincount;
//		int NumRefs() {return BASER+nlist.Count();}
//		RefTargetHandle GetReference(int i);
//		void SetReference(int i, RefTargetHandle rtarg);		
//		RefResult NotifyRefChanged( Interval changeInt,RefTargetHandle hTarget, 
//		   PartID& partID, RefMessage message);
//		void SetUpLifeList();
//		void AddToLifeList(int newlife);
//		void DeleteFromLifeList(int nnum);
//		void ShowName(int dist);
//		int CountLive();
//		int rcounter,vcounter;
//		oldipc lc;
//		static IObjParam *ip;
//		CreateMouseCallBack* GetCreateMouseCallBack() {return NULL;}
//
//		void BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev);		
//		void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next);		
//		TimeValue ParticleLife(TimeValue t, int i);
//		// Animatable methods		
//		void DeleteThis() {delete this;}
//		IOResult Load(ILoad *iload);
//		IOResult Save(ISave *isave);
//		void MapKeys(TimeMap *map,DWORD flags);
//		int RenderBegin(TimeValue t, ULONG flags);		
//		int RenderEnd(TimeValue t);
//		
//		// From SimpleParticle
//		void BuildEmitter(TimeValue t, Mesh& amesh);
//		Interval GetValidity(TimeValue t);		
//		void InvalidateUI();
//		BOOL EmitterVisible();		
//		MarkerType GetMarkerType();	
//		SavePt *sdata;
//		Mesh *pmesh;
//		// From BaseObject
//		TCHAR *GetObjectName();
//		void BirthParticle(INode *node,TimeValue bt,int index,VelDir2 *ptvel2,VelDir ptvel,Matrix3 tmlast);
//		BOOL ComputeParticleStart(TimeValue t0,TimeValue lastt,INode *node,int c);
//		int IsInstanceDependent() {return 1;}
//
//		// From GeomObject
//		Mesh* GetRenderMesh(TimeValue t, INode *inode, View& view, BOOL& needDelete);
//
//		// Animatable methods		
//		Class_ID ClassID() {return PArray_CLASS_ID;} 
//
//		// From ref
//		RefTargetHandle Clone(RemapDir& remap = NoRemap());		
//
//		// From Simple Particle
//		void UpdateParticles(TimeValue t,INode *node);
//		ParamDimension *GetParameterDim(int pbIndex);
//		TSTR GetParameterName(int pbIndex);
//		void GetFilename(TCHAR *filename);
//		void SetupTargetList();
//		int SaveSettings(int overwrite,TCHAR *newname);
//		int GetSettings(int setnum,TCHAR *newname);
//		int RemSettings(int setnum,TCHAR *newname);
//		Point3 ParticlePosition(TimeValue t,int i);
//		Point3 ParticleVelocity(TimeValue t,int i);		
//		float ParticleSize(TimeValue t,int i);
//		int ParticleCenter(TimeValue t,int i);
//
//		Mesh *cmesh,*dispmesh;
//		Box3 *cmbb;
//		Point3 boxcenter;
//		int CustMtls;
//		TimeLst times;
//		void GetTimes(TimeLst &times,TimeValue t,int anifr,int ltype,int fragflags);
//		void TreeDoGroup(INode *node,Matrix3 tspace,Mesh *cmesh,Box3 *cmbb,int *numV,int *numF,int *tvnum,int *ismapped,TimeValue t,int subtree,int custmtl);
//		void CheckTree(INode *node,Matrix3 tspace,Mesh *cmesh,Box3 *cmbb,int *numV,int *numF,int *tvnum,int *ismapped,TimeValue t,int subtree,int custmtl);
//		void GetMesh(TimeValue t,int subtree,int custmtl,int fragflags);
//		void GetNextBB(INode *node,int subtree,int *count,int *tabmax,Point3 boxcenter,TimeValue t,int tcount,INode *onode);
//		void DoGroupBB(INode *node,int subtree,int *count,int *tabmax,Point3 boxcenter,TimeValue t,int tcount,INode *onode);
//		void GetallBB(INode *custnode,int subtree,TimeValue t,int fragflags);
//		void AssignMtl(INode *node,INode *topnode,int subtree,TimeValue t,int fragflags);
//		void DoGroupMtls(INode *node,int subtree,int *numsubs,int *numtabs,int *tabmax,TimeValue t);
//		void RetrieveMtls(INode *node,int subtree,int *numsubs,int *numtabs,int *tabmax,TimeValue t);
//		void GetSubs(INode *node,INode *topnode,int subtree,TimeValue t,int fragflags);
//		void CntDoGroupMtls(INode *node,int subtree,int *numsubs,int *numtabs,int *tabmax,TimeValue t);
//		void CntRetrieveMtls(INode *node,int subtree,int *numsubs,int *numtabs,int *tabmax,TimeValue t);
//		BOOL backpatch;
//		Mtllst mttab;
//		int Display(TimeValue t, INode* inode, ViewExp *vpt, int flags);
//		int HitTest(TimeValue t, INode *inode, int type, int crossing, int flags, 
//		IPoint2 *p, ViewExp *vpt);
//		TimeValue dispt;
//		void MovePart(int j,TimeValue dt,BOOL fullframe,int tpf);
//		void ResetSystem(TimeValue t,BOOL full=TRUE);
//	};
//
//	class PickOperand : 
//		public PickModeCallback,
//		public PickNodeCallback {
//	public:		
//		PArrayParticle *po;
//		int dodist,repi;
//		
//		PickOperand() {po=NULL;}
//
//		BOOL HitTest(IObjParam *ip,HWND hWnd,ViewExp *vpt,IPoint2 m,int flags);
//		BOOL Pick(IObjParam *ip,ViewExp *vpt);
//
//		void EnterMode(IObjParam *ip);
//		void ExitMode(IObjParam *ip);
//
//		BOOL RightClick(IObjParam *ip, ViewExp *vpt) { return TRUE; }
//		BOOL Filter(INode *node);
//		
//		PickNodeCallback *GetFilter() {return this;}
//	};