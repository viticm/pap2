#include "stdafx.h"
#include "./khotkey.h"

namespace UI
{

	KHotkey::KHotkey()
	{
		Clear();
	}


	KHotkey::KHotkey(BYTE aVKey[HOTKEY_VKEY_MAX_COUNT], int nMouseWheelDelta)
	{
		m_nMouseWheelDelta = nMouseWheelDelta;

		memcpy(m_aVKey, aVKey, sizeof(m_aVKey));
		Sort();
	}


	KHotkey::KHotkey(KHotkey const& rhs)
	{
		m_nMouseWheelDelta = rhs.m_nMouseWheelDelta;

		memcpy(m_aVKey, rhs.m_aVKey, sizeof(m_aVKey));
	}


	KHotkey::~KHotkey()
	{
	}


	KHotkey const& KHotkey::operator=(KHotkey const& rhs)
	{
		if (this != &rhs)
		{	
			m_nMouseWheelDelta = rhs.m_nMouseWheelDelta;

			memcpy(m_aVKey, rhs.m_aVKey, sizeof(m_aVKey));
		}
		return *this;
	}


	int	KHotkey::GetCount() const
	{
		int nIndex = 0;
		for (; nIndex < HOTKEY_VKEY_MAX_COUNT; ++nIndex)
		{
			if (m_aVKey[nIndex] == 0)
				break;
		}
		return nIndex;
	}


	BYTE KHotkey::GetAt(int nIndex) const
	{
		_ASSERT(nIndex >= 0 && nIndex < HOTKEY_VKEY_MAX_COUNT);

		return m_aVKey[nIndex];
	}


	void KHotkey::Sort()
	{
		std::sort(&m_aVKey[0], &m_aVKey[0] + HOTKEY_VKEY_MAX_COUNT, greater<BYTE>());
		std::sort(&m_aVKey[0], &m_aVKey[0] + GetCount());
	}


	void KHotkey::Clear()
	{
		m_nMouseWheelDelta = HOTKEY_MOUSE_WHEEL_NONE;

		memset(m_aVKey, 0, sizeof(m_aVKey));
	}


	bool KHotkey::PushBack(BYTE byVKey)
	{
		for (int nIndex = 0; nIndex < HOTKEY_VKEY_MAX_COUNT; ++nIndex)
		{
			if (m_aVKey[nIndex] == byVKey)
				return true;

			if (m_aVKey[nIndex] == 0)
			{
				m_aVKey[nIndex] = byVKey;
				return true;
			}
		}
		return false;
	}


	int	KHotkey::GetMouseWheel() const
	{
		return m_nMouseWheelDelta;
	}


	void KHotkey::SetMouseWheel(int nDelta)
	{
		m_nMouseWheelDelta = MouseWheelDeltaToMouseWheel(nDelta);
	}


	int KHotkey::MouseWheelDeltaToMouseWheel(int nDelta)
	{
		if (nDelta < 0)
			return HOTKEY_MOUSE_WHEEL_UP;
		if (nDelta > 0)
			return HOTKEY_MOUSE_WHEEL_DOWN;
		return HOTKEY_MOUSE_WHEEL_NONE;
	}


	bool operator<(KHotkey const& lhs, KHotkey const& rhs)
	{
		if (lhs.m_nMouseWheelDelta < rhs.m_nMouseWheelDelta)
			return true;
		return (lhs.m_nMouseWheelDelta == rhs.m_nMouseWheelDelta) ? memcmp(lhs.m_aVKey, rhs.m_aVKey, sizeof(lhs.m_aVKey)) < 0 : false;
	}


	bool operator==(KHotkey const& lhs, KHotkey const& rhs)
	{
		return lhs.m_nMouseWheelDelta == rhs.m_nMouseWheelDelta && memcmp(lhs.m_aVKey, rhs.m_aVKey, sizeof(lhs.m_aVKey)) == 0;
	}

}


