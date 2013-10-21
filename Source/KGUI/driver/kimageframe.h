#ifndef UI_DRIVER_GRAPHIC_K_IMAGE_FRAME_H
#define UI_DRIVER_GRAPHIC_K_IMAGE_FRAME_H


namespace UI
{

	// 在整张图片里面的一个小图
	class KImageFrame
	{
	public:
		KImageFrame();

		bool Init(POINT const& ptLeftTop, SIZE const& sizeFrame, WORD wFrame);

		POINT	GetFrameLeftTop() const;
		WORD	GetImageFrame() const;
		SIZE	GetFrameSize() const;

	private:
		POINT	m_ptLeftTop;
		SIZE	m_sizeFrame;
		WORD	m_wFrame;				// stex里面的第几帧
	};


	//--------------------------------------------------------------------------------------------------------------------
	inline POINT KImageFrame::GetFrameLeftTop() const
	{
		return m_ptLeftTop;
	}


	inline WORD KImageFrame::GetImageFrame() const
	{
		return m_wFrame;
	}


	inline SIZE KImageFrame::GetFrameSize() const
	{
		return m_sizeFrame;
	}

}

#endif // UI_DRIVER_GRAPHIC_K_IMAGE_FRAME_H

