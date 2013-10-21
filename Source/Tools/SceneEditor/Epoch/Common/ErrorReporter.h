// File: ErrorReporter.h
// Desc:

#ifndef ERRORREPORTER_H
#define ERRORREPORTER_H

#include"DLLAPI.h"
#include"Singleton.h"
#include"BitMask.h"
#include"String.h"

namespace Common
{

class DLLAPI ErrorReporter: public Singleton<ErrorReporter>
{
public:
	enum ErrorCode  // Code whose most right bit is 0 means FAIL for some reason
	{               // Code whose most right bit is 1 means OK anyway
		EC_FAIL = MakeBitMask(0, 0),
		EC_OK = MakeBitMask(0, 1),
		// More error code...
	};

private:
	ErrorCode m_ErrorCode;

private:
	friend class Singleton<ErrorReporter>;
	ErrorReporter() {}
	ErrorReporter(const ErrorReporter& er) {}
	ErrorReporter& operator = (const ErrorReporter& er) { return *this; }

public:
	void SetErrorCode(ErrorCode ec) { m_ErrorCode = ec; }
	ErrorCode GetErrorCode() const { return m_ErrorCode; }

	static bool IsOK(ErrorCode ec) { return TestBit(ec, 0, 1); }
	static bool IsFail(ErrorCode ec) { return TestBit(ec, 0, 0); }
	//static String TranslateErrorCode(ErrorCode ec);
};

}  // namespace

#endif