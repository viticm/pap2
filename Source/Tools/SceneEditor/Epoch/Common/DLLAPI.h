// File: DLLAPI.h
// Desc: define dllexport for all dlls

#ifndef DLLAPI_H
#define DLLAPI_H

#include"DisableWarns.h"

#ifndef DLLAPI
#define DLLAPI __declspec(dllexport)
#endif

#define SafeDelete(p) { if(p) delete (p); (p) = 0; }
#define SafeDeleteArray(p) { if(p) delete [] (p); (p) = 0; }
#define SafeRelease(p) { if(p) (p)->Release(); (p) = 0; }

#endif