#ifndef UI_DRIVER_K_COLOR_H
#define UI_DRIVER_K_COLOR_H

namespace UI
{
	DWORD StringToColor(LPCSTR szString);	//把字符串表示的颜色信息转为数值表示
	LPCSTR ColorToString(DWORD crColor);	//把数值表示的颜色信息转为字符串表示
    DWORD AddAlpha(DWORD dwColor, int nAlpha);

}

#endif // UI_DRIVER_K_COLOR_H

