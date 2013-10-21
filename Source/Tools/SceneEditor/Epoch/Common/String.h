// File: String.h
// Desc:

#ifndef STRING_H
#define STRING_H

#include<string>
#include"DLLAPI.h"

/*
#if USING_WCHAR
typedef std::wstring BaseString;
#else
*/
typedef std::string BaseString;
//#endif

namespace Common
{

class DLLAPI StringVector;

class DLLAPI String: public BaseString
{
public:
	enum
	{
		NUM_BLANKS = 5
	};

public:
	String(): BaseString() {}
	String(const String& rhs): BaseString(rhs) {}
	String& operator = (const String& rhs);
	String(const BaseString& rhs): BaseString(rhs) {}
	String(const char* sz): BaseString(sz) {}
	virtual ~String() {}

	String(char cVal);
	String(int iVal);
	String(float fVal);
	String& operator = (char cVal);
	String& operator = (int iVal);
	String& operator = (float fVal);
	String& operator += (const String& s);

	operator const char* () const { return c_str(); }
	char operator [] (unsigned int index) const;

	unsigned int GetSize() const { return size(); }
	unsigned int GetLength() const;

	bool IfContain(char c) const { return find(c) != npos; }
	bool IfContain(const String& s) const { return find(s) != npos; }
	unsigned int GetNumOfSubstr(char c) const;
	unsigned int GetNumOfSubstr(const String& s) const;

	void Trim(bool bLeft = true, bool bRight = true);  // 去掉左右空白
	void UpperCase();  // 全部转换为大写
	void LowerCase();  // 全部转换为小写

	int IntValue() const;  // 转换为整型
	float FloatValue() const;  // 转换为单精度浮点数

	void Clear() { erase(0, size()); }

	bool Divide(StringVector& sv, bool bDQM);

	unsigned int find_first_of(const Common::String& s) const { return find(s); }  // STL's string::find_first_of has a big problem when the parameter s is Chinese words. So, overload it
	unsigned int find_last_of(const Common::String& s) const { return BaseString::find_last_of(s); }
	const char* c_str() const { return BaseString::c_str(); }
};

DLLAPI unsigned int GetNumOfDigits(int iNum);

inline bool IsCnChar(char c)  // 是否汉字字节(包括韩日文字节)
{
	return static_cast<unsigned char>(c) > 128;
}

DLLAPI char GetAnotherCase(char c);

DLLAPI const String operator + (const String& a, const String& b);

inline const String operator + (const String& a, char b)
{
	return a + String(b);
}

inline const String operator + (char a, const String& b)
{
	return String(a) + b;
}

inline const String operator + (const String& a, int b)
{
	return a + String(b);
}

inline const String operator + (int a, const String& b)
{
	return String(a) + b;
}

inline const String operator + (const String& a, float b)
{
	return a + String(b);
}

inline const String operator + (float a, const String& b)
{
	return String(a) + b;
}

inline const String operator + (const String& a, const char* b)
{
	return a + String(b);
}

inline const String operator + (const char* a, const String& b)
{
	return String(a) + b;
}

DLLAPI void DecToHex(unsigned int DecNum, String& sHexNum);
DLLAPI unsigned int HexToDec(const String& HexNum);

}  // namespace

#endif