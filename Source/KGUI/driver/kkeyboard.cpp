#include "stdafx.h"
#include "./kkeyboard.h"
//#include "../shortcutkey.h"

namespace UI
{
	KKeyboard::~KKeyboard()
	{
	}

	int KKeyboard::HandleKeyInput(unsigned int uKey, int nModifier)
	{
#if 0
		// TODO : ¸úKShortcutkeyCenterÁ¬½Ó
		KShortcutKeyCentre::HandleKeyInput(uKey, nModifier);
#endif // 0
		return 0;
	}

}

