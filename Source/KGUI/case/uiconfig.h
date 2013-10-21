#ifndef UI_CASE_K_UI_CONFIG_H
#define UI_CASE_K_UI_CONFIG_H

#include "engine.h"

namespace UI
{

	class KUiConfig
	{
	public:
		enum { USER_ACCOUT_ID_MAX_LEN = 32 };

	public:
		KUiConfig();
		virtual ~KUiConfig();

		static int Init();
		static void Exit();

		static bool ReInit();

		static KUiConfig& GetSelf();

        int RemoveUserdata();

		int SetUserAccount(LPCSTR pcszAccountName);							    //帐户名
		int SetUserServer(LPCSTR pcszRegionName, LPCSTR pcszServerName);		//区、服务器的名字
		int SetUserRoleName(LPCSTR pcszRoleName);								//角色名字

		LPCSTR GetUserAccount() const;
		LPCSTR GetUserRegion() const;
		LPCSTR GetUserServer() const;
		LPCSTR GetUserRoleName() const;
		LPCSTR GetUserDataFloder() const;

        int IsUserdataExist(LPCSTR pcszAccountName) const;

		int GetUserDataPath(char szFilePath[MAX_PATH], LPCSTR pcszRoleName = NULL) const;

		int Load(LPCSTR szAccountName);
		int Save(LPCSTR szAccountName);

		static int IsFilePathExist(LPCSTR szFilePath);
        static int CreatePath(LPCSTR szFilePath);

	private:
		void ResetUserdata();

        void LoadVersion();

	private:
		char m_szUserAccount[64];
		char m_szUserRegion[64];
		char m_szUserServer[64];
		char m_szUserRoleName[64];

		static KUiConfig* ms_pSelf;
	};

}

#endif // UI_CASE_K_UI_CONFIG_H

