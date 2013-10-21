// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__7D55B998_E029_4E82_B529_F8F56DAEA0B3__INCLUDED_)
#define AFX_STDAFX_H__7D55B998_E029_4E82_B529_F8F56DAEA0B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable:4100)
#pragma warning(disable:4189)
#pragma warning(disable:4127)
#pragma warning(disable:4238)

#include "KGStdAfx.h"

#undef ASSERT

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#define SAFE_RENDER(p)       { if (p) { p->Render(); } }
#define SAFE_DELETE(p)       { if (p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }

#include <vector>
#include <list>
#include <map>
#include <hash_map>
#include <set>
#include <algorithm>
#include <string>
//using namespace std;

#include "Engine.h"
#include "d3d9.h"
#include "dxerr.h"

#include "./resource.h"

#include "stdio.h"
#include <mmsystem.h>
#include <D3dx9math.h>
//#include "structs.h"
//#include "GraphiceEngine.h"	
//#include "GraphicTool.h"

//#include "DataObject.h"
//#include "ObjectTable.h"
//#include "TextureTable.h"
//#include "SceneTable.h"	
//#include "Scene.h"

#include <windows.h>

#include <afxdlgs.h>

#define PI  3.1415926f

#include "Engine/KGLog.h"
#include "KGPublic.h"
#include "SO3GlobalDef.h"
#include "Engine.h"
#include "../../KG3DEngine/KG3DEngine/KOutputDebug.h"	//<Refactor>这个文件在Engine中没有用到，却在引擎的目录中，以后是不是该移动
#include "../../KG3DEngine/KG3DEngine/KG3DStructs.h"	//<Refactor>这个文件在SceneEditor项目中被多次编译，放到预编译头比较合适
#include "../../KG3DEngine/KG3DEngine/KG3DToolsImpl.h"	//<Refactor>同上

#include "KGTester.h"

#include "KGCommonWidgets.h"
#include "KGHelperClasses.h"
#include "KGStringFunctions.h"
#include "KGStringFunctionsEx.h"
#include "KGMathFunctions.h"

#include "SceneEditorCommon.h" 
#include "KSceneInterfaceGetters.h"
//#include "KSceneHelperFuncs.h"

#define DEBUG_NEW_3DENGINE DEBUG_NEW	//有些SceneEditor的文件和Engine的文件重复，里面用了DEBUG_NEW_3DENGINE，MFC只要使用DEBUG_NEW分配内存就能自动获得内存检查
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__7D55B998_E029_4E82_B529_F8F56DAEA0B3__INCLUDED_)


