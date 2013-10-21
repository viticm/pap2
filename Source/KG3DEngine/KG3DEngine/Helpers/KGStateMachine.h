////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGStateMachine.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-10-20 10:50:29
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KGSTATEMACHINE_H_
#define _INCLUDE_KGSTATEMACHINE_H_

#include "KG3DCommonObject.h"
////////////////////////////////////////////////////////////////////////////////
enum
{
	KG_STATE_MACHINE_SELF = 0,	//标识切状态时返回自己状态的特殊保留字，任何状态不能以SELF开头
};
interface KGStateMachineState;

struct KGStateMachineState : public KG3DCommonObjectSimple
{
	STDMETHOD_(ULONG, GetType)();
	virtual DWORD	GetStateID() = 0;
	virtual KGStateMachineState* Clone(DWORD_PTR param) = 0;	//自己实现内存管理，如果是static型的Singleton，返回this就好了。释放被用Release
	virtual DWORD	CheckInput(DWORD_PTR Input) = 0;//参数1，如果是Mealy型，自己转换成输入结构，如果是Moore型，忽略掉就好了。返回是下一个状态的ID
};

class KGStateMachine : public KG3DCommonObjectSimple
{
public:
	enum
	{
		em_max_state_count = 64,
	};

	struct BuildStruct 
	{
		KGStateMachineState**	pStates;
		size_t					uStateCount;
		DWORD					dwInitStateID;
	};

	static KGStateMachine* Build(LPCTSTR, DWORD_PTR pBuildStruct, DWORD_PTR);
	
	STDMETHOD_(ULONG, GetType)();
	STDMETHOD(Init)(KGStateMachineState** states, size_t uStateCount, DWORD InitID);//ID必须连续，初始State必须是State组其中一个
	VOID CheckInput(DWORD_PTR Input);
	KGStateMachineState&	GetCurState();
private:
	KGStateMachine();
	STDMETHOD(UnInit)();
	std::vector<KGStateMachineState*>	m_States;
	size_t						m_uStateCount;
	DWORD						m_dwFirstID;
	DWORD						m_dwCurStateID;
};

#endif //_INCLUDE_KGSTATEMACHINE_H_
