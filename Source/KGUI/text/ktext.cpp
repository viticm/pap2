#include "stdafx.h"
#include "./ktext.h"


namespace UI
{

	bool IsCharacterSeparator(WCHAR wcChar)
	{
		if (wcChar >= 0xA3A1 && wcChar < 0xA3FE)
            return true;

        if (wcChar >= 0xA1A2 && wcChar < 0xA1C0)
            return true;

		if (wcChar >= 0x20 && wcChar < 0x30)
			return true;

        if (wcChar >= 0x3A && wcChar < 0x41)
            return true;

        if (wcChar >= 0x5B && wcChar < 0x61)
            return true;

        if (wcChar >= 0x7B  && wcChar < 0x80)
            return true;

        if (wcChar == L'\n')
            return true;

		return false;
	}

}

