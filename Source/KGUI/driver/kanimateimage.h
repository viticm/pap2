#ifndef UI_DRIVER_K_ANIMATE_IMAGE_H
#define UI_DRIVER_K_ANIMATE_IMAGE_H

namespace UI
{

	class KAnimateImage
	{
		friend class KAnimateImageMgr;

	public:
		KAnimateImage();

		bool operator==(KAnimateImage const& rhs) const;
		
		void SetImageInfoIndex(int nValue);
		void SetGroup(int nValue);
		void SetFrame(int nValue);
		void SetTotalFrame(int nValue);
		void SetInterval(int nValue);
		void SetLoopCount(int nValue);

		int GetImageInfoIndex() const;
		int GetGroup() const;
		int GetFrame() const;

		int IsFinished() const;

	private:
		bool Init();
		bool Activate(DWORD dwElapsed);

	private:
		// 静态数据
		int					m_nImageInfoIndex;	// KImageInfoMgr的索引
		int					m_nGroup;			// 动画组号
		int					m_nTotalFrame;		// 动画组内总帧数
		int					m_nLoopCount;		// 动画循环播放次数，设为-1表示一直播放
		DWORD				m_dwInterval;		// 动画间隔时间，单位ms，

		// 动态数据
		DWORD				m_dwFlipTime;		// 当前帧切换出来的时间
		int					m_nFrame;			// 动画组内帧，设为-1表示动画
        int                 m_nCurrentCount;    // 当前播放了多少次动画
		int					m_nFinished;		// 是否结束
	};


	//--------------------------------------------------------------------------------------------------------------------
	inline void KAnimateImage::SetImageInfoIndex(int nValue)
	{
		m_nImageInfoIndex = nValue;
	}


	inline void KAnimateImage::SetGroup(int nValue)
	{
		m_nGroup = nValue;
        m_nCurrentCount = 0;
		m_nFinished = false;
	}


	inline void KAnimateImage::SetFrame(int nValue)
	{
		m_nFrame = nValue;
	}


	inline void KAnimateImage::SetTotalFrame(int nValue)
	{
		m_nTotalFrame = nValue;
	}


	inline void KAnimateImage::SetInterval(int nValue)
	{
		if (nValue < 0)
			m_dwInterval = UINT_MAX;
		else
			m_dwInterval = static_cast<DWORD>(nValue);
	}


	inline void KAnimateImage::SetLoopCount(int nValue)
	{
		m_nLoopCount = nValue;
	}


	inline int KAnimateImage::GetImageInfoIndex() const
	{
		return m_nImageInfoIndex;
	}


	inline int KAnimateImage::GetGroup() const
	{
		return m_nGroup;
	}


	inline int KAnimateImage::GetFrame() const
	{
		return m_nFrame;
	}


	inline int KAnimateImage::IsFinished() const
	{
		return m_nFinished;
	}

}

#endif // UI_DRIVER_K_ANIMATE_IMAGE_H

