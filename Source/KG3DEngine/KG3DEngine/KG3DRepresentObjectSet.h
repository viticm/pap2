#pragma once
#include "kg3drepresentobject.h"
#include "IEKG3DRepresentObject.h"

class KG3DRepresentObjectSet :
	public IEKG3DRepresentObjectSet, public KG3DRepresentObject
{
private:
	struct ModelInfo
	{
		string szMeshName;
		string szMaterialName;
		string szAnimationName;
	};
	vector<ModelInfo>m_vecModelInfo;
	string m_szSetFileName;//集合的文件名
	int    m_ncurModel;//当前的模型文件
public:
	virtual HRESULT SaveToInformationBuffer(/*DWORD& dwType,DWORD& dwLength,*/KG3DMemoryFile* pFile);
	virtual HRESULT LoadFromInformationBuffer(/*DWORD dwType,DWORD dwLength,*/const KG3DMemoryFile* pFile,DWORD dwOption);

	HRESULT DeleteModelInfo(int nIndex);
	HRESULT RenderSelectMark();
	HRESULT GetModelSetIniFileName(LPSTR pFileName);

	virtual HRESULT ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);	//<PROCESS_MESSAGE>
	virtual HRESULT Copy(KG3DRepresentObject* pSrcObject);

	HRESULT SetCurrentModel(int nIndex,BOOL bForceLoad);
	int GetCurrentModel();

	HRESULT SaveToIniFile(LPCTSTR pFileName,DWORD dwOption);
	HRESULT LoadFromIniFile(LPCTSTR pFileName,DWORD dwOption);

	int GetNumModelofSet();
	HRESULT AddModelInfo(int& nIndex,LPSTR pMeshName,LPSTR pMtlName,LPSTR pAniName);
	HRESULT GetModelInfo(int nIndex,LPSTR pMeshName,LPSTR pMtlName,LPSTR pAniName);
    HRESULT IELoadFromIniFile(LPCTSTR pFileName,DWORD dwOption);
	KG3DRepresentObjectSet(void);
	virtual ~KG3DRepresentObjectSet(void);

	//(by dengzhihui 2006年9月28日 14:35:50
	virtual HRESULT ChangeModel(INT nChangeCode);
	virtual HRESULT GetIERepresentObject(IEKG3DRepresentObject** pObj);
	//)

	virtual HRESULT Load(LPCTSTR lpFileName, DWORD_PTR wParam, DWORD_PTR lParam);//这个用于绑定内部物件 

	virtual HRESULT GetAnotherIEInterface(DWORD dwType, VOID** ppAnotherInterface);
};
