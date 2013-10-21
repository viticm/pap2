// File: FileError.h
// Desc:

#ifndef FILEERROR_H
#define FILEERROR_H

#include"Exception.h"

namespace Common
{

#define FileError_Title "Error: General file error"
#define FileNotFound_Title "Error: File not found"
#define FileCannotCreate_Title "Error: File can not create"
#define FileEOF_Title "Error: File EOF"
#define FileReadError_Title "Error: File read error"
#define FileWriteError_Title "Error: File write error"

class DLLAPI FileError: public Exception
{
protected:
	String m_sFilename;

public:
	FileError(const String& sFilename, ExceptionLevel el = EL_BAD, const String& sTitle = FileError_Title, const String& sMessage = FileError_Title): Exception(el, sTitle, sMessage), m_sFilename(sFilename) {}

	String GetFilename() const { return m_sFilename; }
};

class DLLAPI FileNotFound: public FileError
{
public:
	FileNotFound(const String& sFilename, ExceptionLevel el = EL_BAD, const String& sTitle = FileNotFound_Title, const String& sMessage = FileNotFound_Title): FileError(sFilename, el, sTitle, sMessage) { m_sMessage += "\nFile: " + m_sFilename; }
};

class DLLAPI FileCannotCreate: public FileError
{
public:
	FileCannotCreate(const String& sFilename, ExceptionLevel el = EL_BAD, const String& sTitle = FileCannotCreate_Title, const String& sMessage = FileCannotCreate_Title): FileError(sFilename, el, sTitle, sMessage) { m_sMessage += "\nFile: " + m_sFilename;}
};

class DLLAPI FileEOF: public FileError
{
private:
	unsigned int m_uiRead;

public:
	FileEOF(const String& sFilename, unsigned int uiRead, ExceptionLevel el = EL_GENERAL, const String& sTitle = FileEOF_Title, const String& sMessage = FileEOF_Title): FileError(sFilename, el, sTitle, sMessage), m_uiRead(uiRead) { m_sMessage += "\nFile: " + m_sFilename;}

	unsigned int GetRead() const { return m_uiRead; }
};

class DLLAPI FileReadError: public FileError
{
public:
	FileReadError(const String& sFilename, ExceptionLevel el = EL_BAD, const String& sTitle = FileReadError_Title, const String& sMessage = FileReadError_Title): FileError(sFilename, el, sTitle, sMessage) {}
};

class DLLAPI FileWriteError: public FileError
{
public:
	FileWriteError(const char* szFilename, ExceptionLevel el = EL_BAD, const String& sTitle = FileWriteError_Title, const String& sMessage = FileWriteError_Title): FileError(szFilename, el, sTitle, sMessage) {}
};

}  // namespace

#endif
