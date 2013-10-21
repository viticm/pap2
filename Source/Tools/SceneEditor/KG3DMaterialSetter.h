#pragma once


enum enuButton
{
	BUTTON_AMBIENT = 0,
	BUTTON_DIFFUSE,
	BUTTON_EMISSIVE,
	BUTTON_SPECULAR,
	//////////////////////////////////////////////////////////////////////////
	BUTTON_COUNT,
	BUTTON_FORCE_DWORD = 0xffffffff,
};

struct _MtlAlphaOperation;
interface IEKG3DMaterial;
struct MaterialOption
{
	BOOL bEnableZ;
	DWORD dwCullMode;
	BOOL bLighting;
	BOOL bHighLight;
	BOOL bHide;
	COLORREF crColors[BUTTON_COUNT];
	_MtlAlphaOperation* pAlphaOperation;
};

class KG3DMaterialSetter
{
public:
	KG3DMaterialSetter(void);
	~KG3DMaterialSetter(void);

	void SetByOption(const TCHAR* strFileName, 
		             const MaterialOption& Option,
					 BOOL bBackup, 
					 CListCtrl* pListBox);

private:
	void SetMaterial(const MaterialOption& Option);
	void SaveToFile(BOOL bBackup, CListCtrl* pListBox);

	HRESULT LoadMaterail(const TCHAR* strFileName);
	void SetMaterial(unsigned int uIndex, const MaterialOption& Option);

	IEKG3DMaterial *m_pMaterial;

};
