// File: Exception.h
// Desc: C++ Exception may be cut down the efficiency

#ifndef EXCEPTION_H
#define EXCEPTION_H

#include"String.h"

namespace Common
{

enum ExceptionLevel
{
	EL_GENERAL,  // program can still run; well, C++ does NOT support this kind of exception. That is, the program must be terminated whenever an exception of whatever kind is caught.
	EL_BAD  // program must be terminated
};

class DLLAPI Exception
{
protected:
	ExceptionLevel m_el;
	String m_sTitle;
	String m_sMessage;

public:
	Exception(ExceptionLevel el, const String& sTitle, const String sMessage): m_el(el), m_sTitle(sTitle), m_sMessage(sMessage) {}
	virtual ~Exception() {}

	ExceptionLevel GetLevel() const { return m_el; }
	String GetTitle() const { return m_sTitle; }
	String GetErrorMessage() const { return m_sMessage; }
};

}  // namespace

#endif