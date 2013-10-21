#pragma once
#include "IEEditor.h"
#include "KG3DSerpentine.h"

//class KG3DRepresentObjectNode;
//class KG3DFrustum;
//class KG3DMemoryFile;
//class KG3DSceneOutDoorSpaceMgr;
class KG3DTerrainRiver;
//struct _PartData;

struct KG3DTerrainRiverPassage : public KG3DPassage 
{		
	float m_fDepth;//河流的深度
	virtual HRESULT UpdateVertex(KG3DSerpentine* pSerpentine);
	virtual HRESULT UpdatAlpha(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr);
	D3DXCOLOR ComputerColor(float Height);
	///////////////////
	HRESULT UpdateTerrainHeight(KG3DTerrainRiver* pRiver,KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr);
};

class KG3DTerrainRiver : public KG3DSerpentine
{
	public:
		struct _RiverHead
		{
			DWORD        dwNumOfNode;//节点数目 
			DWORD        dwNodePosBlock;  //
			DWORD        dwNumOfPassage;//
			DWORD        dwPassageBlock;
			TCHAR		 scName[32];//河流名
			float        fWidth;//河流宽度
			float        fDepth;//河流深度
			float        fBendModulus;     //Hermite中切线方程的参数
			DWORD		 dwSegmentLength;   //分段长度
			DWORD        dwNumParentBlock;//
			DWORD        dwParentTerrainBlock;
			DWORD        dwReserved[5];
			_RiverHead()
			{
				dwNumOfNode = 0;
				dwNodePosBlock = 0;
				dwNumOfPassage =0;
				dwPassageBlock = 0;
				ZeroMemory(scName,sizeof(TCHAR)*32);
				fWidth = 0;
				fBendModulus = 0;
				fDepth = 0;
				dwSegmentLength=0; 
				dwNumParentBlock= 0;//
				dwParentTerrainBlock =0;
				ZeroMemory(dwReserved,sizeof(DWORD)*5);;
			}
			~_RiverHead()
			{
			}
		};
	public:
		float m_fDepth;//河流的深度
	public:
		void SetDepth(float fDepth);
		float GetDepth();
		
		virtual HRESULT AddOnePassage(KG3DPassage** ppPassage,int nPos);
		virtual HRESULT AddOnePassage(KG3DPassage** ppPassage);
		virtual HRESULT SaveToFile(const char cszFileName[]);
		virtual HRESULT LoadFromFile(const char cszFileName[]);
		virtual HRESULT LoadNode(DWORD dwNumOfNode,D3DXVECTOR3* pNodePos);

		HRESULT RefreshHeight(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr,BOOL bToTerrain);
		HRESULT ComputeAlpha(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr);
		
	public:
		KG3DTerrainRiver(void);
		virtual ~KG3DTerrainRiver(void);
};

class KG3DTerrainRiverMgr : public KG3DSerpentineMgr,
							public IEKG3DTerrainRiverMgr
{
public:
	KG3DTerrainRiverMgr();
	KG3DTerrainRiverMgr(KG3DSceneOutDoorSpaceMgr* pOutDoorMgr);
	~KG3DTerrainRiverMgr();
public:
	
	HRESULT LoadByHandle(UINT uHandle,KG3DTerrainRiver** ppOutRiver);
	HRESULT SaveToFile(LPCSTR strFileName); //保存河流信息

	virtual HRESULT DeleteOne(int nIndex);
	virtual HRESULT NewOne(KG3DSerpentine** ppSerpentine);//HRESULT AddOneNewRiver(KG3DTerrainRiver** ppRiver);

	HRESULT AddNodeToCurSerpentine(KG3DRepresentObjectNode* pNode,D3DXVECTOR3& vPos);
	
	virtual float GetCurrentRiverDepth();
	virtual void SetCurrentRiverDepth(float fDepth);
	virtual HRESULT ComputeCurrentRiverAlpha();
/////////////////////////////////////////////////////////////
	virtual DWORD GetCount(){return KG3DSerpentineMgr::GetCount();}
	HRESULT AddNewOne(){return KG3DSerpentineMgr::AddNewOne();}
	virtual HRESULT SetCurSerpentine(int nIndex){return KG3DSerpentineMgr::SetCurSerpentine(nIndex);}
	virtual BOOL	FindSerpentineByName(const TCHAR* strName){return KG3DSerpentineMgr::FindSerpentineByName(strName);}
	virtual HRESULT GetAllName(vector<std::string>& vecName){return KG3DSerpentineMgr::GetAllName(vecName);}
	virtual HRESULT RefreshCurSerpentine(){return KG3DSerpentineMgr::RefreshCurSerpentine();}
	virtual HRESULT RefreshHeight(BOOL bToTerrain){return KG3DSerpentineMgr::RefreshHeight(bToTerrain);}
	virtual float GetCurSerWidth(){return KG3DSerpentineMgr::GetCurSerWidth();}
	virtual void SetCurSerWidth(float fWidth){return KG3DSerpentineMgr::SetCurSerWidth(fWidth);}
	virtual void SetCurSerDrawSelectMark(BOOL bVal){return KG3DSerpentineMgr::SetCurSerDrawSelectMark(bVal);}
	virtual BOOL GetCurSerDrawSelectMark(){return KG3DSerpentineMgr::GetCurSerDrawSelectMark();}
	virtual void SetCurSerDrawExtendLine(BOOL bVal){return KG3DSerpentineMgr::SetCurSerDrawExtendLine(bVal);}
	virtual BOOL GetCurSerDrawExtendLine(){return KG3DSerpentineMgr::GetCurSerDrawExtendLine();}
	virtual HRESULT SetCurSerModulusK(float fK){return KG3DSerpentineMgr::SetCurSerModulusK(fK);}
	virtual HRESULT GetCurSerModulusK(float& fK){return KG3DSerpentineMgr::GetCurSerModulusK(fK);}
	virtual HRESULT SetCurSerName(const TCHAR* strName);
	virtual HRESULT GetCurSerName(std::string& strName){return KG3DSerpentineMgr::GetCurSerName(strName);}
	virtual HRESULT SetCurSerSegmentLength(int nLength){return KG3DSerpentineMgr::SetCurSerSegmentLength(nLength);}
	virtual HRESULT GetCurSerSegmentLength(int& nLength){return KG3DSerpentineMgr::GetCurSerSegmentLength(nLength);}
	virtual HRESULT MoveCurSerpentinePre(){return KG3DSerpentineMgr::MoveCurSerpentinePre();}
	virtual HRESULT MoveCurSerpentineNext(){return KG3DSerpentineMgr::MoveCurSerpentineNext();}
};

