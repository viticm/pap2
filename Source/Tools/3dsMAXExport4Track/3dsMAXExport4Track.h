////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : 3dsMAXExport4Track.h
//  Version     : 1.0
//  Creator     : Zeb
//  Create Date : 2005-11-30 14:28:28
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_3DSMAXEXPORT4TRACK_H_
#define _INCLUDE_3DSMAXEXPORT4TRACK_H_

////////////////////////////////////////////////////////////////////////////////
#ifdef MAXEXPORT_EXPORTS
#define MAXEXPORT_API __declspec(dllexport)
#else
#define MAXEXPORT_API __declspec(dllimport)
#endif

#define TRACK_EXP_CLASS_ID	Class_ID(0x85548e1c, 0x4a26450d)
#define TRACK_EXPORT_VERSION			100				//version 1.00

extern HINSTANCE g_hInstance;
extern int g_controlsInit;

extern BOOL WINAPI DllMain( HINSTANCE hinstDLL,ULONG fdwReason,LPVOID lpvReserved ) ;
extern BOOL CALLBACK TrackExportOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
extern DWORD WINAPI fn(LPVOID arg);

class TrackExport : public SceneExport 
{
public:
	TrackExport();
	~TrackExport()	{}

	int				ExtCount()	{return 1;}
	const TCHAR *	Ext(int n)	{return _T("TRACK");}
	const TCHAR *	LongDesc()	{ return GetString(IDS_LIBDESCRIPTION); }
	const TCHAR *	ShortDesc()	{ return GetString(IDS_SHORTDESC); }
	const TCHAR *	AuthorName()	{ return _T("Zeb"); }
	const TCHAR *	CopyrightMessage()	{ return GetString(IDS_COPYRIGHT); }
	const TCHAR *	OtherMessage1()	{ return ""; }
	const TCHAR *	OtherMessage2()	{ return ""; }
	unsigned int	Version()	{ return TRACK_EXPORT_VERSION; }
	int				DoExport(const TCHAR *name, ExpInterface *ei, Interface *i,  BOOL suppressPrompts=FALSE,  DWORD options = 0 );
	void			ShowAbout(HWND hWnd)	{}

    // Add By ZhaoXinYu
    BOOL SupportsOptions(int ext, DWORD options); 



	bool NodeEnum( INode* pNode, int indentLevel );
	void ExportHelperObject( INode* node, int indentLevel ); 

private:
	Interface *i;
	ExpInterface *ei;

    std::string m_strFileName;

    int     m_nTrackCount;
};

class TrackExpClassDesc:public ClassDesc {
public:
	int				IsPublic() { return 1; }
	void*			Create(BOOL loading = FALSE) { return new TrackExport; } 
	const TCHAR*	ClassName() { return GetString( IDS_EXPNAME ); }
	SClass_ID		SuperClassID() { return SCENE_EXPORT_CLASS_ID; } 
	Class_ID		ClassID() { return TRACK_EXP_CLASS_ID; }
	const TCHAR*	Category() { return GetString(IDS_CATEGORY); }
};

static TrackExpClassDesc TrackExpDesc;


#endif //_INCLUDE_3DSMAXEXPORT4TRACK_H_
