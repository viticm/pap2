////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DAutoTestInterface.h
//  Version     : 1.0
//  Creator     : Wu Caizhong
//  Create Date : 2009/6/15 9:42:44
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#ifdef KG3D_AUTO_TEST
#ifndef _INCLUDE_KG3DAUTOTESTINTERFACE_H_
#define _INCLUDE_KG3DAUTOTESTINTERFACE_H_

////////////////////////////////////////////////////////////////////////////////
class KG3DMeshTable;
class KG3DMaterialTable;

class IKG3DAutoTestInterface : public IUnknown
{
public:
	virtual KG3DMeshTable		*GetMeshTable() = 0;
	virtual KG3DMaterialTable	*GetMaterialTable() = 0;
};

extern "C" __declspec(dllexport) int CreateAutoTestInterface(
	IKG3DAutoTestInterface **ppiRetAutoTest
);

#endif //_INCLUDE_KG3DAUTOTESTINTERFACE_H_
#endif // #ifdef KG3D_AUTO_TEST
