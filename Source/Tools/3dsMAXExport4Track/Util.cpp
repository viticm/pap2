////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : Util.cpp
//  Version     : 1.0
//  Creator     : Zeb
//  Create Date : 2005-11-30 14:28:12
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Util.h"


////////////////////////////////////////////////////////////////////////////////

#include "3dsMAXExport4Track.h"

TCHAR *GetString(int id)
{
	static TCHAR buf[256];

	if (g_hInstance)
		return LoadString(g_hInstance, id, buf, sizeof(buf)) ? buf : NULL;

	return NULL;
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

HRESULT STrack::SaveToFile(const char cszFileName[])
{
    HRESULT hr = E_FAIL;

    DWORD dwFrameNum = vecMatrix.size();
    FILE *fpFile = fopen(cszFileName, "wb");
    if (!fpFile)
        goto Exit0;

    if (fwrite(this, 1, sizeof(STrackHead), fpFile) != sizeof(STrackHead))
        goto Exit0;
    
    if (
        fwrite(&vecMatrix[0], 1, dwFrameNum * sizeof(D3DXMATRIX), fpFile) !=
        dwFrameNum * sizeof(D3DXMATRIX)
    )
        goto Exit0;

    hr = S_OK;
Exit0:
    if (fpFile)
    {
        fclose(fpFile);
        fpFile = NULL;
    }
    return hr;
}