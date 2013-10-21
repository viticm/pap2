#include "stdafx.h"
#include "./kcolor.h"

namespace UI
{
	DWORD StringToColor(LPCSTR szString)
	{
		if (szString == NULL)
			return 0;

		DWORD crColor = 0xFF000000;

		char szBuffer[16] = { 0 };

		int  i = 0;
		int  n = 0;
		while (szString[i] != ',')
		{
			if (szString[i] == 0 || n >= 15)
				return crColor;
			szBuffer[n++] = szString[i++];
		}

		szBuffer[n] = '\0';
		crColor += ((atoi(szBuffer) & 0xFF) << 16);

		n = 0;
		++i;
		while (szString[i] != ',')
		{
			if (szString[i] == 0 || n >= 15)
				return crColor;
			szBuffer[n++] = szString[i++];
		}
		szBuffer[n] = 0;
		crColor += ((_ttoi(szBuffer) & 0xFF) << 8);
		n = 0;
		++i;
		while (szString[i] != 0)
		{
			if (n >= 15)
				return crColor;
			szBuffer[n++] = szString[i++];
		}
		szBuffer[n] = 0;
		crColor += (_ttoi(szBuffer) & 0xFF);
		return crColor;
	}

	LPCSTR ColorToString(DWORD crColor)
	{
		static char szColor[12];

		union 
		{
			struct 
			{
				BYTE b;
				BYTE g;
				BYTE r;
				BYTE a; 
			} ub;

			DWORD udw;
		} uc;

		uc.udw = crColor;

		_snprintf(szColor, _countof(szColor), "%d,%d,%d", uc.ub.r, uc.ub.g, uc.ub.b);
		szColor[11] = '\0';

		return szColor;
	}

    DWORD AddAlpha(DWORD dwColor, int nAlpha)
    {
        return (dwColor & 0x00FFFFFF) | ((nAlpha & 0xFF) << 24);
    }

}

