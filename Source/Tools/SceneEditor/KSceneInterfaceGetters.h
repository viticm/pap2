////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneInterfaceGetters.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-6-25 9:51:56
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KSCENEINTERFACEGETTERS_H_
#define _INCLUDE_KSCENEINTERFACEGETTERS_H_
//#include "SceneEditorCommon.h"
#include "IEKG3DTypes.h"
//#include "IEKG3DScene.h"
#include "KGCommonWidgets.h"
////////////////////////////////////////////////////////////////////////////////
namespace KSCENEEDITOR_HELPERCLASSES
{
	using namespace IEKG3DTYPE;
	/************************************************************************/
	/*为了避免模板代码膨胀用的辅助类，这些函数不是很快，不要放在循环的里面*/
	/************************************************************************/
	namespace PRIVATE
	{
		static VOID* PrivateGetEngineInterfaceCommon(DWORD dwType1, DWORD dwType2, DWORD_PTR wParam = 0, DWORD_PTR lParam = 0)
		{
			VOID* pInterface = NULL;
			HRESULT hr = g_pEngineManager->GetIEInterface(dwType1, dwType2, wParam, lParam, &pInterface);
			if (SUCCEEDED(hr))
			{
				_ASSERTE(pInterface);
				return pInterface;
			}
			return NULL;
		}
		static VOID* PrivateGetEngineInterface(DWORD dwType, DWORD_PTR wParam = 0, DWORD_PTR lParam = 0)
		{
			return PrivateGetEngineInterfaceCommon(NULL, dwType, wParam, lParam);
		}
		static VOID* PrivateGetEngineAbstractInterface(DWORD dwAbstractType, DWORD dwType, DWORD_PTR wParam = 0, DWORD_PTR lParam = 0)
		{
			return PrivateGetEngineInterfaceCommon(dwAbstractType, dwType, wParam, lParam);
		}
		static VOID* PrivateGetEngineCombinativeInterface(DWORD dwCombinativeType, DWORD dwType, DWORD_PTR wParam = 0, DWORD_PTR lParam = 0)
		{
			return PrivateGetEngineInterfaceCommon(dwCombinativeType, dwType, wParam, lParam);
		}
	};
	//////////////////////////////////////////////////////////////////////////
	template<DWORD dwType>
	inline typename TKG3DDWORD_TO_IEINTERFACE<NULL,dwType>::Result* GetEngineInterface(DWORD_PTR wParam = 0, DWORD_PTR lParam = 0)///wParam是第几个
	{
		typedef TKG3DDWORD_TO_IEINTERFACE<NULL,dwType> TypeInterfaceTrait;
		typedef typename TypeInterfaceTrait::Result* _Return_Type;
		_KG3D_DEBUG_STATIC_CHECK_T(false == TypeInterfaceTrait::bIsAbstract, Using_UnCorrect_Function);
		return static_cast<_Return_Type>(PRIVATE::PrivateGetEngineInterface(dwType, wParam, lParam));
	}
	//////////////////////////////////////////////////////////////////////////
	template<DWORD dwAbstractType>
	inline typename TKG3DDWORD_TO_IEINTERFACE<NULL,dwAbstractType>::Result* GetEngineAbstractInterface(DWORD dwSolidType, DWORD_PTR wParam = 0, DWORD_PTR lParam = 0)///wParam是第几个
	{
		typedef TKG3DDWORD_TO_IEINTERFACE<NULL,dwAbstractType> TypeInterfaceTrait;
		typedef typename TypeInterfaceTrait::Result* _Return_Type;
		_KG3D_DEBUG_STATIC_CHECK_T(true == TypeInterfaceTrait::bIsAbstract, Using_UnCorrect_Function);
		return static_cast<_Return_Type>(PRIVATE::PrivateGetEngineAbstractInterface(dwAbstractType, dwSolidType, wParam, lParam));
	}
	//////////////////////////////////////////////////////////////////////////	
	template<DWORD dwType1, DWORD dwType2>
	inline typename TKG3DDWORD_TO_IEINTERFACE<dwType1, dwType2>::Result* GetEngineCombinativeInterface(DWORD_PTR wParam = 0, DWORD_PTR lParam = 0)
	{
		typedef TKG3DDWORD_TO_IEINTERFACE<dwType1, dwType2> TypeLimitType;
		typedef typename TypeLimitType::Result* _Return_Type;
		return static_cast<_Return_Type>(PRIVATE::PrivateGetEngineCombinativeInterface(dwType1, dwType2, wParam, lParam));
	}
	//////////////////////////////////////////////////////////////////////////
	template<class _TypeInterface>
	inline _TypeInterface* GetEngineInterfaceEx(DWORD_PTR wParam = 0, DWORD_PTR lParam = 0)	//这个暴强啊
	{
		typedef TKG3DIEINTERFACE_TO_DWORD<_TypeInterface> TypeInterfaceTrait;
		return static_cast<_TypeInterface*>(PRIVATE::PrivateGetEngineInterfaceCommon(
			TypeInterfaceTrait::_type_dword1
			, TypeInterfaceTrait::_type_dword2
			, wParam
			, lParam
			));
	}
};
namespace KSH = KSCENEEDITOR_HELPERCLASSES;

#endif //_INCLUDE_KSCENEINTERFACEGETTERS_H_
