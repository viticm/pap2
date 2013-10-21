//////////////////////////////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
//  FileName    :   KGLoadController.h    
//  Version     :   1.0
//  Creater     :   dinglong
//  Date        :   2009-6-10 10:00
//  Comment     :   封装加载旧的引擎dll的dll，导出管理加载的实例
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _OldMaterialLoad_H__
#define _OldMaterialLoad_H__

//获得加载控制器的接口
extern "C" __declspec(dllexport) int GetLoadController(void** pController);

#endif