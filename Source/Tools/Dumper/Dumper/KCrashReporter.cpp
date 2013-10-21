///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
// FileName : KCrashReporter.cpp
// Creator  : alex
// Date     : 
// Comment  :
//
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <string.h>
#include <string>
#include <atlbase.h>
#include <Winsock2.h>
#include <intrin.h>
#include <d3d9.h>
#include <ddraw.h>
#include "KCrashReporter.h"
using namespace std;

namespace
{
    tstring NewNode(tstring name)
    {
        tstring node = tstring(_T("<"));
        node.append(name);
        node.append(_T(" />\r\n"));
        return node;
    }

    void NodeAddAttr(tstring &node, tstring name, tstring val)
    {
        size_t pos = node.length() - 4;
        tstring tmp = name;
        tmp.append(_T("=\""));
        tmp.append(val);
        tmp.append(_T("\" "));
        node.insert(pos, tmp);
    }

    tstring WStrToTStr(const wchar_t* const src)
    {
#if defined(UNICODE) || defined(_UNICODE)
        return tstring(src);
#else
        int len = WideCharToMultiByte(CP_ACP, 0, src,
            -1, NULL, 0, NULL, NULL);
        TCHAR *tmp = new TCHAR[len];
        WideCharToMultiByte(CP_ACP, 0, src, -1, tmp,
            len, NULL, NULL);

        tstring result(tmp);
        delete[] tmp;
        return result;
#endif
    }

    tstring AStrToTStr(const char* const src)
    {
#if defined(UNICODE) || defined(_UNICODE)
        int len = MultiByteToWideChar(CP_ACP, 0, src,
            -1, NULL, 0);
        wchar_t *tmp = new wchar_t[len];
        MultiByteToWideChar(CP_ACP, 0, src, -1, tmp, len);
        tstring result(tmp);
        delete[] tmp;
        return result;
#else
        return tstring(src);
#endif
    }

    string TStrToUTF8(const tstring& src)
    {
#if defined(UNICODE) || defined(_UNICODE)
        int len = WideCharToMultiByte(CP_UTF8, 0, src.c_str(), -1, NULL, 0, NULL, NULL);
        char *tmp = new char[len];
        WideCharToMultiByte(CP_UTF8, 0, src.c_str(), -1, tmp, len, NULL, NULL);
        string result(tmp);
        delete[] tmp;
        return result;
#else
        int len = MultiByteToWideChar(CP_ACP, 0, src.c_str(), -1, NULL, 0);
        wchar_t *tmp = new wchar_t[len];
        MultiByteToWideChar(CP_ACP, 0, src.c_str(), -1, tmp, len);
        len = WideCharToMultiByte(CP_UTF8, 0, tmp, -1, NULL, 0, NULL, NULL);
        char *dst = new char[len];
        WideCharToMultiByte(CP_UTF8, 0, tmp, -1, dst, len, NULL, NULL);
        string result(dst);
        delete[] tmp;
        delete[] dst;
        return result;
#endif
    }

	tstring& TStrReplace(tstring& src, const tstring& pattern, const tstring& rep)
	{        
		tstring::size_type pos = src.find(pattern);
		while(pos != tstring::npos) {
			src.replace(pos, pattern.length(), rep);
            pos += pattern.length();
			pos = src.find(pattern, pos);
		}
		return src;
	}
}

BOOL CALLBACK 
KCrashReporter::DumpRptCallback(PVOID pCrashRpt,
                               CONST PMINIDUMP_CALLBACK_INPUT CallbackInput,
                               PMINIDUMP_CALLBACK_OUTPUT CallbackOutput)
{
    if (!CallbackInput || !CallbackOutput)
        return FALSE;

    if(ModuleCallback == CallbackInput->CallbackType)
    {
        KCrashReporter *reporter = static_cast<KCrashReporter*>(pCrashRpt);
        reporter->AddModule(CallbackInput->Module);
    }
    return TRUE;
}

void KCrashReporter::AddModule(MINIDUMP_MODULE_CALLBACK module)
{
    m_modules.push_back(module);
    m_modules.back().FullPath = _wcsdup(module.FullPath);
}

KCrashReporter::KCrashReporter(PEXCEPTION_POINTERS pExcp)
{    
    m_excpInfo = pExcp;
}

bool KCrashReporter::MakeReport(const TCHAR *pVersion, const TCHAR* const rptFile,  const TCHAR* const feedback)
{
    bool result = false;

    FILE *f = _tfopen(rptFile, _T("wb"));
    if(!f) {
        goto Exit0;
    }

    const char* const xmlDecl = "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\r\n";
    const char* const docBegin = "<Exception>\r\n";
    const char* const docEnd = "</Exception>";

    fwrite(xmlDecl, strlen(xmlDecl), 1, f);
    fwrite(docBegin, strlen(docBegin), 1, f);

    if(m_excpInfo) {
        WriteExceptionRecord(f, m_excpInfo->ExceptionRecord);
    }
    WriteCPURecord(f);
    WriteMemoryRecord(f);
    WriteVideoRecord(f);
    WriteOSRecord(f);
	WriteSenderInfo(f);
	WriteJx3ClientVersion(f, pVersion);
	WriteFeedback(f, feedback);
    WriteModuleRecord(f);
    WriteD3DCaps9(f);
    fwrite(docEnd, strlen(docEnd), 1, f);
    fclose(f);

    result = true;
Exit0:
    return result;
}

void KCrashReporter::WriteJx3ClientVersion(FILE *f, const TCHAR* const version)
{
	const TCHAR* const nodeName = _T("JX3Client");
	const TCHAR* const verName = _T("Version");
	tstring node = NewNode(nodeName);
	NodeAddAttr(node, verName, version);
	
	string s = TStrToUTF8(node);
	fwrite(s.c_str(), s.length(), 1, f);
}

void KCrashReporter::WriteFeedback(FILE *f, const TCHAR* const feedback )
{
	const TCHAR* const nodeName = _T("Feedback");
	const TCHAR* const contentName = _T("Content");
	
	tstring node = NewNode(nodeName);
	tstring tmp(feedback);
    TStrReplace(tmp, _T("&"), _T("&amp;"));
	TStrReplace(tmp, _T("\""), _T("&quot;"));
	TStrReplace(tmp, _T("`"), _T("&apos;"));	
	TStrReplace(tmp, _T("<"), _T("&lt;"));
	TStrReplace(tmp, _T(">"), _T("&gt;"));
	NodeAddAttr(node, contentName, tmp);
	string s = TStrToUTF8(node);
	fwrite(s.c_str(), s.length(), 1, f);
}

void KCrashReporter::WriteExceptionRecord(FILE *f, EXCEPTION_RECORD* pExceptionRecord)
{
    TCHAR *nodeName = _T("ExceptionRecord");
    TCHAR *modName = _T("ModuleName");
    TCHAR *typeName = _T("ExceptionType");
    TCHAR *descName = _T("ExceptionDescription");
    TCHAR *addrName = _T("ExceptionAddress");

    TCHAR nameBuf[_MAX_FNAME + 1];
    TCHAR buf[128];

    if(pExceptionRecord == NULL)
        return;

    GetModuleFileName(NULL, nameBuf, sizeof(nameBuf));
    tstring node = NewNode(nodeName);
    NodeAddAttr(node, modName, nameBuf);

    _sntprintf(buf, sizeof(buf), _T("%#x"), pExceptionRecord->ExceptionCode);
    NodeAddAttr(node, typeName, buf);

    tstring tmp;
    switch(pExceptionRecord->ExceptionCode)
    {
    case EXCEPTION_ACCESS_VIOLATION:
        tmp = _T("EXCEPTION_ACCESS_VIOLATION");
        break;
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
        tmp = _T("EXCEPTION_ARRAY_BOUNDS_EXCEEDED");
        break;
    case EXCEPTION_BREAKPOINT:
        tmp = _T("EXCEPTION_BREAKPOINT");
        break;
    case EXCEPTION_DATATYPE_MISALIGNMENT:
        tmp = _T("EXCEPTION_DATATYPE_MISALIGNMENT");
        break;
    case EXCEPTION_FLT_DENORMAL_OPERAND:
        tmp = _T("EXCEPTION_FLT_DENORMAL_OPERAND");
        break;
    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
        tmp = _T("EXCEPTION_FLT_DIVIDE_BY_ZERO");
        break;
    case EXCEPTION_FLT_INEXACT_RESULT:
        tmp = _T("EXCEPTION_FLT_INEXACT_RESULT");
        break;
    case EXCEPTION_FLT_INVALID_OPERATION:
        tmp = _T("EXCEPTION_FLT_INVALID_OPERATION");
        break;
    case EXCEPTION_FLT_OVERFLOW:
        tmp = _T("EXCEPTION_FLT_OVERFLOW");
        break;
    case EXCEPTION_FLT_STACK_CHECK:
        tmp = _T("EXCEPTION_FLT_STACK_CHECK");
        break;
    case EXCEPTION_FLT_UNDERFLOW:
        tmp = _T("EXCEPTION_FLT_UNDERFLOW");
        break;
    case EXCEPTION_ILLEGAL_INSTRUCTION:
        tmp = _T("EXCEPTION_ILLEGAL_INSTRUCTION");
        break;
    case EXCEPTION_IN_PAGE_ERROR:
        tmp = _T("EXCEPTION_IN_PAGE_ERROR");
        break;
    case EXCEPTION_INT_DIVIDE_BY_ZERO:
        tmp = _T("EXCEPTION_INT_DIVIDE_BY_ZERO");
        break;
    case EXCEPTION_INT_OVERFLOW:
        tmp = _T("EXCEPTION_INT_OVERFLOW");
        break;
    case EXCEPTION_INVALID_DISPOSITION:
        tmp = _T("EXCEPTION_INVALID_DISPOSITION");
        break;
    case EXCEPTION_NONCONTINUABLE_EXCEPTION:
        tmp = _T("EXCEPTION_NONCONTINUABLE_EXCEPTION");
        break;
    case EXCEPTION_PRIV_INSTRUCTION:
        tmp = _T("EXCEPTION_PRIV_INSTRUCTION");
        break;
    case EXCEPTION_SINGLE_STEP:
        tmp = _T("EXCEPTION_SINGLE_STEP");
        break;
    case EXCEPTION_STACK_OVERFLOW:
        tmp = _T("EXCEPTION_STACK_OVERFLOW");
        break;
    default:
        tmp = _T("UNKNOWN_EXCEPTION");
        break;
    }

    NodeAddAttr(node, descName, tmp);

    _sntprintf(buf, sizeof(buf), _T("%#p"), pExceptionRecord->ExceptionAddress);
    NodeAddAttr(node, addrName, buf);

    string s = TStrToUTF8(node);
    fwrite(s.c_str(), strlen(s.c_str()), 1, f);
    //_ftprintf(f, _T("%s"), node.c_str());
}

void KCrashReporter::WriteOSRecord(FILE *f)
{
    const TCHAR* const nodeName = _T("OperatingSystem");
    const TCHAR* const majorName = _T("MajorVersion");
    const TCHAR* const minorName = _T("MinorVersion");
    const TCHAR* const buildName = _T("BuildNumber");
    const TCHAR* const csdName = _T("CSDVersion");
	const TCHAR* const defaultUILang = _T("DefaultUILang");
    OSVERSIONINFO osInfo;

    tstring node = NewNode(nodeName);
    osInfo.dwOSVersionInfoSize = sizeof(osInfo);
    GetVersionEx(&osInfo);

    TCHAR tmp[128];
    _ultoa(osInfo.dwMajorVersion, tmp, 10);
    NodeAddAttr(node, majorName, tmp);
    _ultoa(osInfo.dwMinorVersion, tmp, 10);
    NodeAddAttr(node, minorName, tmp);
    _ultoa(osInfo.dwBuildNumber, tmp, 10);
    NodeAddAttr(node, buildName, tmp);
    NodeAddAttr(node, csdName, osInfo.szCSDVersion);
	NodeAddAttr(node, defaultUILang, GetUILang());

    string s = TStrToUTF8(node);
    fwrite(s.c_str(), strlen(s.c_str()), 1, f);
    //_ftprintf(f, _T("%s"), node.c_str());
}

void KCrashReporter::WriteModuleRecord(FILE *f)
{
    TCHAR *nodeName = _T("Module");
    TCHAR *fullPath = _T("FullPath");
    TCHAR *baseAddrName = _T("BaseAddress");
    TCHAR *sizeName = _T("Size");
    TCHAR *timeStampName = _T("TimeStamp");
    TCHAR *fileVerName = _T("FileVersion");
    TCHAR *prodVerName = _T("ProductVersion");
    TCHAR buf[128];

    const char* const modBegin = "<Modules>";
    const char* const modEnd = "</Modules>";
    fwrite(modBegin, strlen(modBegin), 1, f);

    for(vector<MINIDUMP_MODULE_CALLBACK>::iterator it = m_modules.begin();
        it != m_modules.end(); it++)
    {
        tstring node = NewNode(nodeName);

        tstring path = WStrToTStr(it->FullPath);
        NodeAddAttr(node, fullPath, path);


        _sntprintf(buf, sizeof(buf), _T("0x%08x"), it->BaseOfImage);
        NodeAddAttr(node, baseAddrName, buf);

        _sntprintf(buf, sizeof(buf), _T("0x%08x"), it->SizeOfImage);
        NodeAddAttr(node, sizeName, buf);

        FILETIME lastWriteTime;
        HANDLE hFile = CreateFile(path.c_str(), NULL, 
            FILE_SHARE_READ | FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);
        if(hFile) {
            GetFileTime(hFile, NULL, NULL, &lastWriteTime);
            SYSTEMTIME lwt;
            FileTimeToSystemTime(&lastWriteTime, &lwt);
            _sntprintf(buf, sizeof(buf), _T("%04u-%02u-%02u %02u:%02u:%02u"),
                lwt.wYear, lwt.wMonth, lwt.wDay,
                lwt.wHour, lwt.wMinute, lwt.wSecond);
            CloseHandle(hFile);
        }
        else {
            _sntprintf(buf, sizeof(buf), _T("UNKNOWN"));
        }
        NodeAddAttr(node, timeStampName, buf);

        _sntprintf(buf, sizeof(buf), _T("%d.%d.%d.%d"),
            HIWORD(it->VersionInfo.dwFileVersionMS),
            LOWORD(it->VersionInfo.dwFileVersionMS),
            HIWORD(it->VersionInfo.dwFileVersionLS),
            LOWORD(it->VersionInfo.dwFileVersionLS));
        NodeAddAttr(node, fileVerName, buf);

        _sntprintf(buf, sizeof(buf), _T("%d.%d.%d.%d"),
            HIWORD(it->VersionInfo.dwProductVersionMS),
            LOWORD(it->VersionInfo.dwProductVersionMS),
            HIWORD(it->VersionInfo.dwProductVersionLS),
            LOWORD(it->VersionInfo.dwProductVersionLS));
        NodeAddAttr(node, prodVerName, buf);

        string s = TStrToUTF8(node);
        fwrite(s.c_str(), strlen(s.c_str()), 1, f);
        //_ftprintf(f, _T("%s"), node.c_str());

    }
    fwrite(modEnd, strlen(modEnd), 1, f);
    //_ftprintf(f, _T("%s"), _T("</Modules>"));
}

void KCrashReporter::WriteMemoryRecord(FILE *f)
{
    const TCHAR* const nodeName = _T("Memory");
    const TCHAR* const totalPhysMem = _T("TotalPhysMem");
    const TCHAR* const freePhysMem = _T("FreePhysMem");

    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(memInfo);
    GlobalMemoryStatusEx(&memInfo);

    tstring node = NewNode(nodeName);

    TCHAR buf[128];
    _sntprintf(buf, sizeof(buf), _T("%.1fM"), memInfo.ullTotalPhys * 1.0 / (1024 * 1024));
    NodeAddAttr(node, totalPhysMem, buf);

    _sntprintf(buf, sizeof(buf), _T("%.1fM"), memInfo.ullAvailPhys*1.0/(1024*1024));
    NodeAddAttr(node, freePhysMem, buf);

    string s = TStrToUTF8(node);
    fwrite(s.c_str(), strlen(s.c_str()), 1, f);
    //_ftprintf(f, _T("%s"), node.c_str());

}

void KCrashReporter::WriteCPURecord(FILE *f)
{
    const TCHAR* const nodeName = _T("Processor");
    const TCHAR* const cpuNum = _T("CPUCount");
    const TCHAR* const cpuStringName = _T("CPUString");
    const TCHAR* const steppintIdName = _T("SteppingID");
    const TCHAR* const modelName = _T("Model");
    const TCHAR* const familyName = _T("Family");
    const TCHAR* const processorTypeName = _T("ProcessorType");
    const TCHAR* const cpuBrandName = _T("Brand");
    const TCHAR* const hyperThreadingName = _T("HyperThreading");

    int cpuInfo[4] = {-1};
    int steppingId = 0;
    int model = 0;
    int family = 0;
    int processorType = 0;
    int hyperThreading = 0;
    char cpuString[0x20] = {0};
    char cpuBrandString[0x40] = {0};
    unsigned ids, exIds;


    __cpuid(cpuInfo, 0);
    ids = cpuInfo[0];
    *((int*)cpuString) = cpuInfo[1];
    *((int*)(cpuString+4)) = cpuInfo[3];
    *((int*)(cpuString+8)) = cpuInfo[2];

    if(1 <= ids) {
        __cpuid(cpuInfo, 1);
        steppingId = cpuInfo[0] & 0xf;
        model = (cpuInfo[0] >> 4) & 0xf;
        family = (cpuInfo[0] >> 8) & 0xf;
        processorType = (cpuInfo[0] >> 12) & 0x3;
        hyperThreading = (cpuInfo[3] >> 28) & 0x1;
    }


    __cpuid(cpuInfo, 0x80000000);
    exIds = cpuInfo[0];
    if (0x80000002 <= exIds) {
        __cpuid(cpuInfo, 0x80000002);
        memcpy(cpuBrandString, cpuInfo, sizeof(cpuInfo));
        __cpuid(cpuInfo, 0x80000003);
        memcpy(cpuBrandString + 16, cpuInfo, sizeof(cpuInfo));
        __cpuid(cpuInfo, 0x80000004);
        memcpy(cpuBrandString + 32, cpuInfo, sizeof(cpuInfo));
    }

    SYSTEM_INFO si;
    GetSystemInfo(&si);

    TCHAR buf[128];
    tstring node = NewNode(nodeName);
    NodeAddAttr(node, cpuStringName, AStrToTStr(cpuString));
    NodeAddAttr(node, processorTypeName, _itot(processorType, buf, 10));
    NodeAddAttr(node, familyName, _itot(family, buf, 10));
    NodeAddAttr(node, modelName, _itot(model, buf, 10));
    NodeAddAttr(node, steppintIdName, _itot(steppingId, buf, 10));
    NodeAddAttr(node, cpuBrandName, AStrToTStr(cpuBrandString));

    _snprintf(buf, sizeof(buf), _T("%d"), si.dwNumberOfProcessors);
    NodeAddAttr(node, cpuNum, buf);

    _snprintf(buf, sizeof(buf), _T("%d"), hyperThreading);
    NodeAddAttr(node, hyperThreadingName, buf);

    string s = TStrToUTF8(node);
    fwrite(s.c_str(), strlen(s.c_str()), 1, f);
    //_ftprintf(f, _T("%s"), node.c_str());

}

void KCrashReporter::WriteVideoRecord(FILE *f)
{
    const TCHAR* const nodeName = _T("VideoCard");
    const TCHAR* const descName = _T("Description");
    const TCHAR* const driverDesc = _T("DriverDescription");
    const TCHAR* const driverVer = _T("DriverVersion");
    const TCHAR* const vendorIdName = _T("VendorID");
    const TCHAR* const deviceIdName = _T("DeviceID");
    const TCHAR* const subSysIdName = _T("SubSysID");
    const TCHAR* const revisionName = _T("Revision");
    const TCHAR* const videoMemTotalName = _T("TotalVideoMem");
    const TCHAR* const videoMemFreeName = _T("FreeVideoMem");

    LPDIRECT3D9 pD3D = NULL;
    LPDIRECT3DDEVICE9 pD3DDevice = NULL;

    if( NULL == ( pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
        return;

    /*
    D3DPRESENT_PARAMETERS d3dpp; 
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    if( FAILED( pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, 
        GetDesktopWindow(),
        D3DCREATE_SOFTWARE_VERTEXPROCESSING,
        &d3dpp, &pD3DDevice ) ) ) {
            return;
    }
    */
    tstring node = NewNode(nodeName);
    TCHAR buf[128];

    D3DADAPTER_IDENTIFIER9 cardInfo;
    pD3D->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &cardInfo);
    NodeAddAttr(node, descName, AStrToTStr(cardInfo.Description));
    NodeAddAttr(node, driverDesc, AStrToTStr(cardInfo.Driver));

    _sntprintf(buf, sizeof(buf), _T("%d.%d.%d.%d"),
        HIWORD(cardInfo.DriverVersion.HighPart),
        LOWORD(cardInfo.DriverVersion.HighPart),
        HIWORD(cardInfo.DriverVersion.LowPart),
        LOWORD(cardInfo.DriverVersion.LowPart));
    NodeAddAttr(node, driverVer, buf);

    _sntprintf(buf, sizeof(buf), _T("%d"), cardInfo.VendorId);
    NodeAddAttr(node, vendorIdName, buf);

    _sntprintf(buf, sizeof(buf), _T("%d"), cardInfo.DeviceId);
    NodeAddAttr(node, deviceIdName, buf);

    _sntprintf(buf, sizeof(buf), _T("%d"), cardInfo.SubSysId);
    NodeAddAttr(node, subSysIdName, buf);

    _sntprintf(buf, sizeof(buf), _T("%d"), cardInfo.Revision);
    NodeAddAttr(node, revisionName, buf);

    KCrashReporter::VideoMemInfo vidMemInfo = GetVideoMemInfo();
    if(vidMemInfo.totalMem != -1) {
        _sntprintf(buf, sizeof(buf), _T("%d"), vidMemInfo.totalMem);
        NodeAddAttr(node, videoMemTotalName, buf);
    }
    else {
        NodeAddAttr(node, videoMemTotalName, _T("Unknown"));
    }

    if(vidMemInfo.freeMem != -1) {
        _sntprintf(buf, sizeof(buf), _T("%d"), vidMemInfo.freeMem);
        NodeAddAttr(node, videoMemFreeName, buf);
    }
    else {
        NodeAddAttr(node, videoMemFreeName, _T("Unknown"));
    }
    string s = TStrToUTF8(node);
    fwrite(s.c_str(), strlen(s.c_str()), 1, f);
    //_ftprintf(f, _T("%s"), node.c_str());
}

KCrashReporter::VideoMemInfo KCrashReporter::GetVideoMemInfo()
{
    LPDIRECTDRAW2   lpdd;  
    HRESULT   ddrval;  
    KCrashReporter::VideoMemInfo vidMemInfo = {-1, -1};

    CoInitialize(NULL);  

    ddrval = CoCreateInstance(CLSID_DirectDraw,  
        NULL,   CLSCTX_ALL,   IID_IDirectDraw7,   (void**)&lpdd);  


    if(FAILED(ddrval))
        goto Exit0;

    ddrval   =   IDirectDraw7_Initialize(lpdd,   NULL);  

    DDCAPS   ddcaps;  
    ddcaps.dwSize   =   sizeof   DDCAPS;  
    lpdd->GetCaps(&ddcaps,   NULL);  

    lpdd->Release();  

    vidMemInfo.totalMem = ddcaps.dwVidMemTotal / (1024*1024);
    vidMemInfo.freeMem = ddcaps.dwVidMemFree / (1024*1024);

Exit0:
    CoUninitialize();
    return vidMemInfo;
}

void KCrashReporter::WriteSenderInfo(FILE *f)
{
	const TCHAR* const nodeName = _T("Sender");
	const TCHAR* const hostName = _T("HostName");
	const TCHAR* const hostAddr = _T("HostAddr");

	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;
	int err;

	tstring node = NewNode(nodeName);
	char buf[256] = {0};
	hostent *pHost = NULL;
	string s;

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		return;
	}
	if(LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		goto Exit0;
	}

	gethostname(buf, sizeof(buf));
	pHost = gethostbyname(buf);
	in_addr addr;
	memcpy(&addr, pHost->h_addr_list[0], sizeof(addr));
	NodeAddAttr(node, hostName, AStrToTStr(pHost->h_name));
	NodeAddAttr(node, hostAddr, AStrToTStr(inet_ntoa(addr)));

	s = TStrToUTF8(node);
	fwrite(s.c_str(), strlen(s.c_str()), 1, f);

Exit0:
	WSACleanup();
}

tstring KCrashReporter::GetUILang()
{
	LANGID langId = GetSystemDefaultUILanguage();
	DWORD lcid = MAKELCID(langId, SORT_DEFAULT);
	int len = GetLocaleInfo(lcid, LOCALE_SENGLANGUAGE, NULL, 0);
	TCHAR *tmp = new TCHAR[len];
	GetLocaleInfo(lcid, LOCALE_SENGLANGUAGE, tmp, len);
	tstring result(tmp);
	delete[] tmp;
	return result;
}

void KCrashReporter::WriteD3DCaps9(FILE *f)
{            
    HRESULT hrRetCode = S_FALSE;
    const TCHAR* const nodeName = _T("D3DCaps9");
    const TCHAR* const modBegin = _T("<D3DCaps9>");
    const TCHAR* const modEnd = _T("</D3DCaps9>");
    TCHAR szValue[32] = {0};
    tstring node;
    string strUTF8;

    LPDIRECT3D9 pD3d = NULL;
    D3DCAPS9 caps;
    ZeroMemory(&caps, sizeof(caps));

    if (NULL == (pD3d = Direct3DCreate9(D3D_SDK_VERSION)))
        return;

    hrRetCode = pD3d->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);
    if (FAILED(hrRetCode))
        return;
    
    fwrite(modBegin, strlen(modBegin), 1, f);

    WriteD3DCaps9OneNode(nodeName, _T("ShadeCaps"), (DWORD *)&caps.ShadeCaps, d3d_dword, f);
    WriteD3DCaps9OneNode(nodeName, _T("VertexShaderVersion"), (DWORD *)&caps.VertexShaderVersion, d3d_dword, f);
    WriteD3DCaps9OneNode(nodeName, _T("MaxVertexShaderConst"), (DWORD *)&caps.MaxVertexShaderConst, d3d_dword, f);
    WriteD3DCaps9OneNode(nodeName, _T("PixelShaderVersion"), (DWORD *)&caps.PixelShaderVersion, d3d_dword, f);
    WriteD3DCaps9OneNode(nodeName, _T("VertexProcessingCaps"), (DWORD *)&caps.VertexProcessingCaps, d3d_dword, f);
    WriteD3DCaps9OneNode(nodeName, _T("MaxVertexBlendMatrices"), (DWORD *)&caps.MaxVertexBlendMatrices, d3d_dword, f);
    WriteD3DCaps9OneNode(nodeName, _T("MaxVertexBlendMatrixIndex"), (DWORD *)&caps.MaxVertexBlendMatrixIndex, d3d_dword, f);
    WriteD3DCaps9OneNode(nodeName, _T("MaxPrimitiveCount"), (DWORD *)&caps.MaxPrimitiveCount, d3d_dword, f);
    WriteD3DCaps9OneNode(nodeName, _T("TextureCaps"), (DWORD *)&caps.TextureCaps, d3d_dword, f);
    WriteD3DCaps9OneNode(nodeName, _T("MaxTextureWidth"), (DWORD *)&caps.MaxTextureWidth, d3d_dword, f);
    WriteD3DCaps9OneNode(nodeName, _T("MaxTextureHeight"), (DWORD *)&caps.MaxTextureHeight, d3d_dword, f);
    WriteD3DCaps9OneNode(nodeName, _T("MaxTextureRepeat"), (DWORD *)&caps.MaxTextureRepeat, d3d_dword, f);
    WriteD3DCaps9OneNode(nodeName, _T("MaxTextureAspectRatio"), (DWORD *)&caps.MaxTextureAspectRatio, d3d_dword, f);
    WriteD3DCaps9OneNode(nodeName, _T("VertexTextureFilterCaps"), (DWORD *)&caps.VertexTextureFilterCaps, d3d_dword, f);

    fwrite(modEnd, strlen(modEnd), 1, f);    
}

void KCrashReporter::WriteD3DCaps9OneNode(tstring node, tstring name, void* val, D3DCAPS9_FILED_TYPE valType, FILE *f)
{
    const TCHAR* const filedName = _T("name");
    const TCHAR* const valueName = _T("value");
    TCHAR szValue[32] = {0};
    tstring strNode;
    string strUTF8;

    switch(valType)
    {
        case d3d_dword:
        case d3d_uint:
            _snprintf(szValue, sizeof(szValue) / sizeof(TCHAR) - 1, _T("%.8x"), *((DWORD *)val));
            break;
        case d3d_float:
            _snprintf(szValue, sizeof(szValue) / sizeof(TCHAR) - 1, _T("%f"), *((float *)val));
            break;
        default:
            return;
    }

    strNode = NewNode(node);
    NodeAddAttr(strNode, filedName, name);
    NodeAddAttr(strNode, valueName, szValue);
    strUTF8 = TStrToUTF8(strNode);
    fwrite(strUTF8.c_str(), strlen(strUTF8.c_str()), 1, f);
}