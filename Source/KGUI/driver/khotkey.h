#ifndef UI_DRIVER_K_HOTKEY_H
#define UI_DRIVER_K_HOTKEY_H

namespace UI
{

	class KHotkey
	{
		friend bool operator<(KHotkey const& lhs, KHotkey const& rhs);
		friend bool operator==(KHotkey const& lhs, KHotkey const& rhs);

	public:
		enum { HOTKEY_VKEY_MAX_COUNT = 4 };

		enum
		{
			HOTKEY_MOUSE_WHEEL_UP	= -1,
			HOTKEY_MOUSE_WHEEL_NONE = 0,
			HOTKEY_MOUSE_WHEEL_DOWN = 1,
		};

	public:
		KHotkey();
		KHotkey(BYTE aVKey[HOTKEY_VKEY_MAX_COUNT], int nMouseWheelDelta);
		KHotkey(KHotkey const& rhs);
		~KHotkey();

		KHotkey const& operator=(KHotkey const& rhs);

		int		GetCount() const;
		BYTE	GetAt(int nIndex) const;

		void	Sort();
		void	Clear();

		bool	PushBack(BYTE byVKey);

		int		GetMouseWheel() const;
		void	SetMouseWheel(int nDelta);

		static int MouseWheelDeltaToMouseWheel(int nDelta);

	private:
		int		m_nMouseWheelDelta;
		BYTE	m_aVKey[HOTKEY_VKEY_MAX_COUNT];
	};

}

#endif // UI_DRIVER_K_HOTKEY_H

