////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : kaddonmgr.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-11-9 9:28:23
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KADDONMGR_H_
#define _INCLUDE_KADDONMGR_H_

////////////////////////////////////////////////////////////////////////////////

namespace UI
{

	class KAddOnMgr
	{
	public:
		struct KADDONINFO
		{
			std::string strID;				//文件夹名，作为插件的唯一区分
            std::string strName;				//名字
            std::string strDesc;				//描述
			std::string strDependence;			//依赖
			std::string strWeakDependence;		//弱依赖
			std::string	strVersion;				//版本号
			int nDefault;						//默认加载
			int nDisable;						//不加载
			int nLoad;							//是否加载
			int nLoaded;						//已经加载
            std::vector<std::string> aStrFile;
		};

		typedef std::vector<KADDONINFO> KADDONINFOARRAY;

	public:
		int Init();
		int GetCount();
		KADDONINFO * GetAddOnInfo(int nIndex);
		int Enable(int nIndex, int nEnable);
		int Disable(int nIndex, int nDisable);
		int LoadAddOn();

		int EnableOverdueAddOn(int nEnable);
		int IsEnableOverdueAddOn();

		int SetCurrentAddOnVersion(const char * pcszVersion);

		int GetIndexByID(const char * pcszID);

	private:
		int Append(LPCSTR pcszFile, LPCSTR pcszFolder);

		int LoadAddOn(const char * pcszID, BOOL bForce);
		int LoadAddOn(int nIndex, BOOL bForce);

		int CreateEnv();

		static int IndexOfEngine(lua_State* L);
		static int NewIndexOfEngine(lua_State* L);
		static int IndexOfAddOn(lua_State* L);
		static int NewIndexOfAddOn(lua_State* L);

	private:
		KADDONINFOARRAY m_aAddOn;

		int m_nEnableOverdueAddOn;
		std::string m_strCurrentAddOnVersion;
	};

	extern KAddOnMgr g_AddOnMgr;
}



#endif //_INCLUDE_KADDONMGR_H_
