#ifndef UI_ELEM_BUILD_AGENT_K_INI_WND_BUILDER_H
#define UI_ELEM_BUILD_AGENT_K_INI_WND_BUILDER_H

namespace UI
{
	class KWndFrame;

	KWndFrame* CreateWndFrame(IIniFile* pIni);
	KWndFrame* CreateWndFrame(LPCSTR szIniFile, LPCSTR szFrameName = "");

	BOOL BindWndScript(KWndFrame* pWnd);
}


#endif // UI_ELEM_BUILD_AGENT_K_INI_WND_BUILDER_H
