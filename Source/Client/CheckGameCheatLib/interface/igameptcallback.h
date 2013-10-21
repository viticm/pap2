/************************************************************************
* @file      : igameptcallback.h
* @author    : ChenZhiQiang <chenzhiqiang@kingsoft.com>
* @date      : 2009/11/19 9:46:17
* @brief     : 
*
* $Id: igameptcallback.h,v 1.1 2009/12/07 10:18:40 kuangsihao Exp $
/************************************************************************/
#ifndef __IGAMEPTCALLBACK_H__
#define __IGAMEPTCALLBACK_H__

// -------------------------------------------------------------------------

/**
* @class        IGamePt
* @brief        密保通知外挂回调接口定义
*
* @details      
*/
[
    uuid("5B835A22-BB1F-42ba-9EE1-1E88E8917B29")
]
interface IGamePtCallback : public IUnknown
{
    /**
    * @brief        返回通知信息
    * @param        DWORD dwGameID             游戏id
    * @param        DWORD dwRabotID            外挂id
    * @param        const WCHAR* pszGamePath   游戏路径        
    * @param        const WCHAR* pszRabotPath  外挂路径   
    * @remark       
    */
    virtual STDMETHODIMP NotifyInfo(
                DWORD dwGameID, 
                DWORD dwRabotID,
                const WCHAR* pszGamePath,
                const WCHAR* pszRabotPath) = 0;
};

// -------------------------------------------------------------------------
// $Log: igameptcallback.h,v $
// Revision 1.1  2009/12/07 10:18:40  kuangsihao
// 换成duba的dll
//

#endif /* __IGAMEPTCALLBACK_H__ */
