// File: String.cpp
// Desc:

#include"String.h"
#include"Exception.h"
#include"StringVector.h"

#include<vector>
using std::vector;

#include<algorithm>

namespace Common
{

String& String::operator = (const String& rhs)
{
	if(&rhs != this)
	{
		BaseString* pbs = static_cast<BaseString*>(this);
		pbs->operator = (static_cast<const BaseString&>(rhs));
	}

	return *this;
}

unsigned int GetNumOfDigits(int iNum)
{
	bool bSigned = iNum < 0;
	int temp = (bSigned)? -iNum: iNum;

	int NumOfDigits = 0;
	while(temp > 0)
	{
		temp /= 10;
		++NumOfDigits;
	}
	if(bSigned)
	{
		++NumOfDigits;
	}

	return NumOfDigits;
}

char GetAnotherCase(char c)
{
	if('a' <= c  && c <= 'z')
	{
		return c + 'A' - 'a';
	}
	else if('A' <= c  && c <= 'Z')
	{
		return c - 'A' + 'a';
	}

	return c;
}

String::String(char cVal): BaseString()
{
	char sz[2];
	sz[0] = cVal;
	sz[1] = '\0';

	operator = (sz);
}

String::String(int iVal): BaseString()
{
	// 得到位数
	if(iVal == 0)
	{
		BaseString::operator += ('0');
		return;
	}

	unsigned int NumOfDigits = GetNumOfDigits(iVal);
	vector<char> v;

	unsigned int i = 0;
	for(; i < NumOfDigits; ++i)
	{
		v.push_back('-');
	}

	int temp = (iVal < 0)? -iVal: iVal;
	while(temp > 0)
	{
		int tmp1 = temp / 10;
		char res = (char)(temp - tmp1 * 10) + '0';
		v[--NumOfDigits] = res;

		temp = tmp1;
	}

	unsigned int vs = v.size();
	for(i = 0; i < vs; ++i)
	{
		char c = v[i];
		BaseString::operator += (c);
	}

	v.clear();
}

String::String(float fVal): BaseString()
{
	if(fVal == 0.f)
	{
		BaseString::operator += ('0');
		return;
	}

	int iPart = (int)fVal;
	String sIPart(iPart);

	float fPart = fVal - iPart;
	fPart = (fPart < 0)? -fPart: fPart;
	while((fPart - (int)fPart) > 0.f)
	{
		fPart *= 10;
	}
	int ifPart = (int)fPart;
	String sFPart(ifPart);

	BaseString::operator += (sIPart);
	if(!sFPart.empty())
	{
		BaseString::operator += ('.');
		BaseString::operator += (sFPart);
	}
}

String& String::operator = (char cVal)
{
	String temp(cVal);
	operator = (temp);
	return *this;
}

String& String::operator = (int iVal)
{
	String temp(iVal);
	operator = (temp);
	return *this;
}

String& String::operator = (float fVal)
{
	String temp(fVal);
	operator = (temp);
	return *this;
}

String& String::operator += (const String& s)
{
	BaseString::operator += (s);
	return *this;
}

char String::operator [] (unsigned int index) const
{
	if(index >= size())
	{
		throw Exception(EL_BAD, "String::operator []", "Index beyond the mark!");
	}

	return at(index);
}

unsigned int String::GetLength() const
{
	unsigned int iLen = 0;

	bool bEnPre = IsCnChar(at(0))? false: true;
	bool bHead = !bEnPre;

	unsigned int iSize = size();
	for(unsigned int i = 0; i < iSize; ++i)
	{
		if(IsCnChar(at(i)))  // 当前字节是汉字字节
		{
			if(bEnPre)  // 前一个是英文字节
			{
				bHead = true;  // 则当前字节为汉字字符首字节
			}

			if(bHead)  // 当前字节是汉字首字节
			{
				// Do something you like...
				++iLen;
			}

			// 更新(for next loop)
			bEnPre = false;
			bHead = !bHead;
		}
		else  // 当前字节是英文字节
		{
			if(bHead || bEnPre)
			{
				// Do something you like...
				++iLen;
			}

			bEnPre = true;
			bHead = false;
		}
	}
	
	return iLen;
}

unsigned int String::GetNumOfSubstr(char c) const
{
	if(!IfContain(c))
	{
		return 0;
	}

	unsigned int ret = 0;
	String sTemp = *this;
	while(sTemp.IfContain(c))
	{
		++ret;
		sTemp = sTemp.substr(sTemp.find_first_of(c) + sizeof(c));
	}

	return ret;
}

unsigned int String::GetNumOfSubstr(const String& s) const
{
	if(!IfContain(s))
	{
		return 0;
	}

	unsigned int ret = 0;
	String sTemp = *this;
	while(sTemp.IfContain(s))
	{
		++ret;
		sTemp = sTemp.substr(sTemp.find_first_of(s) + s.size());
	}

	return ret;
}

void String::Trim(bool bLeft, bool bRight)
{
	bool bAllBlank = true;
	for(unsigned int j = 0; j < size(); ++j)
	{
		if(at(j) != ' ' && at(j) != '\t' && at(j) != '\r' && at(j) != '\n')
		{
			bAllBlank = false;
		}
	}
	if(bAllBlank)
	{
		Clear();
		return;
	}

	size_t lspaces = 0, rspaces = 0, len = length(), i = 0;

	if(bLeft)
	{
		for(; i < len && (at(i) == ' ' || at(i) == '\t' || at(i) == '\r' || at(i) == '\n');)
		{
			++lspaces;
			if(i++ > i)
			{
				//break;
			}
		}
	}
	
	if(bRight && lspaces < len)
	{
		for( i = len - 1; i >= 0 && ( at(i) == ' ' || at(i) == '\t' || at(i) == '\r' || at(i) == '\n');)
		{
			++rspaces;
			if(i-- < i)
			{
				//break;
			}
		}
	}
	
	*this = substr(lspaces, len-lspaces-rspaces);
}

void String::UpperCase()
{
	std::transform(begin(), end(), begin(), static_cast<int(*)(int)>(::toupper));
}

void String::LowerCase()
{
	std::transform(begin(), end(), begin(), static_cast<int(*)(int)>(::tolower));
}

int String::IntValue() const
{
	return atoi(c_str());
}

float String::FloatValue() const
{
	return static_cast<float>(atof(c_str()));
}

bool String::Divide(StringVector& sv, bool bDQM)
{
	static char Blanks[NUM_BLANKS] = {' ', '\t', '\r', '\n', '\0'};

	sv.clear();

	bool bEnterDQM = false;  // double quotation marks
	Common::String sTemp;
	for(unsigned int i = 0; i < GetSize(); ++i)
	{
		char c = at(i);
		bool bPush = false;

		if(bEnterDQM && bDQM)  // 如果是在双引号中
		{
			if(c == '"')  // 终结双引号
			{
				bPush = true;
				bEnterDQM = false;
			}
			else
			{
				sTemp += c;  // 直接追加字符串
			}
		}
		else  // 不在双引号中
		{
			if(c == '"' && bDQM)  // 要进入双引号中
			{
				bPush = true;
				bEnterDQM = true;
			}
			else  // 不是双引号，只是普通字符
			{
				bool bBlank = false;
				for(unsigned int j = 0; j < NUM_BLANKS; ++j)
				{
					if(c == Blanks[j])
					{
						bPush = true;
						bBlank = true;  // 是空白字符
						break;
					}
				}

				if(!bBlank)  // 不是空白字符
				{
					sTemp += c;
				}
			}
		}

		if(bPush)
		{
			if(!sTemp.empty())
			{
				sv.push_back(sTemp);
				sTemp.Clear();
			}
		}
	}

	if(bEnterDQM && bDQM)  // 还在双引号中，一定是出现了语法错误(漏了结束双引号)
	{
		//throw Common::Exception(Common::EL_GENERAL, "Extractor::DivideString", "Grammar error: a terminator of DQM expected!");
	}
	
	if(!sTemp.empty())
	{
		sv.push_back(sTemp);
		sTemp.Clear();
	}

	return true;
}

DLLAPI const String operator + (const String& a, const String& b)
{
	String ret = a;
	ret += b;
	return ret;
}

void DecToHex(unsigned int DecNum, String& sHexNum)
{
	vector<char> vChars;

	if(DecNum == 0)
	{
		sHexNum = "00";
	}
	else
	{
		unsigned int PosNum = 0;
		while(DecNum > 0)
		{
			++PosNum;
			switch(DecNum & 0x0000000F)
			{
			case 0:
				vChars.push_back('0');
				break;
			case 1:
				vChars.push_back('1');
				break;
			case 2:
				vChars.push_back('2');
				break;
			case 3:
				vChars.push_back('3');
				break;
			case 4:
				vChars.push_back('4');
				break;
			case 5:
				vChars.push_back('5');
				break;
			case 6:
				vChars.push_back('6');
				break;
			case 7:
				vChars.push_back('7');
				break;
			case 8:
				vChars.push_back('8');
				break;
			case 9:
				vChars.push_back('9');
				break;
			case 10:
				vChars.push_back('A');
				break;
			case 11:
				vChars.push_back('B');
				break;
			case 12:
				vChars.push_back('C');
				break;
			case 13:
				vChars.push_back('D');
				break;
			case 14:
				vChars.push_back('E');
				break;
			case 15:
				vChars.push_back('F');
				break;
			}
			DecNum >>= 4;
		}
		if(PosNum % 2 != 0)
		{
			vChars.push_back('0');
		}

		unsigned int NumOfHexPos = vChars.size();

		sHexNum.Clear();
		for(unsigned int i = 0; i < NumOfHexPos; ++i)
		{
			sHexNum += String(vChars[NumOfHexPos - i - 1]);
		}
	}
}

unsigned int HexToDec(const String& HexNum)
{
	if(HexNum.empty())
	{
		return 0;
	}

	unsigned int NumOfHexPos = HexNum.size();
	unsigned int i = 0;
	bool bZero = true;
	for(i = 0; i < NumOfHexPos; ++i)
	{
		if(HexNum.at(i) != '0')
		{
			bZero = false;
			break;
		}
	}
	if(bZero)
	{
		return 0;
	}

	unsigned int Ret = 0;
	
	for(i = 0; i < NumOfHexPos && i < 8; ++i)
	{
		char c = HexNum.at(i);
		switch(c)
		{
		case '0':
			Ret += 0;
			break;
		case '1':
			Ret += 1;
			break;
		case '2':
			Ret += 2;
			break;
		case '3':
			Ret += 3;
			break;
		case '4':
			Ret += 4;
			break;
		case '5':
			Ret += 5;
			break;
		case '6':
			Ret += 6;
			break;
		case '7':
			Ret += 7;
			break;
		case '8':
			Ret += 8;
			break;
		case '9':
			Ret += 9;
			break;
		case 'A':
		case 'a':
			Ret += 10;
			break;
		case 'B':
		case 'b':
			Ret += 11;
			break;
		case 'C':
		case 'c':
			Ret += 12;
			break;
		case 'D':
		case 'd':
			Ret += 13;
			break;
		case 'E':
		case 'e':
			Ret += 14;
			break;
		case 'F':
		case 'f':
			Ret += 15;
			break;
		}
		Ret <<= 4;
	}

	return Ret;
}

}  // namespace