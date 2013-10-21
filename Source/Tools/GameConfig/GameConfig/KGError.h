//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KGError.h
//  Version     :   1.0
//  Creater     :   Freeway Chen
//  Date        :   2004-12-17 10:32:32
//  Comment     :   Kingsoft Game Public Error Code
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _KGERROR_H_
#define _KGERROR_H_    1


#ifdef WIN32

	// if Win32 platform
	#include <Windows.h>
	#include <Unknwn.h>

#else	// #ifndef WIN32

	#ifndef _HRESULT_DEFINED
	#define _HRESULT_DEFINED

	typedef long HRESULT;

	#endif // !_HRESULT_DEFINED

	#ifndef SUCCEEDED
	#define SUCCEEDED(Status) ((HRESULT)(Status) >= 0)
	#endif

	#ifndef FAILED
	#define FAILED(Status) ((HRESULT)(Status) < 0)
	#endif

	#ifndef _HRESULT_TYPEDEF_
	#define _HRESULT_TYPEDEF_(_sc) ((HRESULT)_sc)
	#endif

#endif	// #ifdef WIN32 #else


//=============================================================================
//
// Error Code From WinError.h Start
//
//=============================================================================

//
// Success codes
//
#define KG_S_OK                             _HRESULT_TYPEDEF_(0x00000000L)
#define KG_S_FALSE                          _HRESULT_TYPEDEF_(0x00000001L)

//
// MessageId: KG_E_NOTIMPL
//
// MessageText:
//
//  Not implemented
//
#define KG_E_NOTIMPL                        _HRESULT_TYPEDEF_(0x80004001L)

//
// MessageId: KG_E_OUTOFMEMORY
//
// MessageText:
//
//  Ran out of memory
//
#define KG_E_OUTOFMEMORY                    _HRESULT_TYPEDEF_(0x8007000EL)

//
// MessageId: KG_E_INVALIDARG
//
// MessageText:
//
//  One or more arguments are invalid
//
#define KG_E_INVALIDARG                     _HRESULT_TYPEDEF_(0x80070057L)

//
// MessageId: KG_E_NOINTERFACE
//
// MessageText:
//
//  No such interface supported
//
#define KG_E_NOINTERFACE                    _HRESULT_TYPEDEF_(0x80004002L)

//
// MessageId: KG_E_POINTER
//
// MessageText:
//
//  Invalid pointer
//
#define KG_E_POINTER                        _HRESULT_TYPEDEF_(0x80004003L)

//
// MessageId: KG_E_HANDLE
//
// MessageText:
//
//  Invalid handle
//
#define KG_E_HANDLE                         _HRESULT_TYPEDEF_(0x80070006L)

//
// MessageId: KG_E_ABORT
//
// MessageText:
//
//  Operation aborted
//
#define KG_E_ABORT                          _HRESULT_TYPEDEF_(0x80004004L)

//
// MessageId: KG_E_FAIL
//
// MessageText:
//
//  Unspecified error
//
#define KG_E_FAIL                           _HRESULT_TYPEDEF_(0x80004005L)

//
// MessageId: KG_E_ACCESSDENIED
//
// MessageText:
//
//  General access denied error
//
#define KG_E_ACCESSDENIED                   _HRESULT_TYPEDEF_(0x80070005L)

//=============================================================================
//
// Error Code From WinError.h    End
//
//=============================================================================

//=============================================================================
//
// Error Code for Kingsoft Game    Start
//
//=============================================================================


//
// MessageId: KG_E_TIMEOUT
//
// MessageText:
//
//  Timeout
//
#define KG_E_TIMEOUT                        _HRESULT_TYPEDEF_(0x80880001L)


//
// MessageId: KG_E_SOCKET
//
// MessageText:
//
//  socket error
//
#define KG_E_SOCKET                        _HRESULT_TYPEDEF_(0x80880002L)

//
// MessageId: KG_E_NO_DATA
//
// MessageText:
//
//  data not arrive
//
#define KG_E_NO_DATA                        _HRESULT_TYPEDEF_(0x80880003L)

//
// MessageId: KG_E_DATA_NO_FOUND
//
// MessageText:
//
//  data not found in out queue
//
#define KG_E_DATA_NO_FOUND                  _HRESULT_TYPEDEF_(0x80880004L)

//=============================================================================
//
// Error Code for Kingsoft Game    End
//
//=============================================================================


#endif // _KGERROR_H_
