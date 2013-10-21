////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : CmdLineFileEnumer.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2009-5-7 15:15:56
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_CMDLINEFILEENUMER_H_
#define _INCLUDE_CMDLINEFILEENUMER_H_

////////////////////////////////////////////////////////////////////////////////
struct ICFileEnumer;

ICFileEnumer* CreateCmdLineFileEnumer(LPCTSTR strCmdLine, LPCTSTR strDirectory);//根据/F:"xx" "xx"，分析出所有的文件，文件名必须是命令行最后的。文件名如果没有路径就认为是当前路径


#endif //_INCLUDE_CMDLINEFILEENUMER_H_
