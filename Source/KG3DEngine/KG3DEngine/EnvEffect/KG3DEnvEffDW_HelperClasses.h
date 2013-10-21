////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DEnvEffDW_HelperClasses.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2006-12-19 10:34:54
//  Comment     : KG3DEnvEffDynamicWeather的帮助类
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DENVEFFDW_HELPERCLASSES_H_
#define _INCLUDE_KG3DENVEFFDW_HELPERCLASSES_H_

#if _KG3D_DEBUG_ENABLE_OUTPUT_DWGC == 1
#ifndef _KG3D_DEBUG_SELF_ENABLE_OUTPUT
#define _KG3D_DEBUG_SELF_ENABLE_OUTPUT	1
#define _DEFINED_ENABLE_DEBUG_OUTPUT_	
#endif
#endif


#include "KG3DEnvEffDW_KeyBase.h"

namespace KG3D_ENVIRONMENT_EFFECT
{
	/************************************************************************/
	//KG3DDWGC_InterpTaskStruct	和 KG3DDWGC_InterpTaskQueue 用于插值关键帧的分时计算队列
	/************************************************************************/
	typedef enum _INTERP_TASK_ENUM
	{
		interp_task_max = 100,
	}INTERP_TASK_ENUM;
	/*struct KG3DDWGC_InterpTaskStruct 
	{
		DWORD m_dwModificationOption;
		DWORD m_dwKeyIndexType;
		INT m_nResultKeyIndex;
		INT m_nPrevKeyIndex;
		INT m_nNextKeyIndex;
		float m_fPercent;
		KG3DDWGC_InterpTaskStruct(DWORD dwKeyIndexType, INT nRetIndex, INT nPrevIndex, INT nNexIndex, float fPercent)
			:m_dwKeyIndexType(dwKeyIndexType)
			,m_nResultKeyIndex(nRetIndex)
			,m_nPrevKeyIndex(nPrevIndex)
			,m_nNextKeyIndex(nNexIndex)
			,m_fPercent(fPercent)
		{
			
		}
		///使用系统生成的拷贝函数
		bool Calculate(KG3DDWGC_KeyTableManagerBase& TableMgr)
		{
			_ASSERTE(TableMgr.IsTypeLegal(m_dwKeyIndexType) >= 0);
			KG3DDWGC_KeyTableBase0& Table = TableMgr.GetTableByType(m_dwKeyIndexType);
			KG3DDWGC_KeyBase* pKeyResult = Table.GetKey(m_nResultKeyIndex);
			KG3DDWGC_KeyBase* pKeyPrev = Table.GetKey(m_nPrevKeyIndex);
			KG3DDWGC_KeyBase* pKeyNext = Table.GetKey(m_nNextKeyIndex);
			KG_COM_PROCESS_ERROR(pKeyResult && pKeyPrev && pKeyNext);
			return pKeyResult->CalculateInterp(*pKeyPrev, *pKeyNext, m_fPercent, &TableMgr);
Exit0:
			return false;
		}
	};*/
	/*class KG3DDWGC_InterpTaskQueue : public queue<KG3DDWGC_InterpTaskStruct>
	{
	protected:
		KG3DDWGC_KeyTableManagerBase& m_TableManager;
		KG3DDWGC_InterpTaskQueue(const KG3DDWGC_InterpTaskQueue&);
		typedef queue<KG3DDWGC_InterpTaskStruct> INTERP_TASK_QUEUE;
	public:
		KG3DDWGC_InterpTaskQueue(KG3DDWGC_KeyTableManagerBase& TableManager)
			:m_TableManager(TableManager)
		{

		}
		bool CalculateFrontTaskAndPop()
		{
			_ASSERTE(true != empty());
			bool bResult = (front()).Calculate(m_TableManager);
			pop();
			return bResult;
		}
		bool CalculateAllTasks()
		{
			bool bAllSucceeded = true;
			while(true != empty())
			{
				if (! CalculateFrontTaskAndPop())
				{
					bAllSucceeded = false;
				}
			}
			return bAllSucceeded;
		}
		void push(const value_type& _Val)///不能无限入队
		{
			if ((int)(this->size()) >= interp_task_max)
			{
				_ASSERTE(NULL&&"插值任务Queue溢出，非常不正常");
				return;
			}
			INTERP_TASK_QUEUE::push(_Val);
		}
	};	*/
};///KG3D_ENVIRONMENT_EFFECT

//using namespace KG3D_ENVIRONMENT_EFFECT;


#if defined(_DEFINED_ENABLE_DEBUG_OUTPUT_)
#if defined(_KG3D_DEBUG_SELF_ENABLE_OUTPUT)
#undef _KG3D_DEBUG_SELF_ENABLE_OUTPUT
#endif
#endif
#endif //_INCLUDE_KG3DENVEFFDW_HELPERCLASSES_H_
