#ifndef UI_DRIVER_K_FILE_PATH_MGR_H
#define UI_DRIVER_K_FILE_PATH_MGR_H

#include "../common/tfixedindexrefcountarray.h"

namespace UI
{

	class KFilePathMgr
	{
	public:
        typedef std::map<std::string, std::string> KKeyToFilePathMap;
        typedef TFixedIndexRefCountArray<std::string> KFilePathArray;
		typedef std::map<std::string, int>	KCountedStringMap;

	public:
		KFilePathMgr();
		~KFilePathMgr();

		bool Init();
		void Exit();

		int AddFilePath(LPCSTR szFilePath);
		bool RemoveFilePath(int nIndex);
		LPCSTR GetFilePath(int nIndex) const;
    	LPCSTR GetFilePath(LPCSTR pszKey) const;

		BOOL TabError(LPCSTR szFmt, ...) const;

	private:
		KFilePathArray	m_aFilePath;

        KKeyToFilePathMap m_KeyToPath;

		mutable KCountedStringMap m_TabErrors;
	};


	void FormatFilePath(LPSTR pszPath);
	LPCSTR FormatFilePath(LPCSTR pcszPath);
}

#endif // UI_DRIVER_K_FILE_PATH_MGR_H

