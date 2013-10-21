#ifndef UI_DRIVER_K_ANIMATE_IMAGE_MGR_H
#define UI_DRIVER_K_ANIMATE_IMAGE_MGR_H

#include "./kanimateimage.h"
#include "../common/tfixedindexrefcountarray.h"

namespace UI
{
	
	class KAnimateImageMgr
	{
	private:
		typedef TFixedIndexRefCountArray<KAnimateImage>	KAnimateImageArray;

	public:
		KAnimateImageMgr();
		~KAnimateImageMgr();

		bool Init();
		void Exit();
		bool Activate();

		bool Draw(int nIndex, float fX, float fY, float fWidth, float fHeight, DWORD dwAlpha) const;

		int LoadFile(LPCSTR szFilePath);
		void UnloadFile(int nIndex);
		int	GetCount() const;

		void SetGroup(int nIndex, int nGroup);
		void SetFrame(int nIndex, int nFrame);
		void SetTotalFrame(int nIndex, int nTotalFrame);
		void SetInterval(int nIndex, int nInterval);
		void SetLoopCount(int nIndex, int nLoopCount);

		int GetImageInfoIndex(int nIndex) const;
        int GetFrameIndex(int nIndex) const;

		bool IsFinished(int nIndex) const;

	private:
		KAnimateImageArray	m_aAnimateImage;
	};

}

#endif // UI_DRIVER_K_ANIMATE_IMAGE_MGR_H

