#ifndef UI_DRIVER_K_CONFIG_H
#define UI_DRIVER_K_CONFIG_H

namespace UI
{
	class KConfig
	{
	public:
		KConfig();
		~KConfig();

        bool Init();
		void Exit();

        IIniFile* OpenConfigFileMaxMatching(LPCSTR szFileName);

	private:
		void Close();

	private:
		IIniFile* m_pIniFile;
		char m_szLastIniFileName[MAX_PATH];
	};

}

#define	F_UI_USER_DATA_FOLDER "userdata"

#endif // UI_DRIVER_K_CONFIG_H

