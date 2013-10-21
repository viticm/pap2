////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DAnimationContainer.h
//  Version     : 1.0
//  Creator     : huanshan
//  Create Date : 2006-11-1 20:20:10
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DANIMATIONCONTAINER_H_
#define _INCLUDE_KG3DANIMATIONCONTAINER_H_

////////////////////////////////////////////////////////////////////////////////

class IEKG3DAnimation;
class IEKG3DClip;
class KG3DAnimationContainer
{
public:
	enum enuFillListBoxType
	{
		ENUM_FILL_WITH_ANINAME,
		ENUM_FILL_WITH_FILENAME,
	};

	KG3DAnimationContainer(void);
	~KG3DAnimationContainer(void);
	HRESULT LoadAnimationFromFile(const TCHAR* strFileName, DWORD dwOption);
	HRESULT GetAnimation(UINT uIndex, IEKG3DClip** ppClip); 
    HRESULT ReleaseAnimation(unsigned int uIndex);
	HRESULT ReleaseAnimation(IEKG3DClip* pClip);
	void CleanUp();

	//Communicate with UI
	void FillAnimationList(CListBox& ListBox, enuFillListBoxType Type);
	void FillAnimationComboBox(CComboBox& ComboBox);

private:
	std::vector<IEKG3DClip*> m_Clips;
};


#endif //_INCLUDE_KG3DANIMATIONCONTAINER_H_
