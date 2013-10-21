////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : Util.h
//  Version     : 1.0
//  Creator     : Zeb
//  Create Date : 2005-11-30 14:26:43
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_UTIL_H_
#define _INCLUDE_UTIL_H_

////////////////////////////////////////////////////////////////////////////////

extern TCHAR *GetString(int id);
extern void Matrix3ToMatrix4(D3DXMATRIX& Mat, const Matrix3& Mat3);

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

struct STrackHead
{
    DWORD dwID;
    DWORD dwType;
    DWORD dwFrameNum;
    DWORD dwInterval;

    STrackHead()
    {
        dwID = MAKEFOURCC('T', 'R', 'A', 'C');
        dwType = 0;
        dwFrameNum = 0;
        dwInterval = 0;
    }
};

struct STrack
{
public:
    STrackHead  Head;
    std::vector<D3DXMATRIX> vecMatrix;

    HRESULT SaveToFile(const char cszFileName[]);
};

#endif //_INCLUDE_UTIL_H_
