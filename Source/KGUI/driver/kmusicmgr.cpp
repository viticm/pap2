#include "stdafx.h"
#include "./kmusicmgr.h"
#include "../script/kscriptmgr.h"
#include "../KGUI.h"
#include "IKG3DSoundShell.h"


namespace
{
	int const FADE_FRAME = 3 * 16;
	int const RECOER_TIME = 16;
	int const MUSIC_REPLAY_TIME = 60 * 16;
}

namespace UI
{

    KMusicMgr::KMusicMgr()
    {
		m_nRecover = 0;
		m_pSound = NULL;
    }

    KMusicMgr::~KMusicMgr()
    {
    }

//    int KMusicMgr::Init()
//    {
//        int nResult = false;
//		HRESULT hr = E_FAIL;
//
//		/*m_pSound = Create3DSoundShell();
//		KGLOG_PROCESS_SUCCESS(m_pSound == NULL);
//
//		hr = m_pSound->Init(g_pUI->m_hWnd);
//		KGLOG_COM_CHECK_ERROR(hr);*/
//
////Exit1:
//		nResult = true;
//        if (!nResult)
//        {
//            Exit();
//        }
//		return nResult;
//    }

	void KMusicMgr::SetSoundShell(IKG3DSoundShell *pSoundShell)
	{
		m_pSound = pSoundShell;
	}

    void KMusicMgr::Exit()
    {
        /*if (m_pSound)
        {
            m_pSound->StopBgMusic(TRUE);
            SAFE_RELEASE(m_pSound);
        }*/
    }

	bool KMusicMgr::ReInit()
	{
        if (m_pSound)
        {
            m_pSound->StopBgMusic(FALSE);
            m_pSound->StopAllMusic();
            m_pSound->StopAllSound();
            m_pSound->StopAll3DMusic();
            m_pSound->StopAll3DSound();
        }
		return true;
	}

	void KMusicMgr::BgMusicFinishCallback()
	{
        if (g_pUI->m_MusicMgr.m_pSound == NULL)
            return;

        if (g_pUI->m_MusicMgr.m_pSound->IsBgMusicLoop())
        {
			g_pUI->m_Script.FireEvent("BG_MUSIC_FINISHED", 0);
        }
	}

	void KMusicMgr::Activate()
	{
		if (m_pSound)
		{
			m_pSound->Active();
			if (::GetForegroundWindow() == g_pUI->m_hWnd)
			{
				if (!m_nRecover)
					m_nRecover = 1;

				++m_nRecover;
				if (m_nRecover > RECOER_TIME)
				{
					m_pSound->Unlock();
					m_nRecover = 0;
				}
			}
			else
			{
				m_pSound->Lock();
				m_nRecover = 0;				
			}
		}
	}

	int KMusicMgr::PlayMusic(LPCSTR pcszFileName)
	{
		KG_PROCESS_ERROR(pcszFileName);
		KG_PROCESS_ERROR(m_pSound);

		m_pSound->PlayBgMusic(BG_MUSIC, pcszFileName, 0, 1.0f);

		return true;
Exit0:
		return false;
	}

	int KMusicMgr::StopMusic(BOOL bImmediately)
	{
		if (m_pSound)
			m_pSound->StopBgMusic(bImmediately);
		return TRUE;
	}

	int KMusicMgr::PlaySound(int nType, LPCSTR pcszFileName)
	{
		KG_PROCESS_ERROR(m_pSound);
		KG_PROCESS_ERROR(pcszFileName);

		//if (!m_pSound->IsPlayingSound(szFileFullPath)) //不重复播放一个音效
		m_pSound->PlaySound(nType, pcszFileName, 0, 1.0f, false, NULL);

		return true;
Exit0:
		return false;

	}

}