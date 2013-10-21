#ifndef UI_DRIVER_K_MOUSE_H
#define UI_DRIVER_K_MOUSE_H

#include "./imouse.h"

namespace UI
{
	class KMouse : public IMouse
	{
	public:
		virtual ~KMouse();

		virtual int SetCurrentCursorIndex(int nIndex);
		virtual int GetCurrentCursorByStatus() const;

		virtual int HandleMouseInput(unsigned int uKey, int nModifier, int x, int y);
	};
}

#endif // UI_DRIVER_K_MOUSE_H

