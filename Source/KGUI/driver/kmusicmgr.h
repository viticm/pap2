#ifndef  UI_DRIVER_K_MUSIC_H
#define UI_DRIVER_K_MUSIC_H


struct IKG3DSoundShell;

namespace UI
{
    class KMusicMgr
    {
    public:
        KMusicMgr();
        ~KMusicMgr();

		//int Init();
		void SetSoundShell(IKG3DSoundShell *pSoundShell);
		void Exit();
		bool ReInit();

		void Activate();

		int PlayMusic(LPCTSTR pcszFileName);
		int StopMusic(BOOL bImmediately);

		int PlaySound(int nType, LPCSTR pcszFileName);

		IKG3DSoundShell *Get3DSoundShell() { return m_pSound; };

		static void BgMusicFinishCallback();

    private:

		int m_nRecover;

		IKG3DSoundShell *m_pSound;
    };

}


#endif