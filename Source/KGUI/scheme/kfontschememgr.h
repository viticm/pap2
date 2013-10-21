#ifndef UI_SCHEME_K_FONT_SCHEME_MGR_H
#define UI_SCHEME_K_FONT_SCHEME_MGR_H

#include "KG3DEngine/KG3DInterface.h"
#include "../driver/kcolor.h"

namespace UI
{
	struct KFontScheme
	{
		int	nFontID;
		int	nSize;
		int	nBorderSize;
		int	nShadowOffset;
		int	nFontColorSchemeIndex;
		int	nBorderColorSchemeIndex;
		int	nShadowColorSchemeIndex;
		char szName[32];
	};

	class KFontSchemeMgr
	{
	public:
		struct KFontInfo
		{
			char	szName[32];
			char	szFile[MAX_PATH];
			int		nSize;
			DWORD	dwStyle;
			DWORD	dwID;
			bool	bLoaded;
			bool	bChat;

			KFontInfo()
			{
				szName[0] = '\0';
				szFile[0] = '\0';
				nSize = 16;
				dwStyle = KG3DUI_TEXT_STYLE_BORDER | KG3DUI_TEXT_STYLE_SHADOW;
				dwID = 0;
				bLoaded = false;
				bChat = false;
			}
		};

		struct KChatFontInfo
		{
			char	szName[64];
			char	szFile[MAX_PATH];
			int		nSize;
			DWORD	dwStyle;
			DWORD	dwID;
			bool	bLoaded;
			bool	bSeted;

			KChatFontInfo()
			{
				szName[0]	= '\0';
				szFile[0]	= '\0';
				nSize		= 16;
				dwStyle		= KG3DUI_TEXT_STYLE_BORDER | KG3DUI_TEXT_STYLE_SHADOW;
				dwID		= 0;
				bLoaded		= false;
				bSeted		= false;
			}
		};

		typedef std::vector<KFontScheme const>	KFontSchemeArray;

	public:
		KFontSchemeMgr();
		~KFontSchemeMgr();

		bool Init();
		void Exit();

		KFontScheme const* GetFontScheme(int nIndex) const;

		BOOL SetFontScale(float fScale);
		float GetFontScale() const;

		BOOL IsFontVertical(int nIndex) const;
		int IsFontVerticalByID(DWORD dwFontID) const;

		int GetCount() const;

		int LoadDefaultFontList();
		int LoadFontList(LPCSTR pcszFontList);

		int LoadScheme();

		int IsFontLoaded(DWORD dwFontID) const;
		
		int SetOffset(int nOffSet);
		int GetOffset();

		int SetFont(DWORD dwID, LPCSTR pcszName, LPCSTR pcszFile, int nSize, DWORD dwStyle);
		const KFontInfo * GetFont(DWORD dwID);

		DWORD GetChatFontID();

    private:
		void UnloadFont();
		void Reset();
	private:
		KFontSchemeArray		m_aScheme;
		KFontInfo				m_aFontList[RS2_MAX_FONT_ITEM_NUM];

		KChatFontInfo			m_ChatFont;

		float					m_fFontScale;

		int						m_nFontsizeOffset;
	};

}

#endif // UI_SCHEME_K_FONT_SCHEME_MGR_H

