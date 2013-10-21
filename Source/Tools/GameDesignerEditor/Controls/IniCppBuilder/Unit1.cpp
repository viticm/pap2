//---------------------------------------------------------------------------

#include <vcl.h>
#include <windows.h>
#pragma hdrstop
//---------------------------------------------------------------------------
//   Important note about DLL memory management when your DLL uses the
//   static version of the RunTime Library:
//
//   If your DLL exports any functions that pass String objects (or structs/
//   classes containing nested Strings) as parameter or function results,
//   you will need to add the library MEMMGR.LIB to both the DLL project and
//   any other projects that use the DLL.  You will also need to use MEMMGR.LIB
//   if any other projects which use the DLL will be performing new or delete
//   operations on any non-TObject-derived classes which are exported from the
//   DLL. Adding MEMMGR.LIB to your project will change the DLL and its calling
//   EXE's to use the BORLNDMM.DLL as their memory manager.  In these cases,
//   the file BORLNDMM.DLL should be deployed along with your DLL.
//
//   To avoid using BORLNDMM.DLL, pass string information using "char *" or
//   ShortString parameters.
//
//   If your DLL uses the dynamic version of the RTL, you do not need to
//   explicitly add MEMMGR.LIB as this will be done implicitly for you
//---------------------------------------------------------------------------
#pragma argsused
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fwdreason, LPVOID lpvReserved)
{
    return 1;
}
//---------------------------------------------------------------------------
#include <inifiles.hpp>


#ifdef DLL_EXPORTS
#define DLL_API extern "C" __declspec(dllexport)
#else
#define DLL_API extern "C" __declspec(dllimport)
#endif

//TIniFile* g_ini = NULL;

//注意，这里必须传输绝对路径，不然后面的API读不出来
DLL_API INT Init(LPCWSTR inifilename)
{
   WideString ws = inifilename;

   TIniFile *ini = NULL;
   if (FileExists(ws))
   {
        char szBuf[MAX_PATH];
        char* lpfilepart;
        String strini = WideString(inifilename);
        GetFullPathName(strini.c_str(), MAX_PATH - 1, szBuf, &lpfilepart);
        ini = new TIniFile( szBuf );
   }
   else
   {
        char szBuf[MAX_PATH];
        GetCurrentDirectory(MAX_PATH - 1, szBuf);
        String sdir = szBuf;
        sdir += "\\";
        String strini = WideString(inifilename);
        sdir += strini;
        if (FileExists(sdir))
        {
            ini = new TIniFile( sdir );
        }
        else
        {
            OutputDebugString(String(sdir + " 文件不存在!").c_str());
        }
   }
   //g_ini = ini;
   return (INT)ini;
}

DLL_API void UnInit(INT nHandle)
{
    TIniFile *ini = (TIniFile*) nHandle;
    delete ini;
}



DLL_API INT UpdateFile(INT nHandle)
{
    TIniFile *ini = (TIniFile*) nHandle;

    if (nHandle)
    {
        ini->UpdateFile();

        return true;
    }
    else
       return false;

}

DLL_API bool GetIniString(INT nHandle, LPCWSTR lpwstrSection, LPCWSTR lpwstrKey, LPWSTR lpwOut)
{
    TIniFile *ini = (TIniFile*) nHandle;

    if (nHandle)
    {
        //ini = new TIniFile( "E:\\Test\\bcb\\testDzhDll\\debug\\abc.ini" );

        String strSection = WideString(lpwstrSection);
        String strKey = WideString(lpwstrKey);
        String s =  ini->ReadString( strSection, strKey, "" );

        lstrcpyW(lpwOut, WideString(s).c_bstr());
            return true;
    }
    else
       return false;

}

DLL_API INT SetIniString(INT nHandle, LPCWSTR lpwstrSection, LPCWSTR lpwstrKey, LPCWSTR lpcwValue)
{
    TIniFile *ini = (TIniFile*) nHandle;

    if (nHandle)
    {
        String strSection = WideString(lpwstrSection);
        String strKey = WideString(lpwstrKey);
        ini->WriteString( strSection, strKey, lpcwValue);

        return true;
    }
    else
       return false;

}