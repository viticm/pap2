/************************************************************************
* @file      : igamept.h
* @author    : ChenZhiQiang <chenzhiqiang@kingsoft.com>
* @date      : 2009/11/19 10:04:52
* @brief     : 
*
* $Id: igamept.h,v 1.1 2009/12/07 10:18:40 kuangsihao Exp $
/************************************************************************/
#ifndef __IGAMEPT_H__
#define __IGAMEPT_H__

interface IGamePtCallback;


//IGamePt 实现类的GUID
// {154B1F41-3962-42a8-857D-24EF90A3BEB5}
__declspec(selectany) GUID CLSID_GamePtImpl = 
{ 0x154b1f41, 0x3962, 0x42a8, { 0x85, 0x7d, 0x24, 0xef, 0x90, 0xa3, 0xbe, 0xb5 } };


// -------------------------------------------------------------------------
/**
* @class        IGamePt
* @brief        密保通知反外挂接口定义
*
* @details      
*/
[
    uuid("7576C96D-2F57-4355-8449-FB749E79B1B9")
]
interface IGamePt: public IUnknown
{
    /**
    * @brief        初始化此模块时调用
    * @return       virtual STDMETHODIMP 
										S_OK    成功
										非S_OK  失败
    * @param        DWORD dwGameId  游戏ID 剑三的游戏ID 为 1120
    * @remark       
    */
    virtual STDMETHODIMP Init(DWORD dwGameId) = 0;


    /**
    * @brief        注册回调
    * @return       virtual STDMETHODIMP 
											S_OK    成功 
											非S_OK  失败
    * @param        IGamePtCallback* pGamePtCallBack
    * @remark       
    */
    virtual STDMETHODIMP RegCallback(IGamePtCallback* pGamePtCallBack) = 0;
    
    /**
    * @brief        反初始化此模块时调用
    * @return       S_OK    成功
    * @return       非S_OK  失败
    */
    virtual STDMETHODIMP Uninit(void) = 0;
};

// -------------------------------------------------------------------------
// $Log: igamept.h,v $
// Revision 1.1  2009/12/07 10:18:40  kuangsihao
// 换成duba的dll
//

#endif /* __IGAMEPT_H__ */
