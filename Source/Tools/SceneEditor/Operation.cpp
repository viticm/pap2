// Operation.cpp: implementation of the KEditOperation class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SceneEditor.h"
#include "Operation.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

KEditOperation::KEditOperation()
{

}

KEditOperation::~KEditOperation()
{

}

HRESULT KEditOperation::Do()
{
	return S_OK;
}

HRESULT KEditOperation::Undo()
{
	return S_OK;
}

KEditOperationHistory::KEditOperationHistory()
{
	;
}

KEditOperationHistory::~KEditOperationHistory()
{
	;
}