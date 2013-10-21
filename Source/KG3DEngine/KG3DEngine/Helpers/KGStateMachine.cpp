////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGStateMachine.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-10-20 10:50:27
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KGStateMachine.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////
ULONG STDMETHODCALLTYPE KGStateMachineState::GetType()
{
	return KG3DTYPE_STATE_MACHINE_STATE;
}
KGStateMachine::KGStateMachine()
	:m_dwCurStateID(0)
	,m_dwFirstID(0)
{
	
}

ULONG STDMETHODCALLTYPE KGStateMachine::GetType()
{
	return KG3DTYPE_STATE_MACHINE;
}

KGStateMachine* KGStateMachine::Build( LPCTSTR, DWORD_PTR pBuildStruct, DWORD_PTR )
{
	KGStateMachine* p = new KGStateMachine;
	KG_PROCESS_ERROR(NULL != p);
	{
		BuildStruct* pBS = reinterpret_cast<BuildStruct*>(pBuildStruct);
		KG_PROCESS_ERROR(NULL != pBS);
		HRESULT hr = p->Init(pBS->pStates, pBS->uStateCount, pBS->dwInitStateID);
		KG_COM_PROCESS_ERROR(hr);
		return p;
	}
Exit0:
	SAFE_RELEASE(p);
	return NULL;
}
VOID KGStateMachine::CheckInput( DWORD_PTR Input )
{
	KGStateMachineState& curState = GetCurState();
	DWORD newID = curState.CheckInput(Input);
	if (newID != curState.GetStateID())
	{
		_ASSERTE(newID < m_dwFirstID + m_States.size());

		m_dwCurStateID = m_States[newID - m_dwFirstID]->GetStateID();
	}
}

HRESULT STDMETHODCALLTYPE KGStateMachine::Init( KGStateMachineState** states, size_t uStateCount, DWORD InitID)
{
	_ASSERTE(0 == m_States.size() && _T("重复初始化是不允许的"));
	_ASSERTE(states && uStateCount < em_max_state_count && uStateCount > 0);

#if defined(DEBUG) | defined(_DEBUG)
	for (size_t i = 0; i < uStateCount; ++i)
	{
		_ASSERTE(NULL != states[i]);
	}
	for (size_t i = 0; i < uStateCount - 1; ++i)
	{
		_ASSERTE(states[i]->GetStateID() + 1 == states[i+1]->GetStateID() && _T("ID必须连续"));
	}
	DWORD FirstID = states[0]->GetStateID();
	_ASSERTE(InitID >= FirstID && InitID < FirstID + uStateCount && _T("初始State必须是State组里面的其中一个"));
#endif

	m_States.reserve(uStateCount);
	for (size_t i = 0; i < uStateCount; ++i)
	{
		KGStateMachineState* p = states[i]->Clone(NULL);
		KG_PROCESS_ERROR(NULL != p);
		m_States.push_back(p);
	}

	_ASSERTE(m_States.size() == uStateCount);

	m_dwFirstID = m_States[0]->GetStateID();
	m_dwCurStateID = m_States[InitID - m_dwFirstID]->GetStateID();
	return S_OK;
Exit0:
	KGCH::TContainerRelease(m_States);
	return E_FAIL;
}

KGStateMachineState& KGStateMachine::GetCurState()
{
	DWORD dwPos = m_dwCurStateID - m_dwFirstID;
	_ASSERTE((size_t)dwPos < m_States.size());
	return *m_States[dwPos];
}

HRESULT STDMETHODCALLTYPE KGStateMachine::UnInit()
{
	return KGCH::TContainerRelease(m_States);
}

