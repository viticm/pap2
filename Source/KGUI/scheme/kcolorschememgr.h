#ifndef UI_SCHEME_K_COLOR_SCHEME_MGR_H
#define UI_SCHEME_K_COLOR_SCHEME_MGR_H

#include "../driver/kcolor.h"


namespace UI
{
	class KColorSchemeMgr
	{
	public:
		enum { COLOR_NAME_MAX_LEN = 32 };

	private:
		struct KColorScheme
		{
			WCHAR wszSchemeName[COLOR_NAME_MAX_LEN];
			DWORD dwColor;
		};

	public:
		KColorSchemeMgr();
		~KColorSchemeMgr();

		BOOL Init();
		void Exit();

		int FindScheme(LPCWSTR wszSchemeName) const;

		BOOL IsExistColor(int nIndex) const;

		DWORD GetColor(int nIndex) const;

	private:
		KColorScheme* m_pScheme;
		size_t m_nSchemeCount;
	};

}

#endif // UI_SCHEME_K_COLOR_SCHEME_MGR_H

