// File: INIFile.h
// Desc:  VC提供了读写.ini文件的API。这里是转换接口

#ifndef INIFILE_H
#define INIFILE_H

#include"RWFile.h"

namespace Common
{

class DLLAPI INIFile: public RWFile
{
public:
	bool GetINI(const String& sField, const String& sKey, String& sValue);
	void SetINI(const String& sField, const String& sKey, const String& sValue);
};

}  // namespace

#endif