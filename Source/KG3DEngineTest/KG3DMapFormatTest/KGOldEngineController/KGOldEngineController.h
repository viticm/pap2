//////////////////////////////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
//  FileName    :   KGOldEngineController.h    
//  Version     :   1.0
//  Creater     :   dinglong
//  Date        :   2009-9-15 10:00
//  Comment     :   封装加载旧的引擎dll的dll，导出管理加载的实例
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _KGOldEngineController_H__
#define _KGOldEngineController_H__

//获得引擎控制器的接口
extern "C" __declspec(dllexport) int GetController(void** pController);

#endif