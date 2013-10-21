#ifndef UI_DRIVER_K_INPUT_H
#define UI_DRIVER_K_INPUT_H

namespace UI
{

	class KHotkey;

	class KInput
	{
	private:
		enum
		{
			KEY_BEGIN	= 0,
			KEY_END		= 256,
		};

	public:
		KInput();
		~KInput();

		bool Init();
		void Exit();
		LRESULT HandleInput(UINT uMsg, WPARAM wParam, LPARAM lParam);

        bool IsActivated() const;

	private:
		LRESULT OnSizing(UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT OnNCSizing(UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT OnNCActivate(UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT OnActivateApp(UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT OnWindowPosChanged(UINT uMsg, WPARAM wParam, LPARAM lParam);
        void    OnKG3DSoundMsg(WPARAM wParam, LPARAM lParam);

    private:
        bool m_bActivated;
	};

}

#endif // UI_DRIVER_K_INPUT_H

