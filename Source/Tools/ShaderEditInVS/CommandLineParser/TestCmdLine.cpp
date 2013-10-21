// TestCmdLine.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "cmdlineparser.h"

int main(int argc, char* argv[])
{
	CCmdLineParser parser(_T("/Key1 /Key2: -Key3:Val3 -Key4:\"Val 4-with/spaces/and-delimeters\" /Key5:Val5"));

	ASSERT(parser.HasKey(_T("Key1")) == true);
	ASSERT(parser.HasKey(_T("Key10")) == false);
	ASSERT(parser.HasVal(_T("Key2")) == false);
	_tprintf(_T("==================== Test Parser ====================\n"));
	_tprintf(_T("Command line: [%s]\n"), parser.getCmdLine());
	_tprintf(_T("Key1 has value: [%s]\n"), parser.GetVal(_T("Key1")));// -> []; //(empty string)
	_tprintf(_T("Key2 has value: [%s]\n"), parser.GetVal(_T("Key2")));// -> []; 
	_tprintf(_T("Key3 has value: [%s]\n"), parser.GetVal(_T("Key3")));// -> [Val3];
	_tprintf(_T("Key4 has value: [%s]\n"), parser.GetVal(_T("Key4")));// -> [Val 4-with/spaces/and-delimeters];

	_tprintf(_T("\n================= Real Command Line =================\n"));
	CCmdLineParser realParser(::GetCommandLine());
	CCmdLineParser::POSITION pos = realParser.getFirst();	
	CString sKey, sVal;
	while(!realParser.isLast(pos)) {
		realParser.getNext(pos, sKey, sVal);
		_tprintf(_T("Key: [%s], Val: [%s]\n"), sKey, sVal);
	}
	return 0;
}

