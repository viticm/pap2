////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DModelSetEdit.h
//  Version     : 1.0
//  Creator     : huangshan
//  Create Date : 2006-11-6 20:23:00
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DMODELSETEDIT_H_
#define _INCLUDE_KG3DMODELSETEDIT_H_

////////////////////////////////////////////////////////////////////////////////
class IEKG3DModel;
class KG3DModelSetEdit
{
public:
	KG3DModelSetEdit(void);
	~KG3DModelSetEdit(void);

	void FilltModelSetList(CComboBox& ListBox, IEKG3DModel* pModel);
	void FilltModelSetList(CComboBox& ListBox,vector<IEKG3DModel*>*pvecModel);

	void FillBoneList(CTreeCtrl& TreeCtrl, IEKG3DModel* pModelST);
};


#endif //_INCLUDE_KG3DMODELSETEDIT_H_
