#ifndef UI_DRIVER_K_KEYBOARD_H
#define UI_DRIVER_K_KEYBOARD_H

#include "./ikeyboard.h"

namespace UI
{
	class KKeyboard : public IKeyboard
	{
	public:
		virtual ~KKeyboard();

		virtual int HandleKeyInput(unsigned int uKey, int nModifier);
	};

}

#endif // UI_DRIVER_K_KEYBOARD_H

