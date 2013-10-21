//************************************************************************** 
//* Asciiexp.cpp	- Ascii File Exporter
//* 
//* By Christer Janson
//* Kinetix Development
//*
//* January 20, 1997 CCJ Initial coding
//*
//* This module contains the DLL startup functions
//*
//* Copyright (c) 1997, All Rights Reserved. 
//***************************************************************************

#include "header.h"
#include "asciiexp.h"
#include "Windows.h"
#include "ieditnormals.h"
#include "kg3dbiped.h"
#include "kg3dsk.h"
#include "KG3DMainCharactor.h"
#include "KG3DHelper.h"



using namespace std;

HINSTANCE hInstance;
int controlsInit = FALSE;

static BOOL showPrompts;
static BOOL exportSelected;

// Class ID. These must be unique and randomly generated!!
// If you use this as a sample project, this is the first thing
// you should change!
#define ASCIIEXP_CLASS_ID	Class_ID(0x175973dc, 0x520e153a)

extern Modifier* FindSkinModifier (INode* nodePtr);

KG_3D_EXPORT_ERROR::KG_3D_EXPORT_ERROR()
{
    m_hMainWnd = NULL;
}


HRESULT KG_3D_EXPORT_ERROR::Init(HWND hMainWnd, const CStr &cstrFileName)
{
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(hMainWnd);
    
    m_hMainWnd = hMainWnd;

    m_Strings.clear();
    m_Strings.reserve(32);

    m_strFileName = cstrFileName;

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG_3D_EXPORT_ERROR::UnInit()
{
    HRESULT hrResult = E_FAIL;
    int nRetCode     = false;
    CStr strTitle;
    CStr strErrorString;
    KG_STRING_VECTOR::iterator it;
    int nCount = 0;
    FILE *fpFile = NULL;

    KGLOG_PROCESS_ERROR(m_hMainWnd);

    KG_PROCESS_SUCCESS(m_Strings.size() == 0);   // not need to log

    for (it = m_Strings.begin(); it != m_Strings.end(); ++it)
    {
        strErrorString += (*it);
        strErrorString += "\n";
        nCount++;
        if (nCount > 20)
        {
            strErrorString += "...";
            break;
        }
    }

    strTitle.printf("Save Error Message to File \'%s\'", m_strFileName);


    nRetCode = MessageBox(
        m_hMainWnd,
        strErrorString,
        strTitle,
        MB_YESNO | MB_ICONERROR
    );
    KG_PROCESS_SUCCESS(nRetCode == IDNO);

    // if nRetCode == IDYES

    fpFile = fopen(m_strFileName, "w");
    KGLOG_PROCESS_ERROR(fpFile);


    for (it = m_Strings.begin(); it != m_Strings.end(); ++it)
    {
        fprintf(fpFile, "%s\n", *it);
    }


    m_Strings.clear();
    m_hMainWnd = NULL;
    m_strFileName = "";

Exit1:
    hrResult = S_OK;
Exit0:
    if (fpFile)
    {
        fclose(fpFile);
        fpFile = NULL;
    }

    return hrResult;
}

HRESULT KG_3D_EXPORT_ERROR::AddError(const CStr &cstrError)
{
    HRESULT hrResult = E_FAIL;

    m_Strings.push_back(cstrError);

    hrResult = S_OK;
//Exit0:
    return hrResult;
}




int g_LogInitFlag = false;

BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID lpvReserved) 
{
	hInstance = hinstDLL;

	// Initialize the custom controls. This should be done only once.
	if (!controlsInit) {
		controlsInit = TRUE;
		InitCustomControls(hInstance);
		InitCommonControls();
	}
	
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
	    {
	    //char szPath[MAX_PATH];
     //   OutputDebugString("DLL_PROCESS_ATTACH\r\n");
     //   KGLOG_PARAM LogParam;

     //   //g_SetRootPath();
     //   //g_GetRootPath(szPath);
     //   
     //   memset(&LogParam, 0, sizeof(LogParam));
	    //snprintf(LogParam.szPath,sizeof(LogParam.szPath),"%s/logs", szPath);
	    //LogParam.szPath[sizeof(LogParam.szPath) - 1] = '\0';
     //   strcpy(LogParam.szIdent, "KG_3D_MAX_EXPORT");
     //   LogParam.Options = (KGLOG_OPTIONS)(KGLOG_OPTION_FILE);
     //   LogParam.nMaxLineEachFile = 65536;

     //   //g_LogInitFlag = KGLogInit(LogParam, NULL);
        }
        break;
	case DLL_THREAD_ATTACH:
        OutputDebugString("DLL_THREAD_ATTACH\r\n");
        break;
	case DLL_THREAD_DETACH:
        OutputDebugString("DLL_THREAD_DETACH\r\n");
        break;
	case DLL_PROCESS_DETACH:
        OutputDebugString("DLL_PROCESS_DETACH\r\n");
        if (g_LogInitFlag)
        {
            //KGLogUnInit(NULL);
            g_LogInitFlag = false;
        }
        break;
	}
	
	
	return (TRUE);
}


__declspec(dllexport) const TCHAR* LibDescription() 
{
	return GetString(IDS_LIBDESCRIPTION);
}

/// MUST CHANGE THIS NUMBER WHEN ADD NEW CLASS 
__declspec(dllexport) int LibNumberClasses() 
{
	return 1;
}


__declspec(dllexport) ClassDesc* LibClassDesc(int i) 
{
	switch(i) {
	case 0: return GetAsciiExpDesc();
	default: return 0;
	}
}

__declspec(dllexport) ULONG LibVersion() 
{
	return VERSION_3DSMAX;
}

// Let the plug-in register itself for deferred loading
__declspec(dllexport) ULONG CanAutoDefer()
{
	return 1;
}

class AsciiExpClassDesc:public ClassDesc {
public:
	int				IsPublic() { return 1; }
	void*			Create(BOOL loading = FALSE) { return new AsciiExp; } 
	const TCHAR*	ClassName() { return GetString(IDS_ASCIIEXP); }
	SClass_ID		SuperClassID() { return SCENE_EXPORT_CLASS_ID; } 
	Class_ID		ClassID() { return ASCIIEXP_CLASS_ID; }
	const TCHAR*	Category() { return GetString(IDS_CATEGORY); }
};

static AsciiExpClassDesc AsciiExpDesc;


void printMat(const CMesh::_Matrix& mat, FILE* fp)
{
	assert(fp);
	fprintf(fp, "%f, %f, %f, %f\n%f, %f, %f, %f\n,%f, %f, %f, %f\n%f, %f, %f, %f\n", 
		        mat._11, mat._12, mat._13, mat._14, 
				mat._21, mat._22, mat._23, mat._24, 
				mat._31, mat._32, mat._33, mat._34, 
				mat._41, mat._42, mat._43, mat._44
			);
	fprintf(fp, "\n");
}


ClassDesc* GetAsciiExpDesc()
{
	return &AsciiExpDesc;
}

TCHAR *GetString(int id)
{
	static TCHAR buf[256];

	if (hInstance)
		return LoadString(hInstance, id, buf, sizeof(buf)) ? buf : NULL;

	return NULL;
}

AsciiExp::AsciiExp()
{
	// These are the default values that will be active when 
	// the exporter is ran the first time.
	// After the first session these options are sticky.
	m_bInch2CM = TRUE;

	bNormalizeSocketSize = FALSE;
	bVerticesPosition = TRUE;
	bVerticesNormal = TRUE;
	bVerticesDiffuse = FALSE;
	bTextureUVW1 = TRUE;
	bTextureUVW2 = FALSE;
	bTextureUVW3 = FALSE;
	
	bMapDiffuse = TRUE;
	bMapBump = FALSE;
	bMapSpecular = FALSE;
	bMapReflection = FALSE;
	
	bIncludeMesh = TRUE;
	bIncludeAnim = TRUE;
	bIncludeMtl =  TRUE;
	bIncludeMeshAnim =  FALSE;
	bIncludeCamLightAnim = FALSE;
	bIncludeIKJoints = FALSE;
	bIncludeNormals  =  FALSE;
	bIncludeTextureCoords = FALSE;
	bIncludeVertexColors = FALSE;
	bIncludeObjGeom = TRUE;
	bIncludeObjShape = TRUE;
	bIncludeObjCamera = TRUE;
	bIncludeObjLight = TRUE;
	bIncludeObjHelper = TRUE;
	bAlwaysSample = FALSE;
	nKeyFrameStep = 5;
	nMeshFrameStep = 5;
	nPrecision = 4;
	nStaticFrame = 0;

	bSaveMaterial = TRUE;
	bSaveAnimation = TRUE;
	bSaveMesh = TRUE;
	bUnitModel = TRUE;
	bExchangeYZ = TRUE;
	bDiffuseOnly = FALSE;
	bMoveTextureFile = TRUE;
	wsprintf(scTextureDir, "");
	dwAnimationType = ANIMATION_BONE_RTS;
	bIncludeUVAni = FALSE;
	wsprintf(scAnimationName,"Animation01");
	wsprintf(scTextureDir,"Texture\\");
}

AsciiExp::~AsciiExp()
{
	m_Bones.clear();
}

int AsciiExp::ExtCount()
{
	return 1;
}

const TCHAR * AsciiExp::Ext(int n)
{
	switch(n) {
	case 0:
		// This cause a static string buffer overwrite
		// return GetString(IDS_EXTENSION1);
		return _T("Mesh");
	}
	return _T("");
}

const TCHAR * AsciiExp::LongDesc()
{
	return GetString(IDS_LONGDESC);
}

const TCHAR * AsciiExp::ShortDesc()
{
	return GetString(IDS_SHORTDESC);
}

const TCHAR * AsciiExp::AuthorName() 
{
	return _T("YangLin");
}

const TCHAR * AsciiExp::CopyrightMessage() 
{
	return GetString(IDS_COPYRIGHT);
}

const TCHAR * AsciiExp::OtherMessage1() 
{
	return _T("");
}

const TCHAR * AsciiExp::OtherMessage2() 
{
	return _T("");
}

unsigned int AsciiExp::Version()
{
	return 100;
}

static INT_PTR CALLBACK AboutBoxDlgProc(HWND hWnd, UINT msg, 
	WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_INITDIALOG:
		CenterWindow(hWnd, GetParent(hWnd)); 
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			EndDialog(hWnd, 1);
			break;
		}
		break;
		default:
			return FALSE;
	}
	return TRUE;
}       

void AsciiExp::ShowAbout(HWND hWnd)
{
	//DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, AboutBoxDlgProc, 0);
}



static INT_PTR CALLBACK ExportFrontEndDlgProc(HWND hWnd, 
									          UINT msg,
									          WPARAM wParam, 
									          LPARAM lParam)
{
	
	switch(msg)	
	{
		case WM_INITDIALOG:
			CenterWindow(hWnd, GetParent(hWnd));
			switch(GetExportOption()->m_Type)
			{
			case ENUM_EXPORTTYPE_MAINCHARACTOR:
				CheckDlgButton(hWnd, IDC_RADIO_MAIN, BST_CHECKED);
				break;
			case ENUM_EXPORTTYPE_NORMAL:
				CheckDlgButton(hWnd, IDC_RADIO_NORMAL, BST_CHECKED);
				break;
			}
			break;
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDOK:
					EndDialog(hWnd, 1);
					break;
				case IDCANCEL:
					EndDialog(hWnd, 0);
					break;
			}
			break;
		default:
			return FALSE;
	}
	return TRUE;
}

const static DWORD dwIntervalLength = 20;
// Dialog proc
static INT_PTR CALLBACK ExportDlgProc(HWND hWnd, UINT msg,
	WPARAM wParam, LPARAM lParam)
{
	Interval animRange;
	ISpinnerControl  *spin;

	AsciiExp *exp = (AsciiExp*)GetWindowLongPtr(hWnd,GWLP_USERDATA); 
	switch (msg) {
	case WM_INITDIALOG:
		{
			exp = (AsciiExp*)lParam;
			SetWindowLongPtr(hWnd,GWLP_USERDATA,lParam); 
			CenterWindow(hWnd, GetParent(hWnd)); 
			CheckDlgButton(hWnd, IDC_MATERIAL, exp->GetIncludeMtl());
#ifndef DESIGN_VER

#endif // !DESIGN_VER

			//CheckRadioButton(hWnd, IDC_RADIO_USEKEYS, IDC_RADIO_SAMPLE, 
			//	exp->GetAlwaysSample() ? IDC_RADIO_SAMPLE : IDC_RADIO_USEKEYS);

			// Setup the spinner controls for the mesh definition sample rate 
			spin = GetISpinner(GetDlgItem(hWnd, IDC_MESH_STEP_SPIN)); 
			spin->LinkToEdit(GetDlgItem(hWnd,IDC_MESH_STEP), EDITTYPE_INT); 
			spin->SetLimits(1, 100, TRUE); 
			spin->SetScale(1.0f);
			spin->SetValue(exp->GetMeshFrameStep() ,FALSE);
			ReleaseISpinner(spin);

			// Setup the spinner control for the static frame#
			// We take the frame 0 as the default value
			animRange = exp->GetInterface()->GetAnimRange();
			spin = GetISpinner(GetDlgItem(hWnd, IDC_STATIC_FRAME_SPIN)); 
			spin->LinkToEdit(GetDlgItem(hWnd,IDC_STATIC_FRAME), EDITTYPE_INT); 
			spin->SetLimits(animRange.Start() / GetTicksPerFrame(), animRange.End() / GetTicksPerFrame(), TRUE); 
			spin->SetScale(1.0f);
			spin->SetValue(0, FALSE);
			ReleaseISpinner(spin);

			CheckDlgButton(hWnd, IDC_MESH, exp->GetSaveMesh()); 
			CheckDlgButton(hWnd, IDC_MATERIAL, exp->GetSaveMaterial()); 
			CheckDlgButton(hWnd, IDC_ANIMATION, exp->GetSaveAnimation());
			CheckDlgButton(hWnd, IDC_UNITMODEL, exp->GetUnitModel()); 
			CheckDlgButton(hWnd, IDC_EXCHANGEYZ, exp->GetExchangeYZ()); 
			CheckDlgButton(hWnd, IDC_INCH2CM, exp->m_bInch2CM); 
			CheckDlgButton(hWnd, IDC_CHECK_NORMALIZE_SS, exp->GetNormalizeSocket());
			CheckDlgButton(hWnd, IDC_CHECK_UVANI, exp->GetIncludeUVAni());

			CheckDlgButton(hWnd, IDC_VERTICESPOSITION, exp->GetSaveVerticesPosition()); 
			CheckDlgButton(hWnd, IDC_VERTICESNORMAL, exp->GetSaveVerticesNormal()); 
			CheckDlgButton(hWnd, IDC_VERTICESDIFFUSE, exp->GetSaveVerticesDiffuse()); 
			CheckDlgButton(hWnd, IDC_TEXTUREUVW1, exp->GetSaveTextureUVW1()); 
			CheckDlgButton(hWnd, IDC_TEXTUREUVW2, exp->GetSaveTextureUVW2()); 
			CheckDlgButton(hWnd, IDC_TEXTUREUVW3, exp->GetSaveTextureUVW3()); 

			EnableWindow(GetDlgItem(hWnd, IDC_VERTICESANIMATION), exp->GetSaveAnimation());
			EnableWindow(GetDlgItem(hWnd, IDC_BONEANIMATION), exp->GetSaveAnimation()); 
			EnableWindow(GetDlgItem(hWnd, IDC_ANIMATIONNAME), exp->GetSaveAnimation()); 

			EnableWindow(GetDlgItem(hWnd, IDC_MAPDIFFUSE), exp->GetSaveMaterial());
			EnableWindow(GetDlgItem(hWnd, IDC_MAPBUMP), exp->GetSaveMaterial());
			EnableWindow(GetDlgItem(hWnd, IDC_MAPSPECULAR), exp->GetSaveMaterial());
			EnableWindow(GetDlgItem(hWnd, IDC_MAPREFLECTION), exp->GetSaveMaterial());
			EnableWindow(GetDlgItem(hWnd, IDC_MOVETEXTUREFILE), exp->GetSaveMaterial()); 
			if(exp->GetMoveTextureFile()&&exp->GetSaveMaterial())
			{
				EnableWindow(GetDlgItem(hWnd, IDC_TEXTUREFILEDIR), exp->GetMoveTextureFile()); 
				EnableWindow(GetDlgItem(hWnd, IDC_SETTEXTUREDIR), exp->GetMoveTextureFile()); 
			}
			else
			{
				EnableWindow(GetDlgItem(hWnd, IDC_TEXTUREFILEDIR), FALSE); 
				EnableWindow(GetDlgItem(hWnd, IDC_SETTEXTUREDIR), FALSE); 
			}

			CheckDlgButton(hWnd, IDC_MAPDIFFUSE, exp->GetSaveMapDiffuse());
			CheckDlgButton(hWnd, IDC_MAPBUMP, exp->GetSaveMapBump());
			CheckDlgButton(hWnd, IDC_MAPSPECULAR, exp->GetSaveMapSpecular());
			CheckDlgButton(hWnd, IDC_MAPREFLECTION, exp->GetSaveMapReflection());
			CheckDlgButton(hWnd, IDC_MOVETEXTUREFILE, exp->GetMoveTextureFile()); 
			TCHAR strInterval[dwIntervalLength];
			sprintf_s(strInterval, 
				dwIntervalLength, 
				"%d",
				GetExportOption()->m_dwInterval);
			HWND hwndIntervel = GetDlgItem(hWnd, IDC_EDIT_INTERVEL);
			SetWindowText(hwndIntervel, strInterval);

			if(exp->GetAnimationType()==ANIMATION_VERTICES)
			{
				CheckRadioButton(hWnd, IDC_VERTICESANIMATION, IDC_RADIO_KEYONLY, 
					IDC_VERTICESANIMATION);
			}
			else if(exp->GetAnimationType()==ANIMATION_VERTICES_RELATIVE)
			{
				CheckRadioButton(hWnd, IDC_VERTICESANIMATION, IDC_RADIO_KEYONLY, 
					IDC_RELATEVERTICESANIMATION);
			}
			else if(exp->GetAnimationType()==ANIMATION_BONE)
			{
				CheckRadioButton(hWnd, IDC_VERTICESANIMATION, IDC_RADIO_KEYONLY, 
					IDC_BONEANIMATION);
			}
			else if(exp->GetAnimationType()==ANIMATION_BONE_RELATIVE)
			{
				CheckRadioButton(hWnd, IDC_VERTICESANIMATION, IDC_RADIO_KEYONLY, 
					IDC_RELATEBONEANIMATION);
			}
			else if(exp->GetAnimationType() == ANIMATION_BONE_RTS)
			{
				CheckRadioButton(hWnd, IDC_VERTICESANIMATION, IDC_RADIO_KEYONLY, IDC_RADIO_RTS);
			}
			else if(exp->GetAnimationType() == ANIMATION_BONE_KEYONLY)
			{
				CheckRadioButton(hWnd, IDC_VERTICESANIMATION, IDC_RADIO_KEYONLY, IDC_RADIO_KEYONLY);
			}


			{
				LPSTR pName = NULL;
				pName = exp->GetAnimationName(); 
				SetWindowText(GetDlgItem(hWnd, IDC_ANIMATIONNAME),pName);
			}
			{
				LPSTR pName = NULL;
				pName = exp->GetTextureDir(); 
				SetWindowText(GetDlgItem(hWnd, IDC_TEXTUREFILEDIR),pName);
			}
		}

		switch(GetExportOption()->m_Type)
		{
		case ENUM_EXPORTTYPE_MAINCHARACTOR:
			CheckDlgButton(hWnd, IDC_RADIO_MAIN, BST_CHECKED);
			break;
		case ENUM_EXPORTTYPE_NORMAL:
			CheckDlgButton(hWnd, IDC_RADIO_NORMAL, BST_CHECKED);
			break;
		}
		break;


	case CC_SPINNER_CHANGE:
		spin = (ISpinnerControl*)lParam; 
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_MESH:
			exp->SetSaveMesh(IsDlgButtonChecked(hWnd, IDC_MESH));
			// Enable / disable mesh options
			//EnableWindow(GetDlgItem(hWnd, IDC_VERTICESPOSITION), exp->GetSaveMesh());
			EnableWindow(GetDlgItem(hWnd, IDC_VERTICESNORMAL),   exp->GetSaveMesh()); 
			EnableWindow(GetDlgItem(hWnd, IDC_VERTICESDIFFUSE),  exp->GetSaveMesh());
			//EnableWindow(GetDlgItem(hWnd, IDC_TEXTUREUVW1), exp->GetSaveMesh());
			EnableWindow(GetDlgItem(hWnd, IDC_TEXTUREUVW2), exp->GetSaveMesh());
			EnableWindow(GetDlgItem(hWnd, IDC_TEXTUREUVW3), exp->GetSaveMesh());
			break;
		case IDC_MATERIAL:
			exp->SetSaveMaterial(IsDlgButtonChecked(hWnd, IDC_MATERIAL));
			EnableWindow(GetDlgItem(hWnd, IDC_MAPDIFFUSE), exp->GetSaveMaterial());
			EnableWindow(GetDlgItem(hWnd, IDC_MOVETEXTUREFILE), exp->GetSaveMaterial()); 
			EnableWindow(GetDlgItem(hWnd, IDC_MAPDIFFUSE), exp->GetSaveMaterial());
			EnableWindow(GetDlgItem(hWnd, IDC_MAPBUMP), exp->GetSaveMaterial());
			EnableWindow(GetDlgItem(hWnd, IDC_MAPSPECULAR), exp->GetSaveMaterial());
			EnableWindow(GetDlgItem(hWnd, IDC_MAPREFLECTION), exp->GetSaveMaterial());
			if(exp->GetMoveTextureFile()&&exp->GetSaveMaterial())
			{
				EnableWindow(GetDlgItem(hWnd, IDC_TEXTUREFILEDIR), exp->GetMoveTextureFile()); 
				EnableWindow(GetDlgItem(hWnd, IDC_SETTEXTUREDIR), exp->GetMoveTextureFile()); 
			}
			else
			{
				EnableWindow(GetDlgItem(hWnd, IDC_TEXTUREFILEDIR), FALSE); 
				EnableWindow(GetDlgItem(hWnd, IDC_SETTEXTUREDIR), FALSE); 
			}
			break;
		case IDC_ANIMATION:
			exp->SetSaveAnimation(IsDlgButtonChecked(hWnd, IDC_ANIMATION));
			EnableWindow(GetDlgItem(hWnd, IDC_VERTICESANIMATION), exp->GetSaveAnimation());
			EnableWindow(GetDlgItem(hWnd, IDC_BONEANIMATION), exp->GetSaveAnimation()); 
			EnableWindow(GetDlgItem(hWnd, IDC_RELATEVERTICESANIMATION), exp->GetSaveAnimation());
			EnableWindow(GetDlgItem(hWnd, IDC_RELATEBONEANIMATION), exp->GetSaveAnimation()); 
			EnableWindow(GetDlgItem(hWnd, IDC_ANIMATIONNAME), exp->GetSaveAnimation());
			EnableWindow(GetDlgItem(hWnd, IDC_RADIO_RTS), exp->GetSaveAnimation());
			break;
		case IDC_UNITMODEL:
			exp->SetUnitModel(IsDlgButtonChecked(hWnd, IDC_UNITMODEL));
			break;
		case IDC_EXCHANGEYZ:
			exp->SetExchangeYZ(IsDlgButtonChecked(hWnd, IDC_EXCHANGEYZ));
			break;
		case IDC_INCH2CM:
			exp->m_bInch2CM = IsDlgButtonChecked(hWnd, IDC_INCH2CM);
			break;
		case IDC_CHECK_NORMALIZE_SS:
			exp->SetNormalizeSocketSize(IsDlgButtonChecked(hWnd, IDC_CHECK_NORMALIZE_SS));
			break;
		case IDC_MAPDIFFUSE:
			exp->SetSaveMapDiffuse(IsDlgButtonChecked(hWnd, IDC_MAPDIFFUSE));
			break;
		case IDC_MAPBUMP:
			exp->SetSaveMapBump(IsDlgButtonChecked(hWnd, IDC_MAPBUMP));
			break;
		case IDC_MAPSPECULAR:
			exp->SetSaveMapSpecular(IsDlgButtonChecked(hWnd, IDC_MAPSPECULAR));
			break;
		case IDC_MAPREFLECTION:
			exp->SetSaveMapReflection(IsDlgButtonChecked(hWnd, IDC_MAPREFLECTION));
			break;

		case IDC_MOVETEXTUREFILE:
			exp->SetMoveTextureFile(IsDlgButtonChecked(hWnd, IDC_MOVETEXTUREFILE));
			if(exp->GetMoveTextureFile()&&exp->GetSaveMaterial())
			{
				EnableWindow(GetDlgItem(hWnd, IDC_TEXTUREFILEDIR), exp->GetMoveTextureFile()); 
				EnableWindow(GetDlgItem(hWnd, IDC_SETTEXTUREDIR), exp->GetMoveTextureFile()); 
			}
			else
			{
				EnableWindow(GetDlgItem(hWnd, IDC_TEXTUREFILEDIR), FALSE); 
				EnableWindow(GetDlgItem(hWnd, IDC_SETTEXTUREDIR), FALSE); 
			}
			break;
		case IDC_CHECK_UVANI:
			{
				exp->SetIncludeUVAni(TRUE);
			}
			break;
		case IDC_CHECK_MERGESUBSET:
			{
				GetExportOption()->m_bOptimizeSubsetForSpace = IsDlgButtonChecked(hWnd, IDC_CHECK_MERGESUBSET);
			}
			break;
		case IDC_VERTICESANIMATION:
			CheckRadioButton(hWnd, IDC_VERTICESANIMATION, IDC_RADIO_KEYONLY, 
				IDC_VERTICESANIMATION);
		if(IsDlgButtonChecked(hWnd, IDC_VERTICESANIMATION))
			{
				exp->SetAnimationType(ANIMATION_VERTICES);
			}
			break;
		case IDC_BONEANIMATION:
			CheckRadioButton(hWnd, IDC_VERTICESANIMATION, IDC_RADIO_KEYONLY, 
				IDC_BONEANIMATION);
			if(IsDlgButtonChecked(hWnd, IDC_BONEANIMATION))
			{
				exp->SetAnimationType(ANIMATION_BONE);
			}
			break;
		case IDC_RELATEVERTICESANIMATION:
			CheckRadioButton(hWnd, IDC_VERTICESANIMATION, IDC_RADIO_KEYONLY, 
				IDC_RELATEVERTICESANIMATION);
			if(IsDlgButtonChecked(hWnd, IDC_RELATEVERTICESANIMATION))
			{
				exp->SetAnimationType(ANIMATION_VERTICES_RELATIVE);
			}
			break;
		case IDC_RELATEBONEANIMATION:
			CheckRadioButton(hWnd, IDC_VERTICESANIMATION, IDC_RADIO_KEYONLY, 
				IDC_RELATEBONEANIMATION);
			if(IsDlgButtonChecked(hWnd, IDC_RELATEBONEANIMATION))
			{
				exp->SetAnimationType(ANIMATION_BONE_RELATIVE);
			}
			break;
		case IDC_RADIO_RTS:
			{
				CheckRadioButton(hWnd, IDC_VERTICESANIMATION, IDC_RADIO_KEYONLY, IDC_RADIO_RTS); 
				if(IsDlgButtonChecked(hWnd, IDC_RADIO_RTS))
				{
					exp->SetAnimationType(ANIMATION_BONE_RTS);
				}
			}
			break;
		case IDC_RADIO_KEYONLY:
			{
				CheckRadioButton(hWnd, IDC_VERTICESANIMATION, IDC_RADIO_KEYONLY, IDC_RADIO_KEYONLY); 
				if(IsDlgButtonChecked(hWnd, IDC_RADIO_KEYONLY))
				{
					exp->SetAnimationType(ANIMATION_BONE_KEYONLY);
				}
			}
			break;
		case IDC_VERTICESPOSITION:
			exp->SetSaveVerticesPosition(IsDlgButtonChecked(hWnd, IDC_VERTICESPOSITION));
			break;
		case IDC_VERTICESNORMAL:
			exp->SetSaveVerticesNormal(IsDlgButtonChecked(hWnd, IDC_VERTICESNORMAL));
			break;
		case IDC_VERTICESDIFFUSE:
			exp->SetSaveVerticesDiffuse(IsDlgButtonChecked(hWnd, IDC_VERTICESDIFFUSE));
			break;
		case IDC_TEXTUREUVW1:
			exp->SetSaveTextureUVW1(IsDlgButtonChecked(hWnd, IDC_TEXTUREUVW1));
			break;
		case IDC_TEXTUREUVW2:
			exp->SetSaveTextureUVW2(IsDlgButtonChecked(hWnd, IDC_TEXTUREUVW2));
			break;
		case IDC_TEXTUREUVW3:
			exp->SetSaveTextureUVW3(IsDlgButtonChecked(hWnd, IDC_TEXTUREUVW3));
			break;


		case IDC_ANIMATIONNAME:
			{
				TCHAR pName[256];
				GetWindowText(GetDlgItem(hWnd, IDC_ANIMATIONNAME),pName,256);
				exp->SetAnimationName(pName);
				break;
			}
		case IDC_TEXTUREFILEDIR:
			{
				TCHAR pName[256];
				GetWindowText(GetDlgItem(hWnd, IDC_TEXTUREFILEDIR),pName,256);
				exp->SetTextureDir(pName);
				break;
			}
		case  IDC_SETTEXTUREDIR:
			{
				/*char bigBuff[800] = "";  // maximum common dialog buffer size
				char szFilter[] ="Texture Files|*.bmp;*.jpg;*.png;*.tga;*.Spr;*.DDS|";
				
				CFileDialog dlg(true, NULL, NULL,
					OFN_HIDEREADONLY , szFilter);
				// Modify OPENFILENAME members directly to point to bigBuff
				dlg.m_ofn.lpstrFile = bigBuff;
				dlg.m_ofn.nMaxFile = sizeof(bigBuff);
				if(	dlg.DoModal() == IDOK)
				{	
					;
				}*/
				break;
				
			}
		case IDC_RADIO_MAIN:
			{
				IsDlgButtonChecked(hWnd, IDC_RADIO_MAIN);
				GetExportOption()->m_Type = ENUM_EXPORTTYPE_MAINCHARACTOR;
				break;
			}
		case IDC_RADIO_COMMON:
			{
				IsDlgButtonChecked(hWnd, IDC_RADIO_COMMON);
				GetExportOption()->m_Type = ENUM_EXPORTTYPE_NORMAL;
				break;
			}
		case IDOK:
			{
				//exp->SetIncludeMesh(IsDlgButtonChecked(hWnd, IDC_MESHDATA)); 
				//exp->SetIncludeAnim(IsDlgButtonChecked(hWnd, IDC_ANIMKEYS)); 
				TCHAR strInterval[dwIntervalLength];
				HWND hwndInterval = GetDlgItem(hWnd, IDC_EDIT_INTERVEL);
				GetWindowText(hwndInterval, strInterval, dwIntervalLength);
				GetExportOption()->m_dwInterval = static_cast<DWORD>(strtod(strInterval, '\0'));
				exp->SetIncludeMtl(IsDlgButtonChecked(hWnd, IDC_MATERIAL)); 
#ifndef DESIGN_VER

#endif // !DESIGN_VER

				spin = GetISpinner(GetDlgItem(hWnd, IDC_MESH_STEP_SPIN)); 
				exp->SetMeshFrameStep(spin->GetIVal());
				ReleaseISpinner(spin);

				spin = GetISpinner(GetDlgItem(hWnd, IDC_STATIC_FRAME_SPIN)); 
				exp->SetStaticFrame(spin->GetIVal() * GetTicksPerFrame());

				ReleaseISpinner(spin);

				EndDialog(hWnd, 1);
			}
			break;
		case IDCANCEL:
			EndDialog(hWnd, 0);
			break;
		}
		break;
		default:
			return FALSE;
	}
	return TRUE;
}       

// Dummy function for progress bar
DWORD WINAPI fn(LPVOID arg)
{
	return(0);
}

void nodeEnum(INode* pNode)
{
	int i = 0;
	int nNumChildren = pNode->NumberOfChildren();
	INode* pChild = NULL;

	TSTR NodeName = pNode->NodeName();
	for (i = 0; i < nNumChildren; i++)
	{
		pChild = pNode->GetChildNode(i);
		nodeEnum(pChild);
	}
}

HRESULT AsciiExp::DoTest(Interface* pIp, const TCHAR* strInFileName)
{
    HRESULT hrResult  = E_FAIL;
    HRESULT hrRetCode = E_FAIL;

	HRESULT hr = E_FAIL;
	TCHAR strPartFileName[MAX_PATH];
	TCHAR strMeshName[MAX_PATH];
	TCHAR strMtlName[MAX_PATH];
	INode* pRoot = pIp->GetRootNode();
    TSTR filename;
	TCHAR szRet[MAX_PATH];
	
	KG3DBiped Biped;
	KG3DMainCharactor Charactor(pIp);
	KG3DSK SK;
	Biped.SetIP(pIp);
	
	//SK.GetStandardSkeletonFromFile("d:\\sk.txt");
	//SK.OutputToFile("d:\\re sk.txt");
	
	//bExchangeYZ = FALSE;
    GetStdPlugsDir(szRet);
	Charactor.BeginExport(szRet);

	//here should be in KG3DMainCharactor
	int nNumNode = Charactor.GetNumNodes();
	
	strcpy(GetExportOption()->m_strAnimationName, GetAnimationName());
	for(int i = 0; i < nNumNode; i++)
	{
		CMesh* pMesh = NULL;
		hr = m_cMeshTable.Get1NewMesh(&pMesh);
		INode* pNode = Charactor.GetNode(i);

        hrRetCode = ExportMesh(pNode, pMesh, GetStaticFrame(), 0);
        KGLOG_COM_PROC_ERR_RET_ERR(hrRetCode);

		hrRetCode = ExportSkinInfo1(pNode, pMesh);
        KGLOG_COM_PROC_ERR_RET_ERR(hrRetCode);

		ExportMaterial(pNode, pMesh, 0);

		if (pMesh->m_bSkined)
		{
			for (DWORD j = 0; j < pMesh->m_dNumBone; j++)
			{
				INode* pBoneNode = pMesh->m_lpBoneInfo[j].pThis;
				Biped.GenerateSkeletonFromNode(pBoneNode, FALSE);
			}
		}
		FixSkinInfo(pNode, pMesh);
		Charactor.GetPartFilePath(i, strPartFileName);
		sprintf(strMeshName, "%s.mesh", strPartFileName);
		sprintf(strMtlName, "%s.mtl", strPartFileName);
		
		DWORD dwExportOption = 0;
		if(bVerticesPosition)
		{
			dwExportOption |= MESH_POSITION;
			dwExportOption |= MESH_FACESINDEX;
			dwExportOption |= MESH_SUBSETINDEX;
		}
		if(bVerticesNormal)
			dwExportOption |= MESH_NORMAL;
		if(bVerticesDiffuse)
			dwExportOption |= MESH_DIFFUSE;
		if(bTextureUVW1)
			dwExportOption |= MESH_TEXTUREUVW1;
		if(bTextureUVW2)
			dwExportOption |= MESH_TEXTUREUVW2;
		if(bTextureUVW3)
			dwExportOption |= MESH_TEXTUREUVW3;

		pMesh->SaveMesh(strMeshName, dwExportOption);
		TCHAR strDirName[MAX_PATH];
		TCHAR strTexturePath[MAX_PATH];

		Charactor.GetTexturePath(strTexturePath);
		//strcpy(strDirName, GetExportOption()->m_strTexturePath.c_str());
		
		if(strDirName[strlen(strDirName) - 1] != '\\')
		{
			strcat(strDirName, "\\");
		}

		//pMesh->GetRelativePath(strAbsPath, strDirName, strRelativeTexPath);
		pMesh->MoveTextureFile1(m_ip, strTexturePath);
		pMesh->SaveMaterial(strMtlName);
	}
	
	TCHAR strSaveFileName[MAX_PATH];
	
	Charactor.GetMdlFilePath(strSaveFileName);
	hr = Charactor.SaveToFile(strSaveFileName);
	
    GetStdPlugsDir(szRet);
    filename = szRet;
	filename += "\\";
	filename += "sk.txt";

	hr = Biped.LoadConfigFile(filename);
	if (FAILED(hr))
	{
		MessageBox(pIp->GetMAXHWnd(), "Load standard skeleton config file failed.", "错误", MB_OK);
	}

	Charactor.GetBipFilePath(strSaveFileName);
	Biped.OutputBipedText(strSaveFileName);
	
	Charactor.GetAnimationFilePath(strSaveFileName);
	Biped.ExportRTSAnimation(strSaveFileName);

	Charactor.EndExport();

	m_cMeshTable.CleanUp();
	//here should be in KG3DMainCharactor end
    
    hrResult = S_OK;
Exit0:
    return hrResult;
}


// Start the exporter!
// This is the real entrypoint to the exporter. After the user has selected
// the filename (and he's prompted for overwrite etc.) this method is called.
int AsciiExp::DoExport(const TCHAR *name,
					   ExpInterface *ei,
					   Interface *i, 
					   BOOL suppressPrompts, 
					   DWORD options) 
{
    int nResult = IMPEXP_FAIL;
    HRESULT hrRetCode = E_FAIL;
    int nExportErrorInitFlag = false;

	TCHAR path_buffer[MAX_PATH];
	TCHAR drive[MAX_PATH];
	TCHAR dir[MAX_PATH];
	TCHAR fname[MAX_PATH];
	TCHAR ext[MAX_PATH];
	TCHAR Name[MAX_PATH];
	TCHAR DirName[MAX_PATH];
	TCHAR strAbsPath[MAX_PATH];
	TCHAR strTexturePath[MAX_PATH];
	// Set a global prompt display switch
	showPrompts = suppressPrompts ? FALSE : TRUE;
	exportSelected = (options & SCENE_EXPORT_SELECTED) ? TRUE : FALSE;


	// Grab the interface pointer.
	m_ip = i;

	Interval AniRange = m_ip->GetAnimRange();
	//Add selected node into the set, as we are going to clear the interface's selection list
	for (int c = 0; c < m_ip->GetSelNodeCount(); c++)
	{
		m_SelectedNode.insert(m_ip->GetSelNode(c));
	}

	TSTR strMaxFile = m_ip->GetCurFileName();
	_splitpath(strMaxFile, NULL, NULL, scAnimationName, NULL);

	//DoTest(i, name);
	// Get the options the user selected the last time
	ReadConfig();

	if(showPrompts) {
		// Prompt the user with our dialogbox, and get all the options.
		if (!DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_ASCIIEXPORT_DLG),
			m_ip->GetMAXHWnd(), ExportDlgProc, (LPARAM)this)) 
            {
				goto Exit1;
			}
	}

	GetExportOption()->m_nStaticFrame = AniRange.Start();
	GetExportOption()->m_nKeyFrameStep = nMeshFrameStep;

    wsprintf(path_buffer,"%s",name);
    _splitpath(path_buffer, drive, dir, fname, ext);
    wsprintf(Name,"%s%s%s.Log",drive,dir,fname);

    hrRetCode = m_ExportError.Init(i->GetMAXHWnd(), Name);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    nExportErrorInitFlag = true;

	switch (GetExportOption()->m_Type)
	{
	case ENUM_EXPORTTYPE_MAINCHARACTOR:
		{
			hrRetCode = DoTest(i, name);
            KGLOG_COM_PROCESS_ERROR(hrRetCode);
			break;
		}
	case ENUM_EXPORTTYPE_NORMAL:
		{
			i->SetCommandPanelTaskMode(TASK_MODE_MODIFY);
			sprintf(szFmtStr, "%%4.%df", nPrecision);

			m_ip->SetKeyStepsSelOnly(TRUE);
			// Open the stream
			//pStream = _tfopen(name,_T("wt"));
			/*if (!pStream) {
			goto Exit0;
			}*/

			// Startup the progress bar.
			//m_ip->ProgressStart(GetString(IDS_PROGRESS_MSG), TRUE, fn, NULL);

			// Get a total node count by traversing the scene
			// We don't really need to do this, but it doesn't take long, and
			// it is nice to have an accurate progress bar.
			nTotalNodeCount = 0;
			nCurNode = 0;
			PreProcess(m_ip->GetRootNode(), nTotalNodeCount);

			//////////////////////////////////////////////////////////////////////////
			//4-13-2005 Huangshan Add for EditNormals 
			//Might cause some issue here
			//  1. Node selection collection lost


			//m_ip->SetCommandPanelTaskMode(TASK_MODE_MODIFY);//Needed for get IEditNormalsMod interface

			//m_ip->ClearNodeSelection();

			//4-13-2005 Huangshan Add for EditNormals ends
			//////////////////////////////////////////////////////////////////////////

			// First we write out a file header with global information. 
			//ExportGlobalInfo();

			// Export list of material definitions
			//ExportMaterialList();

			int numChildren = m_ip->GetRootNode()->NumberOfChildren();

			PreFindBones();

			// Call our node enumerator.
			// The nodeEnum function will recurse into itself and 
			// export each object found in the scene.

			
			//for (int idx=0; idx<numChildren; idx++) {
			//	if (m_ip->GetCancel())
			//		break;
			//	nodeEnum(m_ip->GetRootNode()->GetChildNode(idx), 0);
			//}
         /*   if (m_ip->GetCancel())
            {
                switch(MessageBox(m_ip->GetMAXHWnd(), _T("Really Cancel"),
                    _T("Question"), MB_ICONQUESTION | MB_YESNO)) {
                 case IDYES:break;
                 case IDNO:m_ip->SetCancel(FALSE);
                }
            }*/
            //if (!m_ip->GetCancel())
           // {
            for (KG_BONE_CONTAINER::iterator i = m_SelectedNode.begin(); i != m_SelectedNode.end(); i++)
            {
			    nodeEnum((*i), 0);
	    	}
           // }

			// We're done. Finish the progress bar.
			//m_ip->ProgressEnd();

			m_ip->ClearNodeSelection();
			set<INode*>::iterator it = m_SelectedNode.begin();
			while (it != m_SelectedNode.end())
			{
				m_ip->SelectNode(*it, 0);
				it++;
			}
			m_SelectedNode.clear();


			//保存转换后的模型文件
			DWORD Option =0;
			if(bVerticesPosition)
			{
				Option |= MESH_POSITION;
				Option |= MESH_FACESINDEX;
				Option |= MESH_SUBSETINDEX;
			}
			if(bVerticesNormal)
				Option |= MESH_NORMAL;
			if(bVerticesDiffuse)
				Option |= MESH_DIFFUSE;
			if(bTextureUVW1)
				Option |= MESH_TEXTUREUVW1;
			if(bTextureUVW2)
				Option |= MESH_TEXTUREUVW2;
			if(bTextureUVW3)
				Option |= MESH_TEXTUREUVW3;


			if(GetUnitModel())
			{
				DWORD UnitedModelID = 0;
				if(SUCCEEDED(m_cMeshTable.UnitMesh(&UnitedModelID)))
				{
					LPMESH pMesh = NULL;
					if(FAILED(m_cMeshTable.GetMesh(&pMesh,UnitedModelID)))
						goto Exit0;

					pMesh->CheckUpMaterial();
					if(GetSaveMesh())
					{
						wsprintf(path_buffer,"%s",name);
						_splitpath(path_buffer, drive, dir, fname, ext);
						wsprintf(Name,"%s%s%s.Mesh",drive,dir,fname);

						if(FAILED(pMesh->SaveMesh(Name,Option)))
							goto Exit0;
					}
					if(GetSaveMaterial())
					{
						wsprintf(path_buffer,"%s",name);
						_splitpath(path_buffer, drive, dir, fname, ext);
						wsprintf(Name,"%s%s%s.Mtl",drive,dir,fname);
						pMesh->GetAbsPath(path_buffer, strAbsPath);
						if(GetMoveTextureFile())
						{
							wsprintf(DirName,"%s", GetTextureDir());
							if(DirName[strlen(DirName)-1] != '\\')
							{
								strcat(DirName, "\\");
							}
							pMesh->GetRelativePath(strAbsPath, DirName, strTexturePath);
							pMesh->MoveTextureFile(m_ip, strTexturePath, strAbsPath);
						}
						if(FAILED(pMesh->SaveMaterial(Name)))
							goto Exit0;
					}
					if(GetSaveAnimation()) 
					{
						LPSTR strAnimationName = GetAnimationName();
						wsprintf(path_buffer,"%s",name);
						_splitpath(path_buffer, drive, dir, fname, ext);
						if (strAnimationName[0] != NULL)
						{
							wsprintf(Name,"%s%s%s.%s.ani",drive,dir,fname,GetAnimationName());
						}
						else
						{
							sprintf_s(Name, "%s%s%s.ani", drive, dir, fname);
						}
						if(FAILED(pMesh->SaveAnimation(Name,GetAnimationName())))
							goto Exit0;
					}
					/*KG3DBiped Biped;
					Biped.SetIP(m_ip);
					Biped.GenerateSkeletonFromMesh(pMesh);
					Biped.OutputBipedText("d:\\old mesh bip.txt");*/
				}

			}
			else
			{
				for(DWORD i_Mesh=1;i_Mesh<=m_cMeshTable.m_dNumMesh;i_Mesh++)
				{
					LPMESH pMesh = NULL;
					if(FAILED(m_cMeshTable.GetMesh(&pMesh,i_Mesh)))
						goto Exit0;

					pMesh->CheckUpMaterial();

					if(GetSaveMesh())
					{
						wsprintf(path_buffer,"%s",name);
						_splitpath(path_buffer, drive, dir, fname, ext);
						wsprintf(Name,"%s%s%s.Mesh",drive,dir,fname);
						if(FAILED(pMesh->SaveMesh(Name,Option)))
							goto Exit0;
					}
					if(GetSaveMaterial())
					{
						wsprintf(path_buffer,"%s",name);
						_splitpath(path_buffer, drive, dir, fname, ext);
						wsprintf(Name,"%s%s%s.Mtl",drive,dir,fname);
						pMesh->GetAbsPath(path_buffer, strAbsPath);
						if(GetMoveTextureFile())
						{
							wsprintf(DirName,"%s", GetTextureDir());
							if(DirName[strlen(DirName)-1] != '\\')
							{
								strcat(DirName, "\\");
							}
							pMesh->GetRelativePath(strAbsPath, DirName, strTexturePath);
							pMesh->MoveTextureFile(m_ip, strTexturePath, strAbsPath);
						}

						if(FAILED(pMesh->SaveMaterial(Name)))
							goto Exit0;
					}
					if(GetSaveAnimation()) 
					{
						wsprintf(path_buffer,"%s",name);
						_splitpath(path_buffer, drive, dir, fname, ext);
						wsprintf(Name,"%s%s%s.%s.%s.Ani",drive,dir,fname,((INode*)pMesh->m_pNode)->GetName(),GetAnimationName());
						if(FAILED(pMesh->SaveAnimation(Name,GetAnimationName())))
							goto Exit0;
					}
				}
			}
			// Close the stream 
			//fclose(pStream);

			//清理数据对象
			m_cMeshTable.CleanUp(); 
			std::map<INode*, vector<INode*>*>::iterator itChildVec = m_NumrelinkChild.begin();
			while (itChildVec != m_NumrelinkChild.end())
			{
				itChildVec->second->clear();
				delete itChildVec->second;
				itChildVec++;
			}
			m_NumrelinkChild.clear();
			m_relink.clear();
			WriteConfig();
		}
		break;
	}

Exit1:

    nResult = IMPEXP_SUCCESS;

Exit0:

    if (nExportErrorInitFlag)
    {
        if (nResult != IMPEXP_SUCCESS)
        {
            m_ExportError.AddError("Error in DoExport, check Program Log");
        }

        m_ExportError.UnInit();
        nExportErrorInitFlag = false;
    }

	return IMPEXP_SUCCESS;
}

//int AsciiExp::DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options) 
//{
//	i->LoadFromFile("C:\\Documents and Settings\\huangshan\\桌面\\中原区\\zy_茶馆001\\1.max");
//	return 1;
//}
BOOL AsciiExp::nodeEnum1(INode* node)
{
	m_ip->SetCommandPanelTaskMode(TASK_MODE_MODIFY);
	//INode* node = m_ip->GetRootNode();
	
	m_ip->SelectNode(node);
	Object* pObj = node->GetObjOrWSMRef();
	if (pObj && (pObj->SuperClassID() == GEN_DERIVOB_CLASS_ID || pObj->SuperClassID() == DERIVOB_CLASS_ID))
	{
		IDerivedObject* pIObject = static_cast<IDerivedObject*>(pObj);
		for(int modIndex = pIObject->NumModifiers() - 1; modIndex >=0; modIndex--)
		{
			Modifier* pMod = pIObject->GetModifier(modIndex);
			if (pMod->ClassID() == Class_ID(0x4aa52ae3, 0x35ca1cde))
			{
				IEditNormalsMod* pEditNormalsMod = static_cast<IEditNormalsMod*>(pMod->GetInterface(EDIT_NORMALS_MOD_INTERFACE));
				if (pEditNormalsMod->EnfnGetFaceDegree(0) != 3)
				{
					MessageBox(m_ip->GetMAXHWnd(), "Error", "", MB_OK);
				}
				break;
			}
		}
	}

	for (int c = node->NumberOfChildren() - 1; c >=0; c--)
	{
		INode* childNode = node->GetChildNode(c);
		nodeEnum1(childNode);
		
	}
	return TRUE;
}
BOOL AsciiExp::SupportsOptions(int ext, DWORD options) {
	assert(ext == 0);	// We only support one extension
	return(options == SCENE_EXPORT_SELECTED) ? TRUE : FALSE;
	}

// This method is the main object exporter.
// It is called once of every node in the scene. The objects are
// exported as they are encoutered.

// Before recursing into the children of a node, we will export it.
// The benefit of this is that a nodes parent is always before the
// children in the resulting file. This is desired since a child's
// transformation matrix is optionally relative to the parent.

BOOL AsciiExp::nodeEnum(INode* node, int indentLevel) 
{
	TCHAR Name[256];
	wsprintf(Name,"%s",node->GetName());
	//if(exportSelected && node->Selected() == FALSE)
	//	return TREE_CONTINUE;

	nCurNode++;
	//m_ip->ProgressUpdate((int)((float)nCurNode/nTotalNodeCount*100.0f)); 

	// Stop recursing if the user pressed Cancel 
	//if (m_ip->GetCancel())
	//	return FALSE;
	
	TSTR indent = GetIndent(indentLevel);
	
	// If this node is a group head, all children are 
	// members of this group. The node will be a dummy node and the node name
	// is the actualy group name.
	if (node->IsGroupHead()) {
		//fprintf(pStream,"%s%s \"%s\" {\n", indent.data(), ID_GROUP, FixupName(node->GetName())); 
		indentLevel++;
	}
	
	// Only export if exporting everything or it's selected
	if(!exportSelected || m_SelectedNode.count(node))//Modified if(!exportSelected || node->Selected()) {
	{
		

		// The ObjectState is a 'thing' that flows down the pipeline containing
		// all information about the object. By calling EvalWorldState() we tell
		// max to eveluate the object at end of the pipeline.
		ObjectState os = node->EvalWorldState(0); 

		// The obj member of ObjectState is the actual object we will export.
		if (os.obj) 
		{

			// We look at the super class ID to determine the type of the object.
			switch(os.obj->SuperClassID())
			{
			case GEOMOBJECT_CLASS_ID: 
				if (GetIncludeObjGeom()) ExportGeomObject(node, indentLevel); 
				break;
			case CAMERA_CLASS_ID:
				if (GetIncludeObjCamera()) ExportCameraObject(node, indentLevel); 
				break;
			case LIGHT_CLASS_ID:
				if (GetIncludeObjLight()) ExportLightObject(node, indentLevel); 
				break;
			case SHAPE_CLASS_ID:
				if (GetIncludeObjShape()) ExportShapeObject(node, indentLevel); 
				break;
			case HELPER_CLASS_ID:
				if (GetIncludeObjHelper()) ExportHelperObject(node, indentLevel); 
				break;
			}
		}
	


		// For each child of this node, we recurse into ourselves 
		// until no more children are found.
		for (int c = 0; c < node->NumberOfChildren(); c++) {
			if (!nodeEnum(node->GetChildNode(c), indentLevel))
				return FALSE;
		}
		
		// If thie is true here, it is the end of the group we started above.
		if (node->IsGroupHead()) {
			//fprintf(pStream,"%s}\n", indent.data());
			indentLevel--;
		}
	}
	return TRUE;
}


void AsciiExp::PreProcess(INode* node, int& nodeCount)
{
	nodeCount++;
	
	// Add the nodes material to out material list
	// Null entries are ignored when added...
	mtlList.AddMtl(node->GetMtl());

	// For each child of this node, we recurse into ourselves 
	// and increment the counter until no more children are found.
	for (int c = 0; c < node->NumberOfChildren(); c++) {
		PreProcess(node->GetChildNode(c), nodeCount);
	}
}

/****************************************************************************

 Configuration.
 To make all options "sticky" across sessions, the options are read and
 written to a configuration file every time the exporter is executed.

 ****************************************************************************/

TSTR AsciiExp::GetCfgFilename()
{
	TSTR filename;
	TCHAR szPath[MAX_PATH];
    GetStdPlugsDir(szPath);
	filename = szPath;
	filename += "\\";
	filename += CFGFILENAME;

	return filename;
}

// NOTE: Update anytime the CFG file changes
#define CFG_VERSION 0x03

BOOL AsciiExp::ReadConfig()
{
	TSTR filename = GetCfgFilename();
	FILE* cfgStream;

	cfgStream = fopen(filename, "rb");
	if (!cfgStream)
		return FALSE;

	// First item is a file version
	int fileVersion = _getw(cfgStream);

	if (fileVersion > CFG_VERSION) {
		// Unknown version
		fclose(cfgStream);
		return FALSE;
	}

	SetIncludeMesh(fgetc(cfgStream));
	SetIncludeAnim(fgetc(cfgStream));
	SetIncludeMtl(fgetc(cfgStream));
	SetIncludeMeshAnim(fgetc(cfgStream));
	SetIncludeCamLightAnim(fgetc(cfgStream));
	SetIncludeIKJoints(fgetc(cfgStream));
	SetIncludeNormals(fgetc(cfgStream));
	SetIncludeTextureCoords(fgetc(cfgStream));
	SetIncludeObjGeom(fgetc(cfgStream));
	SetIncludeObjShape(fgetc(cfgStream));
	SetIncludeObjCamera(fgetc(cfgStream));
	SetIncludeObjLight(fgetc(cfgStream));
	SetIncludeObjHelper(fgetc(cfgStream));
	SetAlwaysSample(fgetc(cfgStream));
	SetMeshFrameStep(_getw(cfgStream));
	SetKeyFrameStep(_getw(cfgStream));

	// Added for version 0x02
	if (fileVersion > 0x01) {
		SetIncludeVertexColors(fgetc(cfgStream));
	}

	// Added for version 0x03
	if (fileVersion > 0x02) {
		SetPrecision(_getw(cfgStream));
	}


	fclose(cfgStream);

	return TRUE;
}

void AsciiExp::WriteConfig()
{
	TSTR filename = GetCfgFilename();
	FILE* cfgStream;

	cfgStream = fopen(filename, "wb");
	if (!cfgStream)
		return;

	// Write CFG version
	_putw(CFG_VERSION,				cfgStream);

	fputc(GetIncludeMesh(),			cfgStream);
	fputc(GetIncludeAnim(),			cfgStream);
	fputc(GetIncludeMtl(),			cfgStream);
	fputc(GetIncludeMeshAnim(),		cfgStream);
	fputc(GetIncludeCamLightAnim(),	cfgStream);
	fputc(GetIncludeIKJoints(),		cfgStream);
	fputc(GetIncludeNormals(),		cfgStream);
	fputc(GetIncludeTextureCoords(),cfgStream);
	fputc(GetIncludeObjGeom(),		cfgStream);
	fputc(GetIncludeObjShape(),		cfgStream);
	fputc(GetIncludeObjCamera(),	cfgStream);
	fputc(GetIncludeObjLight(),		cfgStream);
	fputc(GetIncludeObjHelper(),	cfgStream);
	fputc(GetAlwaysSample(),		cfgStream);
	_putw(GetMeshFrameStep(),		cfgStream);
	_putw(GetKeyFrameStep(),		cfgStream);
	fputc(GetIncludeVertexColors(),	cfgStream);
	_putw(GetPrecision(),			cfgStream);

	fclose(cfgStream);
}


BOOL MtlKeeper::AddMtl(Mtl* mtl)
{
	if (!mtl) {
		return FALSE;
	}

	int numMtls = mtlTab.Count();
	for (int i=0; i<numMtls; i++) {
		if (mtlTab[i] == mtl) {
			return FALSE;
		}
	}
	mtlTab.Append(1, &mtl, 25);

	return TRUE;
}

int MtlKeeper::GetMtlID(Mtl* mtl)
{
	int numMtls = mtlTab.Count();
	for (int i=0; i<numMtls; i++) {
		if (mtlTab[i] == mtl) {
			return i;
		}
	}
	return -1;
}

int MtlKeeper::Count()
{
	return mtlTab.Count();
}

Mtl* MtlKeeper::GetMtl(int id)
{
	return mtlTab[id];
}

void Swap(float* A,float* B)
{
	float temp = *A;
	*A = *B;
	*B = temp;
}

Matrix3 ExchangeMatrixYZ(Matrix3* pMat)
{
	Matrix3 Temp;
	float S = 0;
	Point3 Row;
	
	Row = pMat->GetRow(0);
	Swap(&Row.y,&Row.z); 
	Temp.SetRow(0,Row);

	Row = pMat->GetRow(1);
	Swap(&Row.y,&Row.z); 
	Temp.SetRow(1,Row);

	Row = pMat->GetRow(2);
	Swap(&Row.y,&Row.z); 
	Temp.SetRow(2,Row);

	Row = pMat->GetRow(3);
	Swap(&Row.y,&Row.z); 
	Temp.SetRow(3,Row);

	return Temp;
}
void Matrix3ToMatrix4(CMesh::LPMATRIX pMat4,Matrix3* pMat3)
{
	Point3 Row = pMat3->GetRow(0);
	pMat4->_11 = Row.x;	pMat4->_12 = Row.y;	pMat4->_13 = Row.z;	pMat4->_14 = 0;
	Row = pMat3->GetRow(1);
	pMat4->_21 = Row.x;	pMat4->_22 = Row.y;	pMat4->_23 = Row.z;	pMat4->_24 = 0;
	Row = pMat3->GetRow(2);
	pMat4->_31 = Row.x;	pMat4->_32 = Row.y;	pMat4->_33 = Row.z;	pMat4->_34 = 0;
	Row = pMat3->GetRow(3);
	pMat4->_41 = Row.x;	pMat4->_42 = Row.y;	pMat4->_43 = Row.z;	pMat4->_44 = 1;
}

void Matrix3ToMatrix4(D3DXMATRIX& Mat, const Matrix3& Mat3)
{
	Point3 Row = Mat3.GetRow(0);
	Mat.m[0][0] = Row.x;	Mat.m[0][1] = Row.y;	Mat.m[0][2] = Row.z;	Mat.m[0][3] = 0;
	Row = Mat3.GetRow(1);
	Mat.m[1][0] = Row.x;	Mat.m[1][1] = Row.y;	Mat.m[1][2] = Row.z;	Mat.m[1][3] = 0;
	Row = Mat3.GetRow(2);
	Mat.m[2][0] = Row.x;	Mat.m[2][1] = Row.y;	Mat.m[2][2] = Row.z;	Mat.m[2][3] = 0;
	Row = Mat3.GetRow(3);
	Mat.m[3][0] = Row.x;	Mat.m[3][1] = Row.y;	Mat.m[3][2] = Row.z;	Mat.m[3][3] = 1;

}

HRESULT AsciiExp::ExportBoneAnimation(INode *node, LPMESH pMesh)
{
	ObjectState os = node->EvalWorldState(GetStaticFrame());
	HRESULT hr = S_OK;
	if (!os.obj)
	{
		return hr;
	}
	// Get animation range
	Interval animRange = m_ip->GetAnimRange();
	// Get validity of the object
	Interval objRange = os.obj->ObjectValidity(GetStaticFrame());

	
	// If the animation range is not fully included in the validity
	// interval of the object, then we're animated.
	TimeValue F = GetTicksPerFrame();
	TimeValue FF = GetMeshFrameStep();

	if (!objRange.InInterval(animRange))
	{
		
		DWORD NumFrame = 0;
				
		TimeValue t = animRange.Start();
		//这个计算过程看起来如此复杂，不如让它试运行一次，看看到底多少桢
		while (1) {
			// This may seem strange, but the object in the pipeline
			// might not be valid anymore.
			os = node->EvalWorldState(t);
			objRange = os.obj->ObjectValidity(t);
			t = objRange.Start() < animRange.Start() ? animRange.Start() : objRange.Start();
			
			// Export the mesh definition at this frame
			//ExportMesh(node, pMesh,t, indentLevel+1);
			NumFrame++;
			if (objRange.End() >= animRange.End()) {
				break;
			}
			else {
				t = (objRange.End()/GetTicksPerFrame()+GetMeshFrameStep()) * GetTicksPerFrame();
			}
		}
		//申请内存，
		
		Matrix3*  m_lpAnimationBone = new Matrix3[pMesh->m_dNumBone * NumFrame];
		ZeroMemory(m_lpAnimationBone ,sizeof(Matrix3)*pMesh->m_dNumBone*NumFrame);

		DWORD NowFrame = 0;
		t = animRange.Start();
		while (1) {
			// This may seem strange, but the object in the pipeline
			// might not be valid anymore.
			os = node->EvalWorldState(t);
			objRange = os.obj->ObjectValidity(t);
			t = objRange.Start() < animRange.Start() ? animRange.Start() : objRange.Start();
			
		
			// Export the mesh definition at this frame
			for(DWORD i=0;i<pMesh->m_dNumBone;i++)
			{
				CMesh::LPBONEINFO pBoneInfo = &pMesh->m_lpBoneInfo[i];
				INode* IBoneNode = pBoneInfo->pThis;
				Matrix3 tm = IBoneNode->GetObjTMAfterWSM(t);
				if(m_bInch2CM)
				{
					Matrix3 Scale;
					Scale.SetScale(Point3(2.54f,2.54f,2.54f));
					tm *= Scale;
				}
				if(GetExchangeYZ())
				{
					tm = ExchangeMatrixYZ(&tm);
				}
				m_lpAnimationBone[i*NumFrame+NowFrame] = tm;
			}
			
			if (objRange.End() >= animRange.End()) {
				break;
			}
			else {
				t = (objRange.End()/GetTicksPerFrame()+GetMeshFrameStep()) * GetTicksPerFrame();
			}
			NowFrame++;
		}
		
		//现在把整个动画过程一动不动的骨骼去掉

		DWORD  NumAnimatedBone = 0;
		BOOL * BoneAnimated = new BOOL[pMesh->m_dNumBone];
		ZeroMemory(BoneAnimated,sizeof(BOOL)*pMesh->m_dNumBone);

		for(DWORD i=0;i<pMesh->m_dNumBone;i++)
		{
			Matrix3 FirstMat;
			FirstMat = m_lpAnimationBone[i*NumFrame];
			for(DWORD n=1;n<NumFrame;n++)
			{
				Matrix3 v = m_lpAnimationBone[i*NumFrame+n];
				if(!(v==FirstMat))
				{
					NumAnimatedBone++;
					BoneAnimated[i] = TRUE;
					break;
				}
			}
		}
		
		//把结果保存到MESH中
	
		pMesh->m_dNumAnimationBones = NumAnimatedBone;
		if(NumAnimatedBone!=0)
		{
			pMesh->m_dNumFrames = NumFrame;
			pMesh->m_fFrameLength = GetTicksPerFrame();

			CMesh::LPMATRIX SrcAnimationBoneTran = new CMesh::MATRIX[NumAnimatedBone*NumFrame];
			//if(SrcAnimationBoneTran==NULL) return E_FAIL;
			ZeroMemory(SrcAnimationBoneTran ,sizeof(CMesh::MATRIX)*NumAnimatedBone*NumFrame);
			
			LPSTR*   SrcAnimationBoneName = new LPSTR[NumAnimatedBone];
			ZeroMemory(SrcAnimationBoneName ,sizeof(LPSTR)*NumAnimatedBone);
			
			DWORD K=0;
			for(i=0;i<pMesh->m_dNumBone;i++)
			{
				CMesh::LPBONEINFO pBoneInfo = &pMesh->m_lpBoneInfo[i];
				if(BoneAnimated[i])
				{
					SrcAnimationBoneName[K] = (LPSTR)pBoneInfo->pThis;
					for(DWORD n=0;n<NumFrame;n++)
					{
						Matrix3* Mat3 = &m_lpAnimationBone[i*NumFrame+n];
						CMesh::LPMATRIX Mat4 = &SrcAnimationBoneTran[K*NumFrame+n];
						Matrix3ToMatrix4(Mat4,Mat3);
					}
					K++;
				}
			}
			pMesh->m_lpAnimationBonesTrans = (CMesh::LPMATRIX) SrcAnimationBoneTran;
			pMesh->m_lpAnimationBonesName = SrcAnimationBoneName;
		}
		//释放内存
		delete [] m_lpAnimationBone;
		delete [] BoneAnimated;

		//把数据中的INode* 转换为字符串，长度为３０
		FixSkinInfo(node, pMesh);
		for(int i = 0; i < pMesh->m_dNumAnimationBones; i++)
		{
			INode* pBoneNode = (INode*) pMesh->m_lpAnimationBonesName[i];
			pMesh->m_lpAnimationBonesName[i] = new TCHAR[MAX_BONE_NAME_LENGTH];
			if(pBoneNode)
				wsprintf(pMesh->m_lpAnimationBonesName[i],"%s",pBoneNode->GetName());
			else
				wsprintf(pMesh->m_lpAnimationBonesName[i],"");
		}
		pMesh->m_dwAnimationType = ANIMATION_BONE;

		//////////////////////////////////////////////////////////////////////////
		//debug
		/*FILE * pF = fopen("Skin.txt","at");
		fprintf(pF,"================= Skinning Bones =================\n");
		for(i=0;i<pMesh->m_dNumBone;i++)
		{
			CMesh::LPBONEINFO pBoneInfo = &pMesh->m_lpBoneInfo[i];
			fprintf(pF,"%d  Bone: %s has %d Children  Parent is : %s   \n",i,
				pBoneInfo->Name,pBoneInfo->NumChild,
				pBoneInfo->ParentName);
			for(DWORD K=0;K<pBoneInfo->NumChild;K++)
			{
				fprintf(pF,"     %d Children is %s \n",K,
					pBoneInfo->lpChildNames[K]);
			}
		}
		fprintf(pF,"================= Animationed Bones =================\n");
		for(i=0;i<pMesh->m_dNumAnimationBones;i++)
		{
			fprintf(pF,"%d  Bone: %s  \n",i,pMesh->m_lpAnimationBonesName[i]);
		}
		fclose(pF);*/
	
		hr = S_OK;

	}
	else
	{
		hr = FixSkinInfo(node, pMesh);
	}
	return hr;
}

HRESULT AsciiExp::ExportRelateBoneAnimation(INode *node, LPMESH pMesh)
{
	ObjectState os = node->EvalWorldState(GetStaticFrame());
	if (!os.obj)
		return S_OK;//
		
	// Get animation range
	Interval animRange = m_ip->GetAnimRange();
	// Get validity of the object
	Interval objRange = os.obj->ObjectValidity(GetStaticFrame());

	
	// If the animation range is not fully included in the validity
	// interval of the object, then we're animated.
	TimeValue F = GetTicksPerFrame();
	TimeValue FF = GetMeshFrameStep();

	if (!objRange.InInterval(animRange)) 
	{
		
		DWORD NumFrame = 0;
				
		TimeValue t = animRange.Start();
		//这个计算过程看起来如此复杂，不如让它试运行一次，看看到底多少桢
		while (1)
		{
			// This may seem strange, but the object in the pipeline
			// might not be valid anymore.
			os = node->EvalWorldState(t);
			objRange = os.obj->ObjectValidity(t);
			t = objRange.Start() < animRange.Start() ? animRange.Start() : objRange.Start();
			
			// Export the mesh definition at this frame
			//ExportMesh(node, pMesh,t, indentLevel+1);
			NumFrame++;
			if (objRange.End() >= animRange.End()) {
				break;
			}
			else {
				t = (objRange.End()/GetTicksPerFrame()+GetMeshFrameStep()) * GetTicksPerFrame();
			}
		}
		//申请内存，
		
		Matrix3*  m_lpAnimationBone = new Matrix3[pMesh->m_dNumBone * NumFrame];
		ZeroMemory(m_lpAnimationBone ,sizeof(Matrix3)*pMesh->m_dNumBone*NumFrame);

		DWORD NowFrame = 0;
		t = animRange.Start();
		while (1)
		{
			// This may seem strange, but the object in the pipeline
			// might not be valid anymore.
			os = node->EvalWorldState(t);
			objRange = os.obj->ObjectValidity(t);
			t = objRange.Start() < animRange.Start() ? animRange.Start() : objRange.Start();
			
		
			// Export the mesh definition at this frame
			for(DWORD i=0;i<pMesh->m_dNumBone;i++)
			{
				CMesh::LPBONEINFO pBoneInfo = &pMesh->m_lpBoneInfo[i];
				INode* IBoneNode = pBoneInfo->pThis;
				
				Matrix3 Nodetm = IBoneNode->GetNodeTM(t);
				
				if(m_bInch2CM)
				{
					Matrix3 Scale;
					Scale.SetScale(Point3(2.54f,2.54f,2.54f));
					Nodetm *= Scale;
				}
						
				INode* INodeParent = IBoneNode->GetParentNode();
				if (INodeParent!=m_ip->GetRootNode())
				{
					
					Matrix3 NodeParent = INodeParent->GetNodeTM(t);
					if(m_bInch2CM)
					{
						Matrix3 Scale;
						Scale.SetScale(Point3(2.54f,2.54f,2.54f));
						NodeParent *= Scale;
					}
					/*if (GetExchangeYZ())
					{
						ParentMat = ExchangeMatrixYZ(&ParentMat);
					}*/
					Nodetm = Nodetm * Inverse(NodeParent);
		
				}
				else
				{
					static BOOL bChange = TRUE;
					if (bChange)
					{
						Nodetm = ExchangeMatrixYZ(&Nodetm);
					}
					
				}
				
				m_lpAnimationBone[i*NumFrame+NowFrame] = Nodetm;
			}
			
			if (objRange.End() >= animRange.End()) {
				break;
			}
			else {
				t = (objRange.End()/GetTicksPerFrame()+GetMeshFrameStep()) * GetTicksPerFrame();
			}
			NowFrame++;
		}
		
		//现在把整个动画过程一动不动的骨骼去掉

		DWORD  NumAnimatedBone = pMesh->m_dNumBone;
		/*BOOL * BoneAnimated = new BOOL[pMesh->m_dNumBone];
		ZeroMemory(BoneAnimated,sizeof(BOOL)*pMesh->m_dNumBone);

		for(DWORD i=0;i<pMesh->m_dNumBone;i++)
		{
			Matrix3 FirstMat;
			FirstMat = m_lpAnimationBone[i*NumFrame];
			for(DWORD n=1;n<NumFrame;n++)
			{
				Matrix3 v = m_lpAnimationBone[i*NumFrame+n];
				if(!(v==FirstMat))
				{
					NumAnimatedBone++;
					BoneAnimated[i] = TRUE;
					break;
				}
			}
		}*/
		
		//把结果保存到MESH中
		for (int iBoneIndex = 0; iBoneIndex < pMesh->m_dNumBone; iBoneIndex++)
		{
			for (int iFrameIndex = 0; iFrameIndex < NumFrame; iFrameIndex++)
			{
				for (int rowIndex = 0; rowIndex < 4; rowIndex++)
				{
					Point3 row = m_lpAnimationBone[iBoneIndex].GetRow(rowIndex);
		
				}
			}
		}
		
		pMesh->m_dNumAnimationBones = pMesh->m_dNumBone;
		if(pMesh->m_dNumAnimationBones!=0)
		{
			pMesh->m_dNumFrames = NumFrame;
			pMesh->m_fFrameLength = GetTicksPerFrame();

			CMesh::LPMATRIX SrcAnimationBoneTran = new CMesh::MATRIX[NumAnimatedBone*NumFrame];
			if(SrcAnimationBoneTran==NULL) return E_FAIL;
			ZeroMemory(SrcAnimationBoneTran ,sizeof(CMesh::MATRIX)*
				NumAnimatedBone*NumFrame);
			 
			LPSTR*   SrcAnimationBoneName = new LPSTR[NumAnimatedBone];
			ZeroMemory(SrcAnimationBoneName ,sizeof(LPSTR)*NumAnimatedBone);
			
			DWORD K=0;
			for(DWORD i=0;i<pMesh->m_dNumBone;i++)
			{
				CMesh::LPBONEINFO pBoneInfo = &pMesh->m_lpBoneInfo[i];
				
				SrcAnimationBoneName[K] = (LPSTR)pBoneInfo->pThis;
				for(DWORD n=0;n<NumFrame;n++)
				{
					int Index = i*NumFrame+n;
					Matrix3* Mat3 = &m_lpAnimationBone[Index];
					CMesh::LPMATRIX Mat4 = &SrcAnimationBoneTran[K*NumFrame+n];
					Matrix3ToMatrix4(Mat4,Mat3);
				
				}
				K++;
			}

			/*pMesh->m_lpAniBScaling     = SrcScaling;
			pMesh->m_lpAniBRotation    = SrcRotation;
			pMesh->m_lpAniBTranslation = SrcTranslation;*/

			pMesh->m_lpAnimationBonesTrans = (CMesh::LPMATRIX) SrcAnimationBoneTran;
			pMesh->m_lpAnimationBonesName = SrcAnimationBoneName;
		}
		
		//释放内存
		SAFE_DELETE_ARRAY(m_lpAnimationBone);

		//把数据中的INode* 转换为字符串，长度为３０
		FixSkinInfo(node, pMesh);
		for(int i = 0; i < pMesh->m_dNumAnimationBones; i++)
		{
			INode* pBoneNode = (INode*) pMesh->m_lpAnimationBonesName[i];
			pMesh->m_lpAnimationBonesName[i] = new TCHAR[MAX_BONE_NAME_LENGTH];
			if(pBoneNode)
				wsprintf(pMesh->m_lpAnimationBonesName[i],"%s",pBoneNode->GetName());
			else
				wsprintf(pMesh->m_lpAnimationBonesName[i],"");
		}
		pMesh->m_dwAnimationType = ANIMATION_BONE_RELATIVE;

	}
	else
	{
		FixSkinInfo(node, pMesh);;
	}

	return S_OK;
}

//HRESULT AsciiExp::ExportRelateBoneAnimationKeyOnly(INode *node, LPMESH pMesh)
//{
//	ObjectState os = node->EvalWorldState(GetStaticFrame());
//	if (!os.obj)
//		return S_OK;
//
//	// Get animation range
//	Interval animRange = m_ip->GetAnimRange();
//	// Get validity of the object
//	Interval objRange = os.obj->ObjectValidity(GetStaticFrame());
//
//
//	// If the animation range is not fully included in the validity
//	// interval of the object, then we're animated.
//	TimeValue F = GetTicksPerFrame();
//	TimeValue FF = GetMeshFrameStep();
//
//	if (!objRange.InInterval(animRange)) {
//
//		DWORD NumFrame = 0;
//
//		TimeValue t = animRange.Start();
//		//这个计算过程看起来如此复杂，不如让它试运行一次，看看到底多少桢
//		while (1) {
//			// This may seem strange, but the object in the pipeline
//			// might not be valid anymore.
//			os = node->EvalWorldState(t);
//			objRange = os.obj->ObjectValidity(t);
//			t = objRange.Start() < animRange.Start() ? animRange.Start() : objRange.Start();
//
//			// Export the mesh definition at this frame
//			//ExportMesh(node, pMesh,t, indentLevel+1);
//			NumFrame++;
//			if (objRange.End() >= animRange.End()) {
//				break;
//			}
//			else {
//				t = (objRange.End()/GetTicksPerFrame()+GetMeshFrameStep()) * GetTicksPerFrame();
//			}
//		}
//		//申请内存，
//
//		Matrix3*  m_lpAnimationBone = new Matrix3[pMesh->m_dNumBone * NumFrame];
//		ZeroMemory(m_lpAnimationBone ,sizeof(Matrix3)*pMesh->m_dNumBone*NumFrame);
//
//		DWORD NowFrame = 0;
//		t = animRange.Start();
//		while (1) {
//			// This may seem strange, but the object in the pipeline
//			// might not be valid anymore.
//			os = node->EvalWorldState(t);
//			objRange = os.obj->ObjectValidity(t);
//			t = objRange.Start() < animRange.Start() ? animRange.Start() : objRange.Start();
//
//
//			// Export the mesh definition at this frame
//			for(DWORD i=0;i<pMesh->m_dNumBone;i++)
//			{
//				CMesh::LPBONEINFO pBoneInfo = &pMesh->m_lpBoneInfo[i];
//				INode* IBoneNode = (INode*) pBoneInfo->Name;
//				Matrix3 tm = IBoneNode->GetObjTMAfterWSM(t);
//				if(m_bInch2CM)
//				{
//					Matrix3 Scale;
//					Scale.SetScale(Point3(2.54f,2.54f,2.54f));
//					tm *= Scale;
//				}
//				if (GetExchangeYZ())
//				{
//					tm = ExchangeMatrixYZ(&tm);
//				}
//
//				INode* INodeParent = IBoneNode->GetParentNode();
//				if (INodeParent!=m_ip->GetRootNode())
//				{
//					Matrix3 ParentMat = INodeParent->GetObjTMAfterWSM(t);
//					if(m_bInch2CM)
//					{
//						Matrix3 Scale;
//						Scale.SetScale(Point3(2.54f,2.54f,2.54f));
//						ParentMat *= Scale;
//					}
//					if (GetExchangeYZ())
//					{
//						ParentMat = ExchangeMatrixYZ(&ParentMat);
//					}
//					ParentMat.Invert();
//					tm = tm * ParentMat;
//				}
//
//				m_lpAnimationBone[i*NumFrame+NowFrame] = tm;
//			}
//
//			if (objRange.End() >= animRange.End()) {
//				break;
//			}
//			else {
//				t = (objRange.End()/GetTicksPerFrame()+GetMeshFrameStep()) * GetTicksPerFrame();
//			}
//			NowFrame++;
//		}
//
//		//现在把整个动画过程一动不动的骨骼标记出来
//
//		DWORD  NumAnimatedBone = pMesh->m_dNumBone;
//
//		//Label the bones never moved during the animation
//		DWORD CurrentOffset = 0;
//		INode* IBoneNode;
//		for(DWORD j = 0; j < pMesh->m_dNumBone; j++)
//		{
//			int nNumKeys = 0;	
//			Interval AnimRange = m_ip->GetAnimRange();
//			IBoneNode = (INode*)pMesh->m_lpBoneInfo[j].Name;
//			m_ip->SelectNode(IBoneNode);
//			ITrackBar* pTrackBar = m_ip->GetTrackBar();
//			TimeValue CurrentTime = GetStaticFrame();
//			BOOL bKeyValid = FALSE;
//			while (CurrentTime <= AnimRange.End())
//			{
//				TimeValue NextFrame;
//				NextFrame = pTrackBar->GetNextKey(CurrentTime, TRUE);
//				nNumKeys++;
//				if (CurrentTime >= NextFrame)//No more key
//					break;
//				CurrentTime = NextFrame;
//			}
//	
//			if (nNumKeys == 1)//No key frame
//			{
//				pMesh->m_lpBoneInfo[j].Static = TRUE;
//			}
//			else 
//			{
//				pMesh->m_lpBoneInfo[j].Static = FALSE;
//			}
//		}
//
//		//把结果保存到MESH中
//		pMesh->m_dNumAnimationBones = pMesh->m_dNumBone;
//		if(pMesh->m_dNumAnimationBones!=0)
//		{
//			pMesh->m_dNumFrames = NumFrame;
//			pMesh->m_fFrameLength = GetTicksPerFrame();
//
//			CMesh::LPMATRIX SrcAnimationBoneTran = new CMesh::MATRIX[NumAnimatedBone*NumFrame];
//			if(SrcAnimationBoneTran==NULL) return E_FAIL;
//			ZeroMemory(SrcAnimationBoneTran ,sizeof(CMesh::MATRIX)*
//				NumAnimatedBone*NumFrame);
//
//			LPSTR*   SrcAnimationBoneName = new LPSTR[NumAnimatedBone];
//			ZeroMemory(SrcAnimationBoneName ,sizeof(LPSTR)*NumAnimatedBone);
//
//			DWORD K=0;
//			for(DWORD i=0;i<pMesh->m_dNumBone;i++)
//			{
//				CMesh::LPBONEINFO pBoneInfo = &pMesh->m_lpBoneInfo[i];
//
//				SrcAnimationBoneName[K] = pBoneInfo->Name;
//				for(DWORD n=0;n<NumFrame;n++)
//				{
//					int Index = i*NumFrame+n;
//					Matrix3* Mat3 = &m_lpAnimationBone[Index];
//					CMesh::LPMATRIX Mat4 = &SrcAnimationBoneTran[K*NumFrame+n];
//					Matrix3ToMatrix4(Mat4,Mat3);
//				}
//				K++;
//			}
//
//			pMesh->m_lpAnimationBonesTrans = (CMesh::LPMATRIX) SrcAnimationBoneTran;
//			pMesh->m_lpAnimationBonesName = SrcAnimationBoneName;
//		}
//
//		//释放内存
//		SAFE_DELETE_ARRAY(m_lpAnimationBone);
//
//		//把数据中的INode* 转换为字符串，长度为３０
//		for(DWORD i=0;i<pMesh->m_dNumBone;i++)
//		{
//			CMesh::LPBONEINFO pBoneInfo = &pMesh->m_lpBoneInfo[i];
//			INode* pBoneNode = (INode*) pBoneInfo->Name;
//			pBoneInfo->Name = new TCHAR[MAX_BONE_NAME_LENGTH];
//			if(pBoneNode)
//				wsprintf(pBoneInfo->Name,"%s",pBoneNode->GetName());
//			else
//				wsprintf(pBoneInfo->Name,"");
//
//			INode* pBoneParent = (INode*) pBoneInfo->ParentName;
//			pBoneInfo->ParentName = new TCHAR[MAX_BONE_NAME_LENGTH];
//			if(pBoneParent)
//				wsprintf(pBoneInfo->ParentName,"%s",pBoneParent->GetName());
//			else
//				wsprintf(pBoneInfo->ParentName,"");
//
//			for(DWORD K=0;K<pBoneInfo->NumChild;K++)
//			{
//				INode* pBoneChild = (INode*) pBoneInfo->lpChildNames[K];
//				pBoneInfo->lpChildNames[K] = new TCHAR[MAX_BONE_NAME_LENGTH];
//				if(pBoneChild)
//					wsprintf(pBoneInfo->lpChildNames[K],"%s",pBoneChild->GetName());
//				else
//					wsprintf(pBoneInfo->lpChildNames[K],"");
//			}
//		}
//		for(i=0;i<pMesh->m_dNumAnimationBones;i++)
//		{
//			INode* pBoneNode = (INode*) pMesh->m_lpAnimationBonesName[i];
//			pMesh->m_lpAnimationBonesName[i] = new TCHAR[MAX_BONE_NAME_LENGTH];
//			if(pBoneNode)
//				wsprintf(pMesh->m_lpAnimationBonesName[i],"%s",pBoneNode->GetName());
//			else
//				wsprintf(pMesh->m_lpAnimationBonesName[i],"");
//		}
//		pMesh->m_dwAnimationType = ANIMATION_BONE_KEYONLY;
//
//	}
//
//	return S_OK;
////	int i, j, k;
////	int nNumKeys = 0;
////	Control* pCtrl[3];
////	IKeyControl* pKeyCtrl[3];
////
////	float TimeUnit = (float) GetTicksPerFrame() * GetFrameRate();
////
////	CMesh::LPBONEINFO pBoneInfo;
////	INode* IBoneNode;
////	for(j = 0; j < pMesh->m_dNumBone; j++)
////	{
////		Interval AnimRange = m_ip->GetAnimRange();
////		IBoneNode = (INode*)pMesh->m_lpBoneInfo[j].Name;
////	
////		//IBoneNode = m_ip->GetINodeByName("Bip01 Head");
////		m_ip->SelectNode(IBoneNode);
////
////		ITrackBar* pTrackBar = m_ip->GetTrackBar();
////		TimeValue CurrentTime = GetStaticFrame();
////		BOOL bKeyValid = FALSE;
////		CMesh::BoneAnimInfo BoneAnimInfo;
////
////		while (CurrentTime <= AnimRange.End())
////		{
////			TimeValue NextFrame;
////			NextFrame = pTrackBar->GetNextKey(CurrentTime, TRUE);
////			CurrentTime = NextFrame;
////			nNumKeys++;
////		}
////
////		if (nNumKeys == 1)//No key frame
////		{
////			pMesh->m_lpBoneInfo[j].Static = TRUE;
////		}
////		else 
////		{
////			pMesh->m_lpBoneInfo[j].Static = FALSE;
////		}
////		i = 0;
////		//while (CurrentTime <= AnimRange.End())
////		//{
////		//	TimeValue NextFrame;
////		//	NextFrame = pTrackBar->GetNextKey(CurrentTime, TRUE);
////		//	CurrentTime = NextFrame;
////		//	Matrix3 matBone = IBoneNode->GetObjTMAfterWSM(CurrentTime);
////		//	INode* pParentNode = IBoneNode->GetParentNode();
////		//	if (pParentNode && pParentNode != m_ip->GetRootNode())
////		//	{
////		//		Matrix3 matParent = pParentNode->GetObjTMAfterWSM(CurrentTime);
////		//		if(m_bInch2CM)
////		//		{
////		//			Matrix3 Scale;
////		//			Scale.SetScale(Point3(2.54f,2.54f,2.54f));
////		//			matParent *= Scale;
////		//		}
////		//		if (GetExchangeYZ())
////		//		{
////		//			matParent = ExchangeMatrixYZ(&matParent);
////		//		}
////		//		matParent.Invert();
////		//		matBone = matBone * matParent;
////		//	}
////		//	Matrix3ToMatrix4(&BoneAnimInfo.pKeyFrames[i].mat, &matBone);
////		//	BoneAnimInfo.pKeyFrames[i].fTime = CurrentTime / TimeUnit;
////		//	i++;
////		//}
////		//pMesh->m_BoneAnimInfo.push_back(BoneAnimInfo);
////	}
////
//////	//把结果保存到MESH中
////	
////	
////	pMesh->m_dNumAnimationBones = pMesh->m_dNumBone;
////	int NumAnimatedBone = pMesh->m_dNumAnimationBones;
////	if(pMesh->m_dNumAnimationBones!=0)
////	{
////		pMesh->m_dNumFrames = nNumKeys;
////		pMesh->m_fFrameLength = GetTicksPerFrame();
////
////		//CMesh::LPMATRIX SrcAnimationBoneTran = new CMesh::MATRIX[NumAnimatedBone*NumFrame];
////		//if(SrcAnimationBoneTran==NULL) return E_FAIL;
////	//	ZeroMemory(SrcAnimationBoneTran ,sizeof(CMesh::MATRIX)*
//////			NumAnimatedBone*NumFrame);
////
////		LPSTR*   SrcAnimationBoneName = new LPSTR[NumAnimatedBone];
////		ZeroMemory(SrcAnimationBoneName ,sizeof(LPSTR)*NumAnimatedBone);
////
////		//DWORD K=0;
////		//for(DWORD i=0;i<pMesh->m_dNumBone;i++)
////		//{
////		//	CMesh::LPBONEINFO pBoneInfo = &pMesh->m_lpBoneInfo[i];
////
////		//	SrcAnimationBoneName[K] = pBoneInfo->Name;
////		//	for(DWORD n=0;n<NumFrame;n++)
////		//	{
////		//		int Index = i*NumFrame+n;
////		//		Matrix3* Mat3 = &m_lpAnimationBone[Index];
////		//		CMesh::LPMATRIX Mat4 = &SrcAnimationBoneTran[K*NumFrame+n];
////		//		Matrix3ToMatrix4(Mat4,Mat3);
////		//	}
////		//	K++;
////		//}
////
////		//pMesh->m_lpAnimationBonesTrans = (CMesh::LPMATRIX) SrcAnimationBoneTran;
////		pMesh->m_lpAnimationBonesName = SrcAnimationBoneName;
////	}
////
////	//释放内存
////	//SAFE_DELETE_ARRAY(m_lpAnimationBone);
////
////	//把数据中的INode* 转换为字符串，长度为３０
////	for(DWORD i=0;i<pMesh->m_dNumBone;i++)
////	{
////		CMesh::LPBONEINFO pBoneInfo = &pMesh->m_lpBoneInfo[i];
////		INode* pBoneNode = (INode*) pBoneInfo->Name;
////		pBoneInfo->Name = new TCHAR[MAX_BONE_NAME_LENGTH];
////		if(pBoneNode)
////			wsprintf(pBoneInfo->Name,"%s",pBoneNode->GetName());
////		else
////			wsprintf(pBoneInfo->Name,"");
////
////		INode* pBoneParent = (INode*) pBoneInfo->ParentName;
////		pBoneInfo->ParentName = new TCHAR[MAX_BONE_NAME_LENGTH];
////		if(pBoneParent)
////			wsprintf(pBoneInfo->ParentName,"%s",pBoneParent->GetName());
////		else
////			wsprintf(pBoneInfo->ParentName,"");
////
////		for(DWORD K=0;K<pBoneInfo->NumChild;K++)
////		{
////			INode* pBoneChild = (INode*) pBoneInfo->lpChildNames[K];
////			pBoneInfo->lpChildNames[K] = new TCHAR[MAX_BONE_NAME_LENGTH];
////			if(pBoneChild)
////				wsprintf(pBoneInfo->lpChildNames[K],"%s",pBoneChild->GetName());
////			else
////				wsprintf(pBoneInfo->lpChildNames[K],"");
////		}
////	}
////	for(i=0;i<pMesh->m_dNumAnimationBones;i++)
////	{
////		INode* pBoneNode = (INode*) pMesh->m_lpAnimationBonesName[i];
////		pMesh->m_lpAnimationBonesName[i] = new TCHAR[MAX_BONE_NAME_LENGTH];
////		if(pBoneNode)
////			wsprintf(pMesh->m_lpAnimationBonesName[i],"%s",pBoneNode->GetName());
////		else
////			wsprintf(pMesh->m_lpAnimationBonesName[i],"");
////	}
////	pMesh->m_dwAnimationType = ANIMATION_BONE_KEYONLY;
////	return S_OK;
//
//	
//}

HRESULT AsciiExp::ExportRelateBoneAnimationKeyOnly(INode *node, LPMESH pMesh)
{
	ObjectState os = node->EvalWorldState(GetStaticFrame());
	if (!os.obj)
		return S_OK;

	// Get animation range
	Interval animRange = m_ip->GetAnimRange();
	// Get validity of the object
	Interval objRange = os.obj->ObjectValidity(GetStaticFrame());


	// If the animation range is not fully included in the validity
	// interval of the object, then we're animated.
	TimeValue F = GetTicksPerFrame();
	TimeValue FF = GetMeshFrameStep();

	
	if (!objRange.InInterval(animRange))
	{
		DWORD NumFrame = 0;	
		TimeValue t = animRange.Start();
		//这个计算过程看起来如此复杂，不如让它试运行一次，看看到底多少桢
		while (1) {
			// This may seem strange, but the object in the pipeline
			// might not be valid anymore.
			os = node->EvalWorldState(t);
			objRange = os.obj->ObjectValidity(t);
			t = objRange.Start() < animRange.Start() ? animRange.Start() : objRange.Start();

			// Export the mesh definition at this frame
			//ExportMesh(node, pMesh,t, indentLevel+1);
			NumFrame++;
			if (objRange.End() >= animRange.End()) {
				break;
			}
			else {
				t = (objRange.End()/GetTicksPerFrame()+GetMeshFrameStep()) * GetTicksPerFrame();
			}
		}
		//申请内存，

		Matrix3*  m_lpAnimationBone = new Matrix3[pMesh->m_dNumBone * NumFrame];
		ZeroMemory(m_lpAnimationBone ,sizeof(Matrix3)*pMesh->m_dNumBone*NumFrame);

		DWORD NowFrame = 0;
		t = animRange.Start();
		while (1) 
		{
			// This may seem strange, but the object in the pipeline
			// might not be valid anymore.
			os = node->EvalWorldState(t);
			objRange = os.obj->ObjectValidity(t);
			t = objRange.Start() < animRange.Start() ? animRange.Start() : objRange.Start();

			
			// Export the mesh definition at this frame
			for(DWORD i=0;i<pMesh->m_dNumBone;i++)
			{
				CMesh::LPBONEINFO pBoneInfo = &pMesh->m_lpBoneInfo[i];
				INode* IBoneNode = pBoneInfo->pThis;
				Matrix3 tm = IBoneNode->GetObjTMAfterWSM(t);
				if(m_bInch2CM)
				{
					Matrix3 Scale;
					Scale.SetScale(Point3(2.54f,2.54f,2.54f));
					tm *= Scale;
				}
				if (GetExchangeYZ())
				{
					tm = ExchangeMatrixYZ(&tm);
				}

				INode* INodeParent = IBoneNode->GetParentNode();
				if (INodeParent!=m_ip->GetRootNode())
				{
					Matrix3 ParentMat = INodeParent->GetObjTMAfterWSM(t);
					if(m_bInch2CM)
					{
						Matrix3 Scale;
						Scale.SetScale(Point3(2.54f,2.54f,2.54f));
						ParentMat *= Scale;
					}
					if (GetExchangeYZ())
					{
						ParentMat = ExchangeMatrixYZ(&ParentMat);
					}
					ParentMat.Invert();
					tm = tm * ParentMat;
				}

				m_lpAnimationBone[i*NumFrame+NowFrame] = tm;
			}

			if (objRange.End() >= animRange.End())
			{
				break;
			}
			else
			{
				t = (objRange.End()/GetTicksPerFrame()+GetMeshFrameStep()) * GetTicksPerFrame();
			}
			NowFrame++;
		}

		//现在把整个动画过程一动不动的骨骼标记出来
		DWORD  NumAnimatedBone = pMesh->m_dNumBone;

		//Label the bones never moved during the animation
		DWORD CurrentOffset = 0;
		INode* IBoneNode;
		
		
 
		//把结果保存到MESH中
		pMesh->m_dNumAnimationBones = pMesh->m_dNumBone;
		if(pMesh->m_dNumAnimationBones!=0)
		{
			pMesh->m_dNumFrames = NumFrame;
			pMesh->m_fFrameLength = GetTicksPerFrame();

			CMesh::LPMATRIX SrcAnimationBoneTran = new CMesh::MATRIX[NumAnimatedBone*NumFrame];
			if(SrcAnimationBoneTran==NULL) return E_FAIL;
			ZeroMemory(SrcAnimationBoneTran ,sizeof(CMesh::MATRIX)*
				NumAnimatedBone*NumFrame);

			LPSTR*   SrcAnimationBoneName = new LPSTR[NumAnimatedBone];
			ZeroMemory(SrcAnimationBoneName ,sizeof(LPSTR)*NumAnimatedBone);

			for(DWORD i=0;i<pMesh->m_dNumBone;i++)
			{
				CMesh::LPBONEINFO pBoneInfo = &pMesh->m_lpBoneInfo[i];

				SrcAnimationBoneName[i] = (LPSTR)pBoneInfo->pThis;
				for(DWORD n=0;n<NumFrame;n++)
				{
					int Index = i*NumFrame+n;
					Matrix3* Mat3 = &m_lpAnimationBone[Index];
					CMesh::LPMATRIX Mat4 = &SrcAnimationBoneTran[i*NumFrame+n];
					Matrix3ToMatrix4(Mat4,Mat3);
					
				}
			}
			
			pMesh->m_lpAnimationBonesTrans = (CMesh::LPMATRIX) SrcAnimationBoneTran;
			pMesh->m_lpAnimationBonesName = SrcAnimationBoneName;
		}

		for(DWORD j = 0; j < pMesh->m_dNumBone; j++)
		{
			int nNumKeys = 0;	
			Interval AnimRange = m_ip->GetAnimRange();
			IBoneNode = pMesh->m_lpBoneInfo[j].pThis;
			m_ip->SelectNode(IBoneNode);
			ITrackBar* pTrackBar = m_ip->GetTrackBar();
			TimeValue CurrentTime = GetStaticFrame();
			BOOL bKeyValid = FALSE;
			while (CurrentTime <= AnimRange.End())
			{
				TimeValue NextFrame;
				NextFrame = pTrackBar->GetNextKey(CurrentTime, TRUE);
				nNumKeys++;
				if (CurrentTime >= NextFrame)//No more key
					break;
				CurrentTime = NextFrame;
			}

			if (nNumKeys == 1)//No key frame
			{
				pMesh->m_lpBoneInfo[j].Static = TRUE;
			}
			else 
			{
				pMesh->m_lpBoneInfo[j].Static = FALSE;
			}
		}

		//释放内存
		SAFE_DELETE_ARRAY(m_lpAnimationBone);

		//把数据中的INode* 转换为字符串，长度为３０
		FixSkinInfo(node, pMesh);

		for(int i = 0; i < pMesh->m_dNumAnimationBones;i++)
		{
			INode* pBoneNode = (INode*) pMesh->m_lpAnimationBonesName[i];
			pMesh->m_lpAnimationBonesName[i] = new TCHAR[MAX_BONE_NAME_LENGTH];
			if(pBoneNode)
				wsprintf(pMesh->m_lpAnimationBonesName[i],"%s",pBoneNode->GetName());
			else
				wsprintf(pMesh->m_lpAnimationBonesName[i],"");
		}
		pMesh->m_dwAnimationType = ANIMATION_BONE_KEYONLY;

	}
	else
	{
		FixSkinInfo(node, pMesh);
	}

	return S_OK;
}

HRESULT AsciiExp::FixSkinInfo(INode* node, LPMESH pMesh)
{
	HRESULT hr = E_FAIL;
	for(DWORD i=0;i<pMesh->m_dNumBone;i++)
	{
		CMesh::LPBONEINFO pBoneInfo = &pMesh->m_lpBoneInfo[i];
		INode* pBoneNode = pBoneInfo->pThis;
		if(pBoneNode)
			wsprintf(pBoneInfo->szName,"%s",pBoneNode->GetName());
		else
			wsprintf(pBoneInfo->szName,"");

		INode* pBoneParent = pBoneInfo->pParent;
		if(pBoneParent)
			wsprintf(pBoneInfo->szParentName,"%s",pBoneParent->GetName());
		else
			wsprintf(pBoneInfo->szParentName,"");

		//for(DWORD K=0;K<pBoneInfo->NumChild;K++)
		//{
		//	INode* pBoneChild = (INode*) pBoneInfo->lpChildNames[K];
		//	pBoneInfo->lpChildNames[K] = new TCHAR[MAX_BONE_NAME_LENGTH];
		//	if(pBoneChild)
		//		wsprintf(pBoneInfo->lpChildNames[K],"%s",pBoneChild->GetName());
		//	else
		//		wsprintf(pBoneInfo->lpChildNames[K],"");
		//}
	}
	return hr;
}

HRESULT AsciiExp::ExportRTSBoneAnimation(INode *node, LPMESH pMesh)
{
    m_bForearm = true;

    ObjectState os = node->EvalWorldState(GetStaticFrame());
	if (!os.obj)//虽然这里osboj是空代表没有动画，但是我们仍然需要把Skin的信息修改好
		return S_OK;

	// Get animation range
	Interval animRange = m_ip->GetAnimRange();
	// Get validity of the object
	Interval objRange = os.obj->ObjectValidity(GetStaticFrame());


	// If the animation range is not fully included in the validity
	// interval of the object, then we're animated.
	TimeValue F = GetTicksPerFrame();
	TimeValue FF = GetMeshFrameStep();

	if (!objRange.InInterval(animRange)) 
	{
		DWORD NumFrame = 0;
		TimeValue t = animRange.Start();
		//这个计算过程看起来如此复杂，不如让它试运行一次，看看到底多少桢
		while (1) {
			// This may seem strange, but the object in the pipeline
			// might not be valid anymore.
			os = node->EvalWorldState(t);
			objRange = os.obj->ObjectValidity(t);
			t = objRange.Start() < animRange.Start() ? animRange.Start() : objRange.Start();

			// Export the mesh definition at this frame
			//ExportMesh(node, pMesh,t, indentLevel+1);
			NumFrame++;
			if (objRange.End() >= animRange.End()) {
				break;
			}
			else {
				t = (objRange.End()/GetTicksPerFrame()+GetMeshFrameStep()) * GetTicksPerFrame();
			}
		}
		//申请内存，

		Matrix3*  m_lpAnimationBone = new Matrix3[pMesh->m_dNumBone * NumFrame];
		ZeroMemory(m_lpAnimationBone ,sizeof(Matrix3)*pMesh->m_dNumBone*NumFrame);

		DWORD NowFrame = 0;
		t = animRange.Start();
		while (1) {
			// This may seem strange, but the object in the pipeline
			// might not be valid anymore.
			os = node->EvalWorldState(t);
			objRange = os.obj->ObjectValidity(t);
			t = objRange.Start() < animRange.Start() ? animRange.Start() : objRange.Start();


			// Export the mesh definition at this frame
			for(DWORD i=0;i<pMesh->m_dNumBone;i++)
			{
				CMesh::LPBONEINFO pBoneInfo = &pMesh->m_lpBoneInfo[i];
				INode* IBoneNode = pBoneInfo->pThis;
				INode* INodeParent = pBoneInfo->pParent;
				TSTR  strTemp = INodeParent->NodeName();
				TCHAR strParentNodeName[255];
				strcpy(strParentNodeName, strTemp);
				size_t ParentNodeNameLength = strlen(strParentNodeName);
				if (ParentNodeNameLength >= 8)
				{
					if (!strcmp(strParentNodeName + ParentNodeNameLength - 7, "Forearm") && !m_bForearm)
					{
						//If node is hand and there is no forearm, link hand to foretwist
						strcpy(strParentNodeName + ParentNodeNameLength - 7, "ForeTwist1");
						
						INode* pFixedParentNode = FindBoneByName(strParentNodeName, pMesh->m_lpBoneInfo, pMesh->m_dNumBone);
						if (pFixedParentNode)
						{
							//Huh, if associated foretwist is not found, do nothing, but that means something is 
							//wrong now.
							INodeParent = pFixedParentNode;
						}
					}
				}
				if (m_relink.count(IBoneNode))//bone is relinked
				{
					INodeParent = m_relink[IBoneNode];
				}

                Matrix3 mat3Node;
                Matrix3 mat3Local;

                if (INodeParent != m_ip->GetRootNode())
                {
                    Matrix3 mat3Parent = INodeParent->GetNodeTM(t);
                    mat3Parent.Invert();
                    mat3Node = IBoneNode->GetNodeTM(t);
                    mat3Local = mat3Node * mat3Parent;

                }               
                else
                {
                    mat3Node = IBoneNode->GetNodeTM(t);
                    mat3Local = mat3Node; 
                    if (GetExportOption()->m_bInch2CM)
                    {
                        mat3Local *= g_mat3Inch2CMScale;
                    }
                }

                if (GetExportOption()->m_bExchangeYZ)
                    GetHelper().ChangeMatrixYZ(&mat3Local);

                m_lpAnimationBone[i*NumFrame+NowFrame] = mat3Local;


// 				//Matrix3 tm = IBoneNode->GetObjTMAfterWSM(t);
// 				Matrix3 tm = IBoneNode->GetNodeTM(t);
// 				if(m_bInch2CM)
// 				{
// 					Matrix3 Scale;
// 					Scale.SetScale(Point3(2.54f,2.54f,2.54f));
// 					tm *= Scale;
// 				}
// 
// 				if (GetExchangeYZ())
// 				{
// 					tm = ExchangeMatrixYZ(&tm);
// 				}
// 
// 				
// 				if (INodeParent!=m_ip->GetRootNode())
// 				{
// 					//Matrix3 ParentMat = INodeParent->GetObjTMAfterWSM(t);
// 					Matrix3 ParentMat = INodeParent->GetNodeTM(t);
// 					if(m_bInch2CM)
// 					{
// 						Matrix3 Scale;
// 						Scale.SetScale(Point3(2.54f,2.54f,2.54f));
// 						ParentMat *= Scale;
// 					}
// 					if (GetExchangeYZ())
// 					{
// 						ParentMat = ExchangeMatrixYZ(&ParentMat);
// 					}
// 					ParentMat.Invert();
// 					tm = tm * ParentMat;
// 				}
// 
// 				m_lpAnimationBone[i*NumFrame+NowFrame] = tm;
			}

			if (objRange.End() >= animRange.End()) {
				break;
			}
			else {
				t = (objRange.End()/GetTicksPerFrame()+GetMeshFrameStep()) * GetTicksPerFrame();
			}
			NowFrame++;
		}

		//现在把整个动画过程一动不动的骨骼去掉

		DWORD  NumAnimatedBone = pMesh->m_dNumBone;
		/*BOOL * BoneAnimated = new BOOL[pMesh->m_dNumBone];
		ZeroMemory(BoneAnimated,sizeof(BOOL)*pMesh->m_dNumBone);

		for(DWORD i=0;i<pMesh->m_dNumBone;i++)
		{
			Matrix3 FirstMat;
			FirstMat = m_lpAnimationBone[i*NumFrame];
			for(DWORD n=1;n<NumFrame;n++)
			{
				Matrix3 v = m_lpAnimationBone[i*NumFrame+n];
				if(!(v==FirstMat))
				{
					NumAnimatedBone++;
					BoneAnimated[i] = TRUE;
					break;
				}
			}
		}*/

		//把结果保存到MESH中
		pMesh->m_dNumAnimationBones = pMesh->m_dNumBone;
		if(pMesh->m_dNumAnimationBones!=0)
		{
			pMesh->m_dNumFrames = NumFrame;
			pMesh->m_fFrameLength = GetTicksPerFrame();

			CMesh::LPMATRIX SrcAnimationBoneTran = new CMesh::MATRIX[NumAnimatedBone*NumFrame];
			if(SrcAnimationBoneTran==NULL) return E_FAIL;
			ZeroMemory(SrcAnimationBoneTran ,sizeof(CMesh::MATRIX)*
				NumAnimatedBone*NumFrame);

			CMesh::RTSBoneAnimation* pSrcRTS = new CMesh::RTSBoneAnimation[NumAnimatedBone*NumFrame];
			ZeroMemory(pSrcRTS, sizeof(CMesh::RTSBoneAnimation) * NumFrame * NumAnimatedBone);
		
			LPSTR*   SrcAnimationBoneName = new LPSTR[NumAnimatedBone];
			ZeroMemory(SrcAnimationBoneName ,sizeof(LPSTR)*NumAnimatedBone);

			DWORD K=0;
			for(DWORD i=0;i<pMesh->m_dNumBone;i++)
			{
				CMesh::LPBONEINFO pBoneInfo = &pMesh->m_lpBoneInfo[i];

				SrcAnimationBoneName[K] = (LPSTR)pBoneInfo->pThis;
				for(DWORD n=0;n<NumFrame;n++)
				{
					int Index = i*NumFrame+n;
					Matrix3* Mat3 = &m_lpAnimationBone[Index];
				
					AffineParts ap;
					decomp_affine(*Mat3, &ap);
					
					pSrcRTS[NumFrame * K + n].Scale.x = ap.k.x;
					pSrcRTS[NumFrame * K + n].Scale.y = ap.k.y;
					pSrcRTS[NumFrame * K + n].Scale.z = ap.k.z;

					pSrcRTS[NumFrame * K + n].Rotation.x = ap.q.x;
					pSrcRTS[NumFrame * K + n].Rotation.y = ap.q.y;
					pSrcRTS[NumFrame * K + n].Rotation.z = ap.q.z;
					pSrcRTS[NumFrame * K + n].Rotation.w = ap.q.w;

					pSrcRTS[NumFrame * K + n].Translation.x = ap.t.x;
					pSrcRTS[NumFrame * K + n].Translation.y = ap.t.y;
					pSrcRTS[NumFrame * K + n].Translation.z = ap.t.z;
					
					pSrcRTS[NumFrame * K + n].Sign = ap.f;

					pSrcRTS[NumFrame * K + n].SRotation.x = ap.u.x;
					pSrcRTS[NumFrame * K + n].SRotation.y = ap.u.y;
					pSrcRTS[NumFrame * K + n].SRotation.z = ap.u.z;
					pSrcRTS[NumFrame * K + n].SRotation.w = ap.u.w;
					
				}
				K++;
			}

			pMesh->m_lpRTSAniBones = pSrcRTS;
			pMesh->m_lpAnimationBonesName = SrcAnimationBoneName;
		}

		//释放内存
		SAFE_DELETE_ARRAY(m_lpAnimationBone);

		//把数据中的INode* 转换为字符串，长度为３０
		FixSkinInfo(node, pMesh);

		for(int i = 0; i < pMesh->m_dNumAnimationBones;i++)
		{
			INode* pBoneNode = (INode*) pMesh->m_lpAnimationBonesName[i];
			pMesh->m_lpAnimationBonesName[i] = new TCHAR[MAX_BONE_NAME_LENGTH];
			if(pBoneNode)
				wsprintf(pMesh->m_lpAnimationBonesName[i],"%s",pBoneNode->GetName());
			else
				wsprintf(pMesh->m_lpAnimationBonesName[i],"");
		}
		pMesh->m_dwAnimationType = ANIMATION_BONE_RTS;

	}
	else
	{
		FixSkinInfo(node, pMesh);
	}

	return S_OK;
}

HRESULT AsciiExp::_ExchangeMeshData_GetVertexPos(
    const Mesh *pcSrcMesh, 
    Point3 *pDestVertices, 
    const Matrix3 &cmTranform, 
    TimeValue tvTime,
    INode *pNode
)
{
    HRESULT hrResult = E_FAIL;
	Point3  ZeroPoint(0, 0, 0);
	Matrix3 mTemp;

    KGLOG_PROCESS_ERROR(pcSrcMesh);    
    KGLOG_PROCESS_ERROR(pDestVertices);
    KGLOG_PROCESS_ERROR(pNode);
	
	INode *pParentNode = pNode->GetParentNode();
	mTemp = pParentNode->GetObjTMAfterWSM(tvTime);

	ZeroPoint = ZeroPoint * mTemp;
	float fFactorial = 1.0f;
	if (m_bInch2CM)
	    fFactorial = 2.54f;
    
	for (int i = 0; i < pcSrcMesh->getNumVerts(); i++)
	{
		Point3 pt3Point = cmTranform * pcSrcMesh->verts[i] * fFactorial - ZeroPoint;

		if (GetExchangeYZ())
		    swap(pt3Point.y, pt3Point.z);

		pDestVertices[i] = pt3Point;
	}
    
    
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT AsciiExp::_ExchangeMeshData_GetTextureCoord(
    int nFaceIndex,
    KG_3D_EXPORT_FACE *pExportFace, 
    Mesh *pSrcMesh, 
    DWORD dwChannelID,
    int VertexOrder[3]
)
{
    HRESULT hrResult = E_FAIL;
    int j = 0;
    
    KGLOG_PROCESS_ERROR(pExportFace);
    KGLOG_PROCESS_ERROR(pSrcMesh);
    
    KGLOG_PROCESS_ERROR(
        (dwChannelID == 2) ||
        (dwChannelID == 3)
    );
    
	if (pSrcMesh->mapSupport(dwChannelID))
	{
		int nMapVertexs = pSrcMesh->getNumMapVerts(dwChannelID);
		//KG_ASSERT_EXIT(nMapVertexs > 0);
		
		for (j = 0; j < 3; j++)
		{
    		int    nVertexIndex = pSrcMesh->mapFaces(dwChannelID)[nFaceIndex].t[VertexOrder[j]];
    		UVVert uvVertex     = pSrcMesh->mapVerts(dwChannelID)[nVertexIndex];

            uvVertex.y = 1 - uvVertex.y;
		    pExportFace[nFaceIndex].Vertex[j].pt3TextureCoord[dwChannelID - 1] = uvVertex;
		}
	}    
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT AsciiExp::_ExchangeMeshData_GetTextureCoord_0(
    int nFaceIndex,
    KG_3D_EXPORT_FACE *pExportFace, 
    INode *pNode,
    Mesh *pSrcMesh, 
    int VertexOrder[3]
)
{
    HRESULT hrResult = E_FAIL;
    int     j        = 0;
	Mtl    *pmtlNode = NULL; 
    ULONG   ulMatReq = 0;

    KGLOG_PROCESS_ERROR(pExportFace);
    KGLOG_PROCESS_ERROR(pNode);
    KGLOG_PROCESS_ERROR(pSrcMesh);
    
    pmtlNode = pNode->GetMtl();
    KG_PROCESS_SUCCESS(!pmtlNode);   // if null then success

    ulMatReq = pmtlNode->Requirements(-1);

    if (ulMatReq & MTLREQ_FACEMAP)
    {
        Point3 pt3TV[3];
        Face *pFace = &pSrcMesh->faces[nFaceIndex];
        make_face_uv(pFace, pt3TV);
        for (j = 0; j < 3; j++)
        {
            pt3TV[j].y = 1 - pt3TV[j].y;
            pExportFace[nFaceIndex].Vertex[j].pt3TextureCoord[0] = pt3TV[j];
        }
        goto Exit1;
    }

    //ASSERT(pSrcMesh->tVerts);
    for (j = 0; j < 3; j++)
    {
        Point3 pt3TV;
        pt3TV = pSrcMesh->tVerts[pSrcMesh->tvFace[nFaceIndex].t[VertexOrder[j]]];
        pt3TV.y = 1 - pt3TV.y;
        pExportFace[nFaceIndex].Vertex[j].pt3TextureCoord[0] = pt3TV;
    }

Exit1:    
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT _ExchangeMeshData_Diffuse(
    int nFaceIndex,
    KG_3D_EXPORT_FACE *pExportFace, 
    Mesh *pSrcMesh, 
    int VertexOrder[3]
)
{
    HRESULT hrResult = E_FAIL;
    int j = 0;

    KGLOG_PROCESS_ERROR(pExportFace);
    KGLOG_PROCESS_ERROR(pSrcMesh);
    
    for (j = 0; j < 3; j++)
    {
        int nVertexIndex = pSrcMesh->vcFace[nFaceIndex].t[VertexOrder[j]];
		
		VertColor vcA = pSrcMesh->vertCol[nVertexIndex];
		
		pExportFace[nFaceIndex].Vertex[j].Diffuse.a = 1.0f;
		pExportFace[nFaceIndex].Vertex[j].Diffuse.r = vcA.x;
		pExportFace[nFaceIndex].Vertex[j].Diffuse.g = vcA.y;
		pExportFace[nFaceIndex].Vertex[j].Diffuse.b = vcA.z;
	}
    
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT AsciiExp::_ExchangeMeshData_Normal(
    int nFaceIndex,
	IEditNormalsMod *pEditNormalMod,
	Matrix3 TM,
    KG_3D_EXPORT_FACE *pExportFace, 
    Mesh *pSrcMesh, 
    int VertexOrder[3]
)
{
    HRESULT hrResult = E_FAIL;
    int j = 0;
	int  nVertex = 0;
	DWORD *pVertex = NULL;
	Point3 Normal; 

    KGLOG_PROCESS_ERROR(pExportFace);
    KGLOG_PROCESS_ERROR(pSrcMesh);
    
	pVertex = pSrcMesh->faces[nFaceIndex].getAllVerts();
	KGLOG_PROCESS_ERROR(pVertex);
	

    for (j = 0; j < 3; j++)
    {	
        nVertex = pVertex[VertexOrder[j]];
		if (!pEditNormalMod)
		{
			Normal = GetVertexNormal(pSrcMesh, nFaceIndex, pSrcMesh->getRVertPtr(nVertex));
		}
		else
		{
			int nNormalID = pEditNormalMod->EnfnGetNormalID(nFaceIndex, VertexOrder[j]);
			Normal = *(pEditNormalMod->EnfnGetNormal(nNormalID));
			Normal = TM.VectorTransform(Normal);
		}
	    if (GetExchangeYZ())
	        swap(Normal.y, Normal.z);

	    pExportFace[nFaceIndex].Vertex[j].pt3Normal = Normal;
    }
 
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT _ExchangeMeshData_Optimize(
    int nFaceNum,
    KG_3D_EXPORT_FACE  *pExportFace, 
    KG_3D_EXPORT_VERTEX *pNewVertex,
    int *pnRetNewVertex
)
{
    HRESULT hrResult = E_FAIL;
    int nRetCode = false;
    int i = 0;
    int j = 0;
    int k = 0;
    int nNewVertex = 0;

    KGLOG_PROCESS_ERROR(pExportFace);
    KGLOG_PROCESS_ERROR(pNewVertex);
    KGLOG_PROCESS_ERROR(pnRetNewVertex);
    

    for (i = 0; i < nFaceNum; i++)
    {
        for (j = 0; j < 3; j++)
        {
             for (k = 0; k < nNewVertex; k++)
             {
                 nRetCode = memcmp(&pNewVertex[k], &pExportFace[i].Vertex[j], sizeof(pNewVertex[k]));
                 if (nRetCode == 0)
                     break;  //if found
             }
             if (k < nNewVertex)
             {
                 pExportFace[i].dwIndex[j] = k;
                 continue;
             }

            pNewVertex[nNewVertex] = pExportFace[i].Vertex[j];

            pExportFace[i].dwIndex[j] = nNewVertex;

            nNewVertex++;
        }
    }
        
    hrResult = S_OK;
Exit0:
    if (pnRetNewVertex)
    {
        *pnRetNewVertex = nNewVertex; 
    }
    return hrResult;
}

HRESULT AsciiExp::_ExchangeMeshData_SetDestMesh(
    int nFaceNum,
    KG_3D_EXPORT_FACE   *pExportFace, 
    KG_3D_EXPORT_VERTEX *pNewVertex,
    int nNewVertex,
    LPMESH pDestMesh
)
{
    HRESULT hrResult = E_FAIL;
    int i = 0;
    int j = 0;
    int k = 0;
    CMesh::LPVEC3 pVertexPos              = NULL;
    CMesh::LPVEC3 pVertexText[3]          = { NULL, NULL, NULL };
	CMesh::LPCOLOR4 pVertexDiffuse  = NULL;
	CMesh::LPVEC3 pVertexNormal           = NULL;
	DWORD  *pFaceVertexIndex        = NULL;
	DWORD  *pFaceMaterialID         = NULL;
    DWORD  *pVertexRemap            = NULL;

    KGLOG_PROCESS_ERROR(nFaceNum > 0);
    KGLOG_PROCESS_ERROR(pExportFace);
    KGLOG_PROCESS_ERROR(pNewVertex);
    KGLOG_PROCESS_ERROR(nNewVertex > 0);
	
	pVertexPos = new CMesh::VEC3[nNewVertex];
	KGLOG_PROCESS_ERROR(pVertexPos);
	
	for (k = 0; k < 3; k++)
	{
        pVertexText[k] = new CMesh::VEC3[nNewVertex];
	    KGLOG_PROCESS_ERROR(pVertexText[k]);
    }
	
    pVertexNormal = new CMesh::VEC3[nNewVertex];
    KGLOG_PROCESS_ERROR(pVertexNormal);
	pVertexDiffuse = new CMesh::COLOR4[nNewVertex];
	KGLOG_PROCESS_ERROR(pVertexDiffuse);

	pFaceVertexIndex = new DWORD[nFaceNum * 3];
	KGLOG_PROCESS_ERROR(pFaceVertexIndex);
	
	pFaceMaterialID = new DWORD[nFaceNum];
	KGLOG_PROCESS_ERROR(pFaceMaterialID);
	
    pVertexRemap = new DWORD[nNewVertex];
    KGLOG_PROCESS_ERROR(pVertexRemap);

    for (i = 0; i < nNewVertex; i++)
    {
        pVertexPos[i].x = pNewVertex[i].pt3Pos.x;
        pVertexPos[i].y = pNewVertex[i].pt3Pos.y;
        pVertexPos[i].z = pNewVertex[i].pt3Pos.z;
        for (k = 0; k < 3; k++)
        {
            pVertexText[k][i].x = pNewVertex[i].pt3TextureCoord[k].x;
            pVertexText[k][i].y = pNewVertex[i].pt3TextureCoord[k].y;
            pVertexText[k][i].z = pNewVertex[i].pt3TextureCoord[k].z;
        }

        pVertexDiffuse[i] = pNewVertex[i].Diffuse;
        pVertexNormal[i].x  = pNewVertex[i].pt3Normal.x;
        pVertexNormal[i].y  = pNewVertex[i].pt3Normal.y;
        pVertexNormal[i].z  = pNewVertex[i].pt3Normal.z;
        pVertexRemap[i]   = pNewVertex[i].dwOrignIndex;
    }

	for (i = 0; i < nFaceNum; i++)
	{
	    for (j = 0; j < 3; j++)
	    {
        	pFaceVertexIndex[i * 3 + j] = pExportFace[i].dwIndex[j];
		}
		pFaceMaterialID[i] = pExportFace[i].dwMaterialID;
	}
	
 
    pDestMesh->m_dNumFaces    = nFaceNum;
	pDestMesh->m_dNumVertices = nNewVertex;
	pDestMesh->m_lpFacesIndex = pFaceVertexIndex;
	pDestMesh->m_lpSubsetID   = pFaceMaterialID;

	pDestMesh->m_lpVerticePosition      = (CMesh::LPVEC3)pVertexPos;
	pDestMesh->m_lpVerticeTextCoord1    = (CMesh::LPVEC3)pVertexText[0];
	pDestMesh->m_lpVerticeTextCoord2    = (CMesh::LPVEC3)pVertexText[1];
	pDestMesh->m_lpVerticeTextCoord3    = (CMesh::LPVEC3)pVertexText[2];
	pDestMesh->m_lpVerticeDiffuse       = (CMesh::LPCOLOR4)pVertexDiffuse;
    pDestMesh->m_lpVerticeNormal        = (CMesh::LPVEC3)pVertexNormal;
	pDestMesh->m_lpVerticeRemap         = pVertexRemap;
    
    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        KG_DELETE_ARRAY(pVertexRemap);
        KG_DELETE_ARRAY(pFaceMaterialID);
        KG_DELETE_ARRAY(pFaceVertexIndex);
        KG_DELETE_ARRAY(pVertexNormal);
        KG_DELETE_ARRAY(pVertexDiffuse);
	    for (k = 3; k > 0; k--)
	    {
	        KG_DELETE_ARRAY(pVertexText[k - 1]);
        }
    
        KG_DELETE_ARRAY(pVertexPos);
    }
    return hrResult;
}

HRESULT _ExchangeMeshData_CheckValide(
    int nNumVertex,
    Point3 *pSrcVertices,
    LPMESH pDestMesh
)
{
    HRESULT hrResult = E_FAIL;
    int nRetCode = false;
    int i = 0;

    KGLOG_PROCESS_ERROR(pSrcVertices);
    KGLOG_PROCESS_ERROR(pDestMesh);
    KGLOG_PROCESS_ERROR(pDestMesh->m_lpVerticePosition);
    KGLOG_PROCESS_ERROR(pDestMesh->m_lpVerticeRemap);

    for (i = 0; i < pDestMesh->m_dNumVertices; i++)
    {
        KGLOG_PROCESS_ERROR(pDestMesh->m_lpVerticeRemap[i] < nNumVertex);
        nRetCode = memcmp(
            &pDestMesh->m_lpVerticePosition[i], 
            &pSrcVertices[pDestMesh->m_lpVerticeRemap[i]], 
            sizeof(Point3)
        );
        KGLOG_PROCESS_ERROR(nRetCode == 0);
    }

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT AsciiExp::ExchangeMeshData(LPMESH pDestMesh, INode *node, Mesh *pSrcMesh, Mesh *pSrcTextMesh, TimeValue t, Matrix3 tm)
{
    HRESULT hrResult  = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    int i = 0;
    int j = 0;
	KG_3D_EXPORT_FACE   *pExportFace = NULL;
    KG_3D_EXPORT_VERTEX *pNewVertex  = NULL;
    int nNewVertexNum = 0;

	Mtl* nodeMtl = node->GetMtl();
	vector<DWORD> FixID;
	map<DWORD, DWORD> IDRemap;
	std::set<DWORD> setSubset;
    std::set<DWORD>::iterator itSubset;
	BOOL negScale = TMNegParity(tm);
	int VertexOrder[3] = {0, 1, 2};
	if (negScale)
	{
		VertexOrder[0] = 2;
		VertexOrder[1] = 1;
		VertexOrder[2] = 0;
	}

	Matrix3 TMthis = node->GetObjTMAfterWSM(t);
	//Matrix3 TMparent = node->GetParentNode()->GetObjTMAfterWSM(t);
	//TMthis *= TMparent;

	//pSrcMesh->displayNormals(MESH_DISP_VERTEX_NORMALS, 100.0f);

	//get 1 new mesh in table and get some basic information
	pDestMesh->m_pNode = (PVOID)node;
	DWORD NumVertex = pSrcMesh->getNumVerts();
	DWORD NumFaces  = pSrcMesh->getNumFaces();
	DWORD numCVx    = pSrcMesh->numCVerts;


	// Export the vertices
	Point3 *pSrcVertices = new Point3[NumVertex];
	KGLOG_PROCESS_ERROR(pSrcVertices);
	memset(pSrcVertices, 0, sizeof(Point3) * NumVertex);
	
	hrRetCode = _ExchangeMeshData_GetVertexPos(pSrcMesh, pSrcVertices, tm, t, node);
	KGLOG_COM_PROC_ERR_RET_ERR(hrRetCode);
	    
	// To determine visibility of a face, get the vertices in clockwise order.
	// If the objects has a negative scaling, we must compensate for that by
	// taking the vertices counter clockwise
	// we ignore the edge visibility,we need faces index only

	pExportFace = new KG_3D_EXPORT_FACE[NumFaces];
	KGLOG_PROCESS_ERROR(pExportFace);
	memset(pExportFace, 0, sizeof(KG_3D_EXPORT_FACE) * NumFaces);

    pNewVertex = new KG_3D_EXPORT_VERTEX[NumFaces * 3];
    KGLOG_PROCESS_ERROR(pNewVertex);
    memset(pNewVertex, 0, sizeof(KG_3D_EXPORT_VERTEX) * NumFaces * 3);

	
	for (i = 0; i < NumFaces; i++)
	{
	    for (j = 0; j < 3; j++)
	    {
		    pExportFace[i].Vertex[j].Diffuse.a = 1.0f;
		    pExportFace[i].Vertex[j].Diffuse.r = 0.0f;
		    pExportFace[i].Vertex[j].Diffuse.g = 1.0f;
		    pExportFace[i].Vertex[j].Diffuse.b = 0.0f;
		}
	}
	for (i =0; i < NumFaces; i++) 
	{
	    for (j = 0; j < 3; j++)
	    {
            pExportFace[i].dwIndex[j] = i * 3 + j;

	        pExportFace[i].Vertex[j].dwOrignIndex = pSrcMesh->faces[i].v[VertexOrder[j]];
	        
    	    pExportFace[i].Vertex[j].pt3Pos =  pSrcVertices[pExportFace[i].Vertex[j].dwOrignIndex];
	    }

		// This is the material ID for the face.
		pExportFace[i].dwMaterialID = pSrcMesh->faces[i].getMatID();

		//Calculate the count of subset
		setSubset.insert(pExportFace[i].dwMaterialID);

		hrRetCode = _ExchangeMeshData_GetTextureCoord_0(
		    i,
		    pExportFace,
		    node,
		    pSrcMesh,
		    VertexOrder
		);
		KGLOG_COM_PROC_ERR_RET_ERR(hrRetCode);
	    
		//Try to get channel 2 and channel 3 texture coordinate
		hrRetCode = _ExchangeMeshData_GetTextureCoord(
		    i,
		    pExportFace,
		    pSrcMesh,
		    2,
		    VertexOrder
		);
		KGLOG_COM_PROC_ERR_RET_ERR(hrRetCode);
		
		hrRetCode = _ExchangeMeshData_GetTextureCoord(
		    i,
		    pExportFace,
		    pSrcMesh,
		    3,
		    VertexOrder
		);
		KGLOG_COM_PROC_ERR_RET_ERR(hrRetCode);
	}
	// End face list

	// Export mesh vertices diffuse
	if (numCVx) 
	{
		for (i = 0; i < NumFaces; i++)
		{
		    hrRetCode = _ExchangeMeshData_Diffuse(
                i,
                pExportFace, 
                pSrcMesh, 
                VertexOrder
            );
    		KGLOG_COM_PROC_ERR_RET_ERR(hrRetCode);
		}
	}
	
	// Face and vertex normals.
	// In MAX a vertex can have more than one normal (but doesn't always have it).
	// This is depending on the face you are accessing the vertex through.	
	// To get all information we need to export all three vertex normals
	// for every face.
	
	IEditNormalsMod* pMod = NULL;
	IDerivedObject* iObject = NULL;
	Object* pObj = node->GetObjOrWSMRef();
	m_ip->SetCommandPanelTaskMode(TASK_MODE_MODIFY);
	if ( pObj->SuperClassID() == GEN_DERIVOB_CLASS_ID || pObj->SuperClassID() == DERIVOB_CLASS_ID )
	{
		iObject = static_cast<IDerivedObject*>(pObj);
		for ( int i = iObject->NumModifiers() - 1; i >= 0; i-- ) //find the edit normals modifier
		{
			Modifier* tmpMod = iObject->GetModifier(i);
			if ( tmpMod->ClassID() == Class_ID(0x4aa52ae3, 0x35ca1cde) )
			{
				pMod = (IEditNormalsMod*)tmpMod->GetInterface(EDIT_NORMALS_MOD_INTERFACE);
				if ( pMod->EnfnGetFaceDegree(0) == 0 )//Modifier not valid
				{
					pMod = NULL;
					break;
				}
				pMod->EnfnRecomputeNormals(node);
				break;
			}
		}
	}

	for (i = 0; i < NumFaces; i++)
	{
	    hrRetCode = _ExchangeMeshData_Normal(
            i,
			pMod,
			TMthis,
            pExportFace, 
            pSrcMesh, 
            VertexOrder
        );
		KGLOG_COM_PROC_ERR_RET_ERR(hrRetCode);
	}
	
    hrRetCode = _ExchangeMeshData_Optimize(
        NumFaces,
        pExportFace,
        pNewVertex,
        &nNewVertexNum
    );
    KGLOG_COM_PROC_ERR_RET_ERR(hrRetCode);


    hrRetCode = _ExchangeMeshData_SetDestMesh(
        NumFaces,
        pExportFace, 
        pNewVertex,
        nNewVertexNum,
        pDestMesh
    );
    KGLOG_COM_PROC_ERR_RET_ERR(hrRetCode);


    pDestMesh->m_dNumSubsets = 0;

	itSubset = setSubset.begin();
	while (itSubset != setSubset.end())
	{
		FixID.push_back(*itSubset);
		itSubset++;
	}

	sort(FixID.begin(), FixID.end());
	for (size_t i = 0; i < FixID.size(); i++)
	{
		IDRemap[FixID[i]] = i;
	}

	for (DWORD i = 0; i < pDestMesh->m_dNumFaces; i++)
	{
		DWORD dwOrgID = pDestMesh->m_lpSubsetID[i];
		pDestMesh->m_lpSubsetID[i] = IDRemap[dwOrgID];
	}

    pDestMesh->m_dNumSubsets = setSubset.size();
    
    hrRetCode = _ExchangeMeshData_CheckValide(
        NumVertex,
        pSrcVertices,
        pDestMesh
    );
    KGLOG_COM_PROC_ERR_RET_ERR(hrRetCode);

    
	

	//////////////////////////////////////////////////////////////////////////
	//Maybe will can fix the vetex normal here 
	
// 	m_ip->SetCommandPanelTaskMode(TASK_MODE_MODIFY);
// 
// 	int nFace = pSrcMesh->getNumFaces();
// 	
// 	nFace = pSrcMesh->getNumFaces();
// 	
// 	DWORD dwNormalFix = NewNumVerex;
// 	DWORD dwSize = NewNumVerex;
// 	DWORD dwIndex;
// 	vector<DWORD>* normalIndex = new vector<DWORD>[NewNumVerex];
// 	IEditNormalsMod* pMod = NULL;
// 
// 	Matrix3 TMthis   = node->GetObjTMAfterWSM(t);
// 	Matrix3 TMparent = node->GetParentNode()->GetObjTMAfterWSM(t);
// 	TMthis *= TMparent;
// 
// 	IDerivedObject* iObject = NULL;
// 	Object* pObj = node->GetObjOrWSMRef();
// 	if (pObj->SuperClassID() == GEN_DERIVOB_CLASS_ID || pObj->SuperClassID() == DERIVOB_CLASS_ID)
// 	{
// 		iObject = static_cast<IDerivedObject*>(pObj);
// 		for (int i = iObject->NumModifiers() - 1; i >= 0; i--) //find the edit normals modifier
// 		{
// 			Modifier* tmpMod = iObject->GetModifier(i);
// 			if (tmpMod->ClassID() == Class_ID(0x4aa52ae3, 0x35ca1cde))
// 			{
// 				pMod = (IEditNormalsMod*)tmpMod->GetInterface(EDIT_NORMALS_MOD_INTERFACE);
// 				if (pMod->EnfnGetFaceDegree(0) == 0)//Modifier not valid
// 				{
// 					pMod = NULL;
// 					break;
// 				}
// 				pMod->EnfnRecomputeNormals(node);
// 				break;
// 			}
// 		}
// 	}
// 
// 	nFace = pSrcMesh->getNumFaces();
// 	for (dwIndex = 0; dwIndex < pSrcMesh->getNumFaces(); dwIndex++)
// 	{
// 		DWORD vertexIndex[3]  = { pSrcMesh->faces[dwIndex].v[vx1], pSrcMesh->faces[dwIndex].v[vx2], pSrcMesh->faces[dwIndex].v[vx3] };
// 		DWORD vIFix[3]        = { SrcFaces[dwIndex * 3], SrcFaces[dwIndex * 3 + 1], SrcFaces[dwIndex * 3 + 2] };
// 		int vx[3] = { vx1, vx2, vx3	};
// 		for (int faceVIndex = 0; faceVIndex < 3; faceVIndex++)
// 		{
// 			Point3 vertexNormal;
// 			if (pMod)
// 			{
// 				//if exist a edit normal modifier
// 				if (pMod->EnfnGetFaceDegree(dwIndex) != 3)//Must be triangle object
// 					vertexNormal = TMthis.VectorTransform(GetVertexNormal(pSrcMesh, dwIndex, pSrcMesh->getRVertPtr(vertexIndex[faceVIndex])));
// 				else
// 				{
// 					int nNormalID = pMod->EnfnGetNormalID(dwIndex, vx[faceVIndex], node);
// 					vertexNormal = *(pMod->EnfnGetNormal(nNormalID, node));
// 					vertexNormal = TMthis.VectorTransform(vertexNormal);
// 				}
// 			}
// 			else
// 				vertexNormal = TMthis.VectorTransform(GetVertexNormal(pSrcMesh, dwIndex, pSrcMesh->getRVertPtr(vertexIndex[faceVIndex])));
// 			if (GetExchangeYZ())
// 			{
// 				float tmp = vertexNormal.y;
// 				vertexNormal.y = vertexNormal.z;
// 				vertexNormal.z = tmp;
// 			}
// 			
// 			if (!normalIndex[vertexIndex[faceVIndex]].size() || //if no normal info exist
// 				vertexIndex[faceVIndex] != vIFix[faceVIndex])   //if vertex information has been shifted
// 			{
// 				//We rewrite the normal for sure
// 				pVerticesNormalTemp[vIFix[faceVIndex]] = vertexNormal;
// 				normalIndex[vertexIndex[faceVIndex]].push_back(vIFix[faceVIndex]);	
// 			}
// 			else 
// 			{
// 				size_t j;
// 				for (j = 0; j < normalIndex[vertexIndex[faceVIndex]].size(); j++)
// 				{
// 					int CurNIndex = normalIndex[vertexIndex[faceVIndex]][j];
// 					//If the normal exist is equal to the one we got, leave it alone
// 					if (pVerticesNormalTemp[CurNIndex] == vertexNormal)						 
// 					{
// 						SrcFaces[dwIndex * 3 + faceVIndex] = CurNIndex;
// 						break;
// 					}
// 				}
// 				if (j == normalIndex[vertexIndex[faceVIndex]].size())//No equal instance found, open an new entry
// 				{
// 					
// 					pVerticesPosTemp[dwNormalFix]     = pVerticesPosTemp[vIFix[faceVIndex]];
// 					pVerticesTextTemp1[dwNormalFix]   = pVerticesTextTemp1[vIFix[faceVIndex]];
// 					pVerticesTextTemp2[dwNormalFix]   = pVerticesTextTemp2[vIFix[faceVIndex]];
// 					pVerticesTextTemp3[dwNormalFix]   = pVerticesTextTemp3[vIFix[faceVIndex]];
// 					pVerticesDiffuseTemp[dwNormalFix] = pVerticesDiffuseTemp[vIFix[faceVIndex]];
// 					pVerticesNormalTemp[dwNormalFix]  = vertexNormal;
// 					VerticeRemap[dwNormalFix]         = VerticeRemap[vIFix[faceVIndex]];
// 					SrcFaces[dwIndex * 3 + faceVIndex] = dwNormalFix;
// 					vector<DWORD>& CurVec = normalIndex[vertexIndex[faceVIndex]];
// 					CurVec.push_back(dwNormalFix);	
// 					dwNormalFix++;	
// 				}
// 			}
// 		}
// 	}
//
//	
//	//////////////////////////////////////////////////////////////////////////
//	//Clean up here
//	for(dwIndex = 0; dwIndex < NewNumVerex; dwIndex++)
//	{
//		normalIndex[dwIndex].clear();
//	}
//	delete[] normalIndex;
//	NewNumVerex = dwNormalFix;
//	//Clean up here end.
//	//////////////////////////////////////////////////////////////////////////

//	//Maybe will can fix the vertex normal here end
//	//////////////////////////////////////////////////////////////////////////
//	
//	//so ,put the right info.
//	Point3* VertexPos =  new Point3[NewNumVerex];
//  
//	Point3* VertexText1 = new Point3[NewNumVerex];
//	Point3* VertexText2 = new Point3[NewNumVerex];
//	Point3* VertexText3 = new Point3[NewNumVerex];
//
//	CMesh::LPCOLOR4 VertexDiffuse = new CMesh::COLOR4[NewNumVerex];
//
//	Point3* VertexNormal = new Point3[NewNumVerex];
//	DWORD*  VertexRemap = new DWORD[NewNumVerex];
//	for(DWORD n_Ver=0;n_Ver<NewNumVerex;n_Ver++)
//	{
//		VertexPos[n_Ver] = pVerticesPosTemp[n_Ver];
//		VertexText1[n_Ver] = pVerticesTextTemp1[n_Ver];
//		VertexText2[n_Ver] = pVerticesTextTemp2[n_Ver];
//		VertexText3[n_Ver] = pVerticesTextTemp3[n_Ver];
//
//		VertexDiffuse[n_Ver] = pVerticesDiffuseTemp[n_Ver];
//
//		VertexNormal[n_Ver] = pVerticesNormalTemp[n_Ver];
//		VertexRemap[n_Ver] = VerticeRemap[n_Ver];
//	}
//	//整理 SubsetRemap
//
//	pDestMesh->m_dNumVertices = NewNumVerex;
//	pDestMesh->m_lpFacesIndex = SrcFaces;
//	pDestMesh->m_lpSubsetID   = SrcMaterialID;

//	pDestMesh->m_lpVerticePosition = (CMesh::LPVEC3) VertexPos;
//	pDestMesh->m_lpVerticeTextCoord1 = (CMesh::LPVEC3) VertexText1;
//	pDestMesh->m_lpVerticeTextCoord2 = (CMesh::LPVEC3) VertexText2;
//	pDestMesh->m_lpVerticeTextCoord3 = (CMesh::LPVEC3) VertexText3;
//	pDestMesh->m_lpVerticeDiffuse = (CMesh::LPCOLOR4) VertexDiffuse;
//	pDestMesh->m_lpVerticeNormal = (CMesh::LPVEC3) VertexNormal;
//	pDestMesh->m_lpVerticeRemap = VertexRemap;

	//release temp date
//	SAFE_DELETE_ARRAY(pVerticesPosTemp);
//	SAFE_DELETE_ARRAY(pVerticesTextTemp1);
//	SAFE_DELETE_ARRAY(pVerticesTextTemp2);
//	SAFE_DELETE_ARRAY(pVerticesTextTemp3);
//	SAFE_DELETE_ARRAY(pVerticesDiffuseTemp);
//	SAFE_DELETE_ARRAY(pVerticesNormalTemp);
//	SAFE_DELETE_ARRAY(VerticeRemap);

	hrResult = S_OK;
Exit0:

    KG_DELETE_ARRAY(pNewVertex);
    KG_DELETE_ARRAY(pExportFace);
	KG_DELETE_ARRAY(pSrcVertices);

	return hrResult;
}


//HRESULT AsciiExp::ExchangeParticleArrayData(CMeshTable* pDestMeshTable,INode* node,TimeValue t)
//{
//	ObjectState os = node->EvalWorldState(t);
//	if (!os.obj || os.obj->SuperClassID()!=GEOMOBJECT_CLASS_ID) {
//		return E_FAIL; // Safety net. This shouldn't happen.
//	}
//	
//	Mesh* mesh = NULL;
//	Mesh* pTextMesh = NULL;
//	LPMESH pDestMesh = NULL;
//	
//	ParticleObject *po = GetParticleInterface(os.obj);
//	if(po==NULL) return E_FAIL;
//
//	Class_ID X = po->ClassID();
//	if(PArray_CLASS_ID != X)
//		return E_FAIL;
//	
//	PArrayParticle* pArray = (PArrayParticle*) po;
//	
//	Matrix3 tm = node->GetObjTMAfterWSM(t);
//	int pcount=pArray->parts.points.Count();
//	for(DWORD S =0;S<pcount;S++)
//	{
//		pArray->UpdateParticles(t,node);
//		Matrix3 Move;
//		Move.IdentityMatrix();
//		Point3 pos = pArray->ParticlePosition(t,S);
//		Move.Translate(pos);
//
//		mesh = &pArray->pmesh[S];
//		if(pArray->cmesh!=NULL)
//			pTextMesh = &pArray->cmesh[S];
//		
//		if(mesh==NULL) continue;
//		mesh->buildNormals();
//		pDestMeshTable->Get1NewMesh(&pDestMesh);
//		ExchangeMeshData(pDestMesh,node,mesh,pTextMesh,t,Move);
//		ExportMaterial(node,pDestMesh,0);
//		if(pDestMesh->m_lpMaterial==NULL)
//			pDestMesh->m_dNumSubsets = 0;
//		ExchangeParticleArrayAnimation(pDestMesh,node,mesh,pArray,S);
//		
//	}
//	return S_OK;
//}
//
//HRESULT AsciiExp::ExchangeParticleArrayAnimation(LPMESH pDestMesh,INode* node,Mesh* pSrcMesh,PArrayParticle* pArray,int ParticleIndex)
//{
//	ObjectState os = node->EvalWorldState(GetStaticFrame());
//	if (!os.obj)
//		return E_FAIL;
//		
//	// Get animation range
//	Interval animRange = m_ip->GetAnimRange();
//	// Get validity of the object
//	Interval objRange = os.obj->ObjectValidity(GetStaticFrame());
//
//	
//	// If the animation range is not fully included in the validity
//	// interval of the object, then we're animated.
//	if (!objRange.InInterval(animRange)) {
//		
//		DWORD NumFrame = 0;
//				
//		TimeValue t = animRange.Start();
//		//这个计算过程看起来如此复杂，不如让它试运行一次，看看到底多少桢
//		while (1) {
//			// This may seem strange, but the object in the pipeline
//			// might not be valid anymore.
//			os = node->EvalWorldState(t);
//			objRange = os.obj->ObjectValidity(t);
//			t = objRange.Start() < animRange.Start() ? animRange.Start() : objRange.Start();
//			
//			// Export the mesh definition at this frame
//			//ExportMesh(node, pMesh,t, indentLevel+1);
//			NumFrame++;
//			if (objRange.End() >= animRange.End()) {
//				break;
//			}
//			else {
//				t = (objRange.End()/GetTicksPerFrame()+GetMeshFrameStep()) * GetTicksPerFrame();
//			}
//		}
//		//申请内存，
//		Point3*  m_lpAnimationVerticesPosition = new Point3[pDestMesh->m_dNumVertices*NumFrame];
//		ZeroMemory(m_lpAnimationVerticesPosition ,sizeof(Point3)*pDestMesh->m_dNumVertices*NumFrame);
//
//		DWORD NowFrame = 0;
//		t = animRange.Start();
//		Point3 VLast = Zero;
//		Point3 PosLast = Zero;
//		Matrix3 Move,Rot;
//		Rot.IdentityMatrix();
//		while (1) {
//			// This may seem strange, but the object in the pipeline
//			// might not be valid anymore.
//			os = node->EvalWorldState(t);
//			objRange = os.obj->ObjectValidity(t);
//			t = objRange.Start() < animRange.Start() ? animRange.Start() : objRange.Start();
//			TimeValue tt = t;
//			if(GetStaticFrame()>t) tt = GetStaticFrame();
//
//			Matrix3 tm = node->GetObjTMAfterWSM(tt);
//			BOOL negScale = TMNegParity(tm);
//			int vx1, vx2, vx3;
//
//			pArray->UpdateParticles(tt,node);
//			
//			Move.IdentityMatrix();
//			Point3 pos = pArray->ParticlePosition(tt,ParticleIndex);
//			if(tt==GetStaticFrame())
//			{
//				PosLast = pos;
//			}
//			Point3 Vec = pos - PosLast;
//			Move.Translate(pos);
//			Rot.RotateX(Vec.x);	
//			Rot.RotateY(Vec.y);	
//			Rot.RotateZ(Vec.z);	
//			PosLast = pos;
//			// Order of the vertices. Get 'em counter clockwise if the objects is
//			// negatively scaled.
//			if (negScale) {
//				vx1 = 2;
//				vx2 = 1;
//				vx3 = 0;
//			}
//			else {
//				vx1 = 0;
//				vx2 = 1;
//				vx3 = 2;
//			}		
//			// Export the mesh definition at this frame
//			for(DWORD i=0;i<pDestMesh->m_dNumVertices;i++)
//			{
//				DWORD OldVerticeIndex = pDestMesh->m_lpVerticeRemap[i];
//				Point3 v = Rot * Move * pSrcMesh->verts[OldVerticeIndex];
//				if(v==Zero)
//					v = VLast;
//				else
//					VLast = v;
//				
//				if(GetExchangeYZ())
//				{
//					float Temp = v.z;
//					v.z = v.y;
//					v.y = Temp;
//				}
//
//				Point3 v0 = (*(Point3*)&pDestMesh->m_lpVerticePosition[i]);
//				m_lpAnimationVerticesPosition[i*NumFrame+NowFrame] =
//					v - v0;
//			}
//			
//			if (objRange.End() >= animRange.End()) {
//				break;
//			}
//			else {
//				t = (objRange.End()/GetTicksPerFrame()+GetMeshFrameStep()) * GetTicksPerFrame();
//			}
//			NowFrame++;
//			
//		}
//		//现在把整个动画过程一动不动的点去掉
//		Point3 Zero = Point3(0,0,0);
//
//		DWORD  NumAnimatedVer = 0;
//		BOOL * VerAnimated = new BOOL[pDestMesh->m_dNumVertices];
//		ZeroMemory(VerAnimated,sizeof(BOOL)*pDestMesh->m_dNumVertices);
//
//		for(DWORD i=0;i<pDestMesh->m_dNumVertices;i++)
//		{
//			for(DWORD n=0;n<NumFrame;n++)
//			{
//				Point3 v = m_lpAnimationVerticesPosition[i*NumFrame+n];
//				if(v!=Zero)
//				{
//					NumAnimatedVer++;
//					VerAnimated[i] = TRUE;
//					break;
//				}
//			}
//		}
//		//把结果保存到MESH中
//		pDestMesh->m_dNumAnimationVertices = NumAnimatedVer;
//		if(NumAnimatedVer!=0)
//		{
//			pDestMesh->m_dNumFrames = NumFrame;
//			pDestMesh->m_fFrameLength = GetTicksPerFrame();
//			Point3*  SrcAnimationVerticesPosition = new Point3[NumAnimatedVer*NumFrame];
//			ZeroMemory(SrcAnimationVerticesPosition ,sizeof(Point3)*NumAnimatedVer*NumFrame);
//			DWORD*   SrcAnimationVerticesIndex = new DWORD[NumAnimatedVer];
//			ZeroMemory(SrcAnimationVerticesIndex ,sizeof(DWORD)*NumAnimatedVer);
//			
//			DWORD K=0;
//			for(i=0;i<pDestMesh->m_dNumVertices;i++)
//			{
//				if(VerAnimated[i])
//				{
//					SrcAnimationVerticesIndex[K] = i;
//					for(DWORD n=0;n<NumFrame;n++)
//					{
//						Point3 v = m_lpAnimationVerticesPosition[i*NumFrame+n];
//						SrcAnimationVerticesPosition[K*NumFrame+n] = v;
//					}
//					K++;
//				}
//			}
//			pDestMesh->m_lpAnimationVerticesPosition = (CMesh::LPVEC3) SrcAnimationVerticesPosition;
//			pDestMesh->m_lpAnimationVerticesIndex = SrcAnimationVerticesIndex;
//		}
//		//释放内存
//		delete [] m_lpAnimationVerticesPosition;
//		delete [] VerAnimated;
//
//		pDestMesh->m_dwAnimationType = ANIMATION_VERTICES_RELATIVE;
//	}
//	
//	return S_OK;
//}

HRESULT AsciiExp::KG_AddBone(KG_BONE_CONTAINER_TABLE *pBoneContainerTable, INode *pBoneNode)
{
    HRESULT hrResult = E_FAIL;
    int nRetCode = false;
    int i = 0;
    int nIndex = 0;
    TSTR BoneName;
    pair<KG_BONE_CONTAINER_TABLE::iterator, bool> PairValue;

    KGLOG_PROCESS_ERROR(pBoneContainerTable);
    KGLOG_PROCESS_ERROR(pBoneNode);

    BoneName = pBoneNode->NodeName();
    UINT uSuffixIndex = strlen(BoneName) - 3;
    if (uSuffixIndex >= 0)
    {
        nRetCode = strcmp(BoneName + uSuffixIndex, "Nub");
        KG_PROCESS_SUCCESS(nRetCode == 0);
    }

    for (KG_BONE_CONTAINER_TABLE::iterator it = pBoneContainerTable->begin(); it != pBoneContainerTable->end(); ++it)
    {
        TSTR strCurBoneName;
        KG_PROCESS_SUCCESS((it->first) == pBoneNode);

        strCurBoneName = it->first->NodeName();
        nRetCode = strcmp(BoneName, strCurBoneName);
        if (nRetCode == 0)
        {
            char szOutputInfo[512];

            //如果骨骼名字相同，但是却不是同一个Node， 那么说明有同名的骨骼， 提示出错
            sprintf(szOutputInfo, "蒙皮中骨骼%s重名", BoneName);
            
            m_ExportError.AddError(szOutputInfo);
            goto Exit0;
        }
    }
    nIndex = pBoneContainerTable->size();
    
    PairValue = pBoneContainerTable->insert(make_pair(pBoneNode, nIndex));
    KGLOG_PROCESS_ERROR(PairValue.second);

Exit1:
    hrResult = S_OK;
Exit0:
    return hrResult;
}

INode *AsciiExp::KG_FindBoneByName(const KG_BONE_CONTAINER_TABLE &cBoneContainerTable, const char cszBoneName[])
{
    int nRetCode = false;

    for (KG_BONE_CONTAINER_TABLE::const_iterator it = cBoneContainerTable.begin(); it != cBoneContainerTable.end(); ++it)
    {
        TSTR strCurBoneName = it->first->NodeName();
        nRetCode = strcmp(strCurBoneName, cszBoneName);
        if (nRetCode == 0)
            return it->first;

    }
    return NULL;
}

HRESULT AsciiExp::KG_AddAllBone(KG_BONE_CONTAINER_TABLE *pBoneContainerTable)
{
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(pBoneContainerTable);

    for (KG_BONE_CONTAINER::iterator it = m_Bones.begin(); it != m_Bones.end(); ++it)
    {
        int nIndex       = pBoneContainerTable->size();
        INode *pBoneNode = *it;

        pair<KG_BONE_CONTAINER_TABLE::iterator, bool> PairValue = 
            pBoneContainerTable->insert(make_pair(pBoneNode, nIndex));

        KGLOG_PROCESS_ERROR(PairValue.second);
    }

    hrResult = S_OK;
Exit0:
    return hrResult;
}


void AsciiExp::AddBone(INode* pBoneNode,INode** pBoneNodeList,DWORD& NumBones)
{
	TSTR BoneName = pBoneNode->NodeName();
	TCHAR strOutputInfo[512];
	UINT uSuffixIndex = strlen(BoneName) - 3;
	if (!strcmp(BoneName + uSuffixIndex, "Nub")) //If suffix is Nub, the bone should be discard
		return;
	
	for (DWORD dwBoneIndex = 0; dwBoneIndex < NumBones; dwBoneIndex++)
	{
		TSTR strCurBoneName = pBoneNodeList[dwBoneIndex]->NodeName();
		if (!strcmp(BoneName, strCurBoneName) && 
			pBoneNodeList[dwBoneIndex] != pBoneNode)
		{
			//如果骨骼名字相同，但是却不是同一个Node， 那么说明有同名的骨骼， 提示出错
			sprintf(strOutputInfo, "蒙皮中骨骼%s重名", BoneName);
			MessageBox(m_ip->GetMAXHWnd(), strOutputInfo, "错误", MB_OK);
		}
	}
		
	//把检查到的骨骼添加到列表中
	for(DWORD i_Bone = 0;i_Bone<NumBones;i_Bone++)
	{
		if(pBoneNodeList[i_Bone] == pBoneNode)
		{
			break;
		}
	}
	if(i_Bone>=NumBones)
	{
		pBoneNodeList[NumBones]=pBoneNode;
		NumBones++;
	}
}

void AsciiExp::PreFindBones()
{
	for(KG_BONE_CONTAINER::iterator it = m_SelectedNode.begin(); it != m_SelectedNode.end(); ++it)
	{
		Modifier* pSkinMod =  FindSkinModifier(*it);
		
        if(pSkinMod)
		{
			ISkin* pSkin = (ISkin *)pSkinMod->GetInterface(I_SKIN);
			if(pSkin)
			{
				int  nNumBones = pSkin->GetNumBones();
				for (int j = 0; j < nNumBones; j++)
				{
					INode* pBoneNode = pSkin->GetBone(j);

                    Control* c = pBoneNode->GetTMController();
                    if ((c->ClassID() == BIPSLAVE_CONTROL_CLASS_ID) ||
                        (c->ClassID() == BIPBODY_CONTROL_CLASS_ID) ||
                        (c->ClassID() == FOOTPRINT_CLASS_ID))
                    {
                        IBipedExport* pBipExport = static_cast<IBipedExport*>(c->GetInterface(I_BIPINTERFACE));
                        pBipExport->RemoveNonUniformScale(TRUE);
                        c->ReleaseInterface(I_BIPINTERFACE, pBipExport);
                    }
                    TSTR strBoneName = pBoneNode->NodeName();
                    size_t  nNameLength = strlen(strBoneName);
                    if (nNameLength >= 8)
                    {
                        const TCHAR* strSuffix;
                        strSuffix = strBoneName + nNameLength - 7;
                        if (!strcmp(strSuffix, "Forearm"))
                        {
                            m_bForearm = TRUE;
                        }
                    }

					m_Bones.insert(pBoneNode);
				}
			}
		}
	}

}

void AsciiExp::FindAllBones(ISkin* pSkin, INode** pBoneNodeList, DWORD& NumBones)
{
	int i;
	m_bForearm = FALSE;
	const TCHAR* strSuffix;
	if (!pSkin)
		return;
	for(set<INode*>::iterator it = m_Bones.begin(); it != m_Bones.end(); it++)
	{
		AddBone(*it, pBoneNodeList, NumBones);
	}
	int  nNumBones = pSkin->GetNumBones();
	for (i = 0; i< nNumBones; i++)
	{
		INode* pBoneNode = pSkin->GetBone(i);

		Control* c = pBoneNode->GetTMController();
		if ((c->ClassID() == BIPSLAVE_CONTROL_CLASS_ID) ||
			(c->ClassID() == BIPBODY_CONTROL_CLASS_ID) ||
			(c->ClassID() == FOOTPRINT_CLASS_ID))
		{
			IBipedExport* pBipExport = static_cast<IBipedExport*>(c->GetInterface(I_BIPINTERFACE));
			pBipExport->RemoveNonUniformScale(TRUE);
			c->ReleaseInterface(I_BIPINTERFACE, pBipExport);
		}
		TSTR strBoneName = pBoneNode->NodeName();
		size_t  nNameLength = strlen(strBoneName);
		if (nNameLength >= 8)
		{
			strSuffix = strBoneName + nNameLength - 7;
			if (!strcmp(strSuffix, "Forearm"))
			{
				m_bForearm = TRUE;
			}
		}
		AddBone(pBoneNode, pBoneNodeList, NumBones);
	}
}

INode* AsciiExp::FindBaseBone(INode *pBone)
{
	while(1)
	{
		INode* pParentNode = pBone->GetParentNode();
		if(pParentNode==m_ip->GetRootNode())
			return pBone;
		pBone = pParentNode;
	}
}

INode* AsciiExp::FindBoneByName(const TCHAR* strBoneName, INode** ppNodeList, DWORD dwNumBones)
{
	INode* pNodeReturn = NULL;
	DWORD i;
	if (!strBoneName || !ppNodeList || dwNumBones <= 0)
		return NULL;
	for(i = 0; i < dwNumBones; i++)
	{
		TSTR strCurBoneName = ppNodeList[i]->NodeName();
		if (!strcmp(strCurBoneName, strBoneName))
		{
			pNodeReturn = ppNodeList[i];
			break;
		}
	}
	return pNodeReturn;
}

BOOL AsciiExp::IsBone(INode* pBoneNode, INode** ppNodeList, DWORD dwNumBones)
{
	BOOL bReturn = FALSE;
	DWORD dwIndex;
	for (dwIndex = 0; dwIndex < dwNumBones; dwIndex++)
	{
		TSTR strBoneName = pBoneNode->NodeName();
		if (pBoneNode == ppNodeList[dwIndex])
		{
			bReturn = TRUE;
			break;
		}
	}
	return bReturn;
}


INode* AsciiExp::FindBoneByName(const TCHAR* strBoneName, CMesh::LPBONEINFO pBoneInfo, DWORD dwNumBones)
{
	//Note: This function must be called before the CMesh::LPBONEINFO::Name is not changed to real node names
	//      That is the parameter in Name is still a pointer to the node, not the name
	INode* pNodeReturn = NULL;
	DWORD i;
	if (!strBoneName || !pBoneInfo || dwNumBones <= 0)
		return NULL;

	for (i = 0; i < dwNumBones; i++)
	{
		INode* pBoneNode = pBoneInfo[i].pThis;
		TSTR strCurBoneName = pBoneNode->NodeName();
		if (!strcmp(strBoneName, strCurBoneName))
		{
			pNodeReturn = pBoneNode;
			break;
		}
	}
	return pNodeReturn;
}


//Desc: AjustSkeleton的是用来处理这种情况的， 父骨骼没有被绑入Skin， 但是子骨骼却有动画，
//      这个问题在Bone Animation 导出是不会出现，因为那个保存的世界矩阵，但是在Relative， RTS
//      模式会出现问题，因为他们是使用的相对变换矩阵，所以需要重新连接子骨骼和父骨骼，新的父骨骼
//      可能是子骨骼的父系骨骼，但是不是直接连接到子骨骼的， 为了不破坏max文件中的骨骼关系导致出现
//      未知的问题，只是在插件中记录子父骨骼关系，并不改变骨骼Node间的关系
void AsciiExp::AdjustSkeleton(INode** ppNodeList, ISkin* pSkin, BOOL bForearmAdj)
{
	DWORD dwIndex;
	DWORD dwNumBones = m_Bones.size();//pSkin->GetNumBones();
	TCHAR strErrorInfo[512];
	for(dwIndex = 0; dwIndex < dwNumBones; dwIndex++)
	{
		INode* pBone = ppNodeList[dwIndex];//pSkin->GetBone(dwIndex);
		TSTR strBoneName = pBone->NodeName();
		INode* pParentNode;
		if (pBone != m_ip->GetRootNode())
		{
			pParentNode = pBone->GetParentNode();
			if (pParentNode != m_ip->GetRootNode() && !IsBone(pParentNode, ppNodeList, m_Bones.size()))//Parent bone is not in the bone list
			{
				if (bForearmAdj && CheckNodeSuffix(pParentNode, "Forearm"))//This is modified specially
					continue;
				if (m_relink.count(pBone) && m_relink[pBone] != pParentNode)
				{
					TSTR strBoneName = pBone->NodeName();
					sprintf(strErrorInfo, "骨骼%s有多于一个的父骨骼", strBoneName);
					MessageBox(m_ip->GetMAXHWnd(), strErrorInfo, "错误", MB_OK);
				}
				else
				{
					m_relink[pBone] = pParentNode;
					//Need to adjust the pParentNode's num of child
					if (m_NumrelinkChild.count(pParentNode))
						m_NumrelinkChild[pParentNode]->push_back(pBone);//If bone exist, add the relinked child bone count
					else
					{
						m_NumrelinkChild[pParentNode] = new std::vector<INode*>;//or add bone to the list
						m_NumrelinkChild[pParentNode]->push_back(pBone);
					}
				}
			}
		}
	}
}
BOOL AsciiExp::GetStdPlugsDir(LPSTR val)
{
    TCHAR szpath[MAX_PATH];
    TCHAR szDestDriver[MAX_PATH];
    TCHAR szDestpath[MAX_PATH];

    DWORD len = GetModuleFileName(NULL,szpath,MAX_PATH);
    _splitpath_s(szpath,
        szDestDriver,
        MAX_PATH,
        szDestpath,
        MAX_PATH,
        NULL,
        0,
        NULL,
        0);
    sprintf_s(val,MAX_PATH,"%s%s\plugcfg",szDestDriver,szDestpath);
    return TRUE;
}