////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : 3dsMAXExport4Track.cpp
//  Version     : 1.0
//  Creator     : Zeb
//  Create Date : 2005-11-30 14:28:49
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "3dsMAXExport4Track.h"
#include "Util.h"
#include "resource.h"

////////////////////////////////////////////////////////////////////////////////

HINSTANCE g_hInstance;
int g_controlsInit = FALSE;

BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID lpvReserved) 
{
	g_hInstance = hinstDLL;

	if (!g_controlsInit) 
	{
		g_controlsInit = TRUE;
		InitCustomControls(g_hInstance);
		InitCommonControls();
	}

	return TRUE;
}

DWORD WINAPI fn(LPVOID arg)
{ 
	return(0); 
}


TrackExport::TrackExport()
{
}

BOOL TrackExport::SupportsOptions(int ext, DWORD options)
{
    assert(ext == 0);
    return(options == SCENE_EXPORT_SELECTED) ? TRUE : FALSE;
}

// 这个是主要的导出函数
int	TrackExport::DoExport(
    const TCHAR *name,
	ExpInterface *ei,
	Interface *i, 
	BOOL suppressPrompts, 
	DWORD options
)
{
	// 显示对话框，用户指定导出的一些参数
	/*if (
        !DialogBoxParam(
            g_hInstance, 
            MAKEINTRESOURCE(IDD_PANEL), 
            GetActiveWindow(), 
            TrackExportOptionsDlgProc, 
            (LPARAM)this
        ) 
    )
        return 0;*/

	this->ei = ei;
	this->i = i;

    m_nTrackCount = 0;
    m_strFileName = name;

	i->SetCommandPanelTaskMode(TASK_MODE_MODIFY);
	i->SetKeyStepsSelOnly(TRUE);
	
	int numChildren = i->GetRootNode()->NumberOfChildren();
	for (int idx = 0; idx < numChildren; idx++)
	{
		if(i->GetCancel())
			break;
		NodeEnum(i->GetRootNode()->GetChildNode(idx), 0);
	}
	
    if (m_nTrackCount == 1)
    {
        TCHAR szInfo[MAX_PATH];
        sprintf(szInfo, TEXT("轨道成功导出:%s"), m_strFileName.c_str());
        MSGBOXPARAMS msgBox;
        msgBox.cbSize = sizeof(MSGBOXPARAMS);
        msgBox.dwStyle = MB_USERICON | MB_OK;
        msgBox.hInstance = g_hInstance;
        msgBox.hwndOwner = ::GetActiveWindow();
        msgBox.lpszCaption = TEXT("3dMax Export");
        msgBox.lpszIcon = MAKEINTRESOURCE(IDI_ICON);
        msgBox.lpszText = szInfo;
        MessageBoxIndirect(&msgBox);
    }
    else if (m_nTrackCount > 1)
    {
        ::MessageBox(
            ::GetActiveWindow(),
            TEXT("你选中的虚拟体个数大于1"),
            TEXT("3dMax Export"),
            MB_OK | MB_ICONERROR
        );
    }
    else
    {
        ::MessageBox(
            ::GetActiveWindow(),
            TEXT("你没有选中任何的虚拟体"),
            TEXT("3dMax Export"),
            MB_OK | MB_ICONERROR
         );
    }
	return TRUE;
}

bool TrackExport::NodeEnum( INode* pNode, int indentLevel )
{
	// 结点没有被选中的话，就返回
	if( pNode->Selected() == FALSE )
		return false;

	// 用户点了取消
	if (i->GetCancel())
		return false;

	// If this node is a group head, all children are 
	// members of this group. The node will be a dummy node and the node name
	// is the actualy group name.
	if ( pNode->IsGroupHead() ) 
	{
		indentLevel++;
	}

	if( pNode->Selected() ) 
	{
		// The ObjectState is a 'thing' that flows down the pipeline containing
		// all information about the object. By calling EvalWorldState() we tell
		// max to eveluate the object at end of the pipeline.
		ObjectState os = pNode->EvalWorldState(0); 

		// The obj member of ObjectState is the actual object we will export.
		if (os.obj) 
		{
			// We look at the super class ID to determine the type of the object.
            DWORD ID = os.obj->SuperClassID();
			switch( os.obj->SuperClassID() ) 
			{
			case HELPER_CLASS_ID:
				ExportHelperObject(pNode, indentLevel); 
				break;
			}
		}
	}	

	// For each child of this node, we recurse into ourselves 
	// until no more children are found.
	for (int c = 0; c < pNode->NumberOfChildren(); c++) 
	{
		if (!NodeEnum(pNode->GetChildNode(c), indentLevel))
			return false;
	}

	// If thie is true here, it is the end of the group we started above.
	if (pNode->IsGroupHead()) 
	{
		indentLevel--;
	}

	return true;
}

void TrackExport::ExportHelperObject(INode* node, int indentLevel)
{
    Object* helperObj = node->EvalWorldState(0).obj;
    if (!helperObj)
        return;

    TSTR className;
    helperObj->GetClassName(className);
    if (className != TSTR("Dummy"))
        return;

    ObjectState os = node->EvalWorldState(0);
    if (!os.obj)
        return;

    Interval animRange = i->GetAnimRange();

    STrack track;
    DWORD NowFrame = 0;
    TimeValue Interval = GetTicksPerFrame();
    TimeValue t = animRange.Start();
    while (1)
    {
        Matrix3 mat3 = node->GetNodeTM(t);

        // here exchange Y and Z
        Point3 RowY = mat3.GetRow(1);
        Point3 RowZ = mat3.GetRow(2);
        mat3.SetRow(1, RowZ);
        mat3.SetRow(2, RowY);
        Point4 ColY = mat3.GetColumn(1);
        Point4 ColZ = mat3.GetColumn(2);
        mat3.SetColumn(1, ColZ);
        mat3.SetColumn(2, ColY);

        D3DXMATRIX matD3D;
        Matrix3ToMatrix4(matD3D, mat3);
        track.vecMatrix.push_back(matD3D);

        NowFrame++;
        t = Interval * NowFrame;

        if (t > animRange.End())
            break;
    }

    track.Head.dwFrameNum = NowFrame;
    track.Head.dwInterval = (DWORD)Interval;

    m_nTrackCount++;
    track.SaveToFile(m_strFileName.c_str());
}

BOOL CALLBACK TrackExportOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	static TrackExport *exp = NULL;

	switch(message)
	{
	case WM_INITDIALOG:
		{
			exp = (TrackExport *)lParam;
			CenterWindow(hWnd,GetParent(hWnd));
        }
        break;

    case WM_CLOSE:
		EndDialog(hWnd, 0);
		break;

	case WM_COMMAND:
		switch(LOWORD(wParam)) 
		{
		case IDC_BUTTON_OK:
            EndDialog(hWnd, 1);
			break;

		case IDC_BUTTON_CANCEL:
            {
                MessageBox(hWnd, "You want a piece of me, boy?", "", MB_OK);
                EndDialog(hWnd, 0);
            }
			break;
		}

	default:
		return FALSE;
	}

	return TRUE;
}

__declspec( dllexport ) const TCHAR* LibDescription() 
{
	return GetString(IDS_LIBDESCRIPTION);
}

/// MUST CHANGE THIS NUMBER WHEN ADD NEW CLASS 
__declspec( dllexport ) int LibNumberClasses() 
{
	return 1;
}


__declspec( dllexport ) ClassDesc* LibClassDesc(int i) 
{
	switch(i) {
	case 0: return &TrackExpDesc;
	default: return 0;
	}
}

__declspec( dllexport ) ULONG LibVersion() 
{
	return VERSION_3DSMAX;
}

// Let the plug-in register itself for deferred loading
__declspec( dllexport ) ULONG CanAutoDefer()
{
	return 1;
}
