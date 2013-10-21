#ifndef UI_DRIVER_K_IMAGE_INFO_MGR_H
#define UI_DRIVER_K_IMAGE_INFO_MGR_H

#include "./kimageinfo.h"
#include "../common/tfixedindexrefcountarray.h"


namespace UI
{

	class KImageInfoMgr
	{
	private:
        typedef stdext::hash_map<std::string, int> KFilePathToImageInfoIndex;
		typedef TFixedIndexRefCountArray<KImageInfo> KImageInfoArray;

	public:
		KImageInfoMgr();
		~KImageInfoMgr();

		bool Init();
		void Exit();

		int LoadImageInfo(LPCSTR szFilePath);
		bool UnloadImageInfo(LPCSTR szFilePath);
		bool UnloadImageInfo(int nIndex);

		int GetImageInfoIndex(LPCSTR szFilePath) const;

		KImageInfo*	GetImageInfo(LPCSTR szFilePath);
		KImageInfo*	GetImageInfo(int nIndex);

		LPCSTR	GetImagePath(int nIndex) const;

	private:
		KFilePathToImageInfoIndex	m_aFilePathToImageInfoIndex;
		KImageInfoArray				m_aImageInfo;
	};

}

#endif // UI_DRIVER_K_IMAGE_INFO_MGR_H

