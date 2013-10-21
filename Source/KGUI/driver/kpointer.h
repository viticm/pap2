#ifndef UI_DRIVER_K_POINTER_H
#define UI_DRIVER_K_POINTER_H

#include "KG3DEngine/KG3DInterface.h"

namespace UI
{

#define HAND_OBJECT_CURSOR_WIDTH	32
#define HAND_OBJECT_CURSOR_HEIGHT	32

	enum CURSOR_STATE
	{
		CURSOR_BEGIN,

		CURSOR_NORMAL				= CURSOR_BEGIN,		//默认
		CURSOR_CAST,				//施放技能
		CURSOR_UNABLECAST,			//不能施放技能
		CURSOR_TRAVEL,				//驿站
		CURSOR_UNABLETRAVEL,		//不能驿站
		CURSOR_SELL,				//出售
		CURSOR_UNABLESELL,			//不能出售
		CURSOR_BUYBACK,				//回购
		CURSOR_UNABLEBUYBACK,		//不能回购
		CURSOR_REPAIRE,				//修理
		CURSOR_UNABLEREPAIRE,		//不能修理
		CURSOR_ATTACK,				//攻击
		CURSOR_UNABLEATTACK,		//不能攻击
		CURSOR_SPEAK,				//对话
		CURSOR_UNABLESPEAK,			//不能对话
		CURSOR_LOOT,				//拾取
		CURSOR_UNABLELOOT,			//不能拾取
		CURSOR_LOCK,				//锁
		CURSOR_UNABLELOCK,			//不能锁
		CURSOR_INSPECT,				//察看
		CURSOR_UNABLEINSPECT,		//不能察看
		CURSOR_SPLIT,				//拆分
		CURSOR_UNABLESPLIT,			//不能拆分
		CURSOR_FLOWER,				//采花
		CURSOR_UNABLEFLOWER,		//不能采花
		CURSOR_MINE,				//采矿
		CURSOR_UNABLEMINE,			//不能采矿
		CURSOR_SEARCH,				//搜索
		CURSOR_UNABLESEARCH,		//不能搜索
		CURSOR_QUEST,				//任务道具
		CURSOR_UNABLEQUEST,			//不能捡任务道具
		CURSOR_READ,				//阅读
		CURSOR_UNABLEREAD,			//不能阅读

		CURSOR_MARKPRICE,			//标价

		CURSOR_TOP_BOTTOM,			//左右改变大小
		CURSOR_LEFT_RIGHT,			//上下改变大小
		CURSOR_LEFTTOP_RIGHTBOTTOM,	//左上右下改变大小
		CURSOR_RIGHTTOP_LEFTBOTTOM, //右上左下改变大小
		CURSOR_MOVE,				//移动

        // 18个npc类型相关光标
        CURSOR_NPC_BAG,             
        CURSOR_NPC_BANK,
        CURSOR_NPC_BOOK,
        CURSOR_NPC_CRAFT,
        CURSOR_NPC_DRESS,
        CURSOR_NPC_FOOD,
        CURSOR_NPC_GROCERIES,
        CURSOR_NPC_GUARDER,
        CURSOR_NPC_HORSE,
        CURSOR_NPC_MAIL,
        CURSOR_NPC_MATERIAL,
        CURSOR_NPC_MEDICINE,
        CURSOR_NPC_REPUTATION,
        CURSOR_NPC_RING,
        CURSOR_NPC_SKILL,
        CURSOR_NPC_SPECIAL,
        CURSOR_NPC_STAGE,
        CURSOR_NPC_WEAPON,

		CURSOR_HAND_OBJECT,			//把手中的图标转换为的光标

		CURSOR_END,
		CURSOR_WINDOWS_DEAULT = CURSOR_END, //系统
	};

	class KPointer
	{
#pragma pack(push, 2)
		struct KUSERSINGLEFRAMECURFILEHEADER
		{
			WORD cdreserved;
			WORD cdtype;
			WORD cdcount;

			byte bwidth;
			byte bheight;
			byte bcolorcount;
			byte breserved;

			WORD wxhotspot;
			WORD wyhotspot;

			DWORD lbytesinres;
			DWORD dwimageoffset;

			DWORD      biSize;
			LONG       biWidth;
			LONG       biHeight;
			WORD       biPlanes;
			WORD       biBitCount;
			DWORD      biCompression;
			DWORD      biSizeImage;
			LONG       biXPelsPerMeter;
			LONG       biYPelsPerMeter;
			DWORD      biClrUsed;
			DWORD      biClrImportant;

			KUSERSINGLEFRAMECURFILEHEADER()
			{
				cdreserved = 0;
				cdtype = 2;
				cdcount = 1;

				bwidth = HAND_OBJECT_CURSOR_WIDTH;
				bheight = HAND_OBJECT_CURSOR_HEIGHT;
				bcolorcount = 0;
				breserved = 0;

				wxhotspot = 0;
				wyhotspot = 0;

				lbytesinres = HAND_OBJECT_CURSOR_WIDTH * HAND_OBJECT_CURSOR_HEIGHT * sizeof(DWORD) + 
					(HAND_OBJECT_CURSOR_WIDTH * HAND_OBJECT_CURSOR_HEIGHT) / 8;
				dwimageoffset = 22;

				biSize = sizeof(BITMAPINFOHEADER);
				biWidth = HAND_OBJECT_CURSOR_WIDTH;
				biHeight = HAND_OBJECT_CURSOR_HEIGHT * 2;
				biPlanes = 1;
				biBitCount = 32;
				biCompression = 0;
				biSizeImage = HAND_OBJECT_CURSOR_WIDTH * HAND_OBJECT_CURSOR_HEIGHT * sizeof(DWORD);
				biXPelsPerMeter = 0;
				biYPelsPerMeter = 0;
				biClrUsed = 0;
				biClrImportant = 0;
			}
		};
#pragma pack(pop)
	public:
		KPointer();
		~KPointer();

        bool Init();
		void Exit();

		bool ShowCursor(bool bShow);						//设置鼠标指针的显示状态
		bool IsCursorVisible() const;

		bool SetCursorPos(int nX, int nY);					//设置鼠标指针位置
		bool GetCursorPos(int& nX, int& nY) const;			//获取鼠标指针位置

		int	SwitchCursor(int nIndex, int nIconID = -1);		//切换当前鼠标指针图形
		int	GetCurrentCursorIndex() const;					//得到当前鼠标图形的索引
		void RestoreCursor();	        					//重新激活鼠标
		void RestoreToWindowDefaultCursor();

		bool IsExistCursor(int nIndex) const;
		bool LoadCursor(int nIndex, LPCSTR szCursorFile);
		bool UnloadCursor(int nIndex); 
		int LoadAllCursor();
		bool UnloadAllCursor();

		bool LoadCursor(int nIndex, int nIconID);	        //由IconID转换的鼠标指针

	private:
		int KUiLoadCursor(const char* pszImgFile, HCURSOR &rhRetCursor);
		int KUiLoadCursor(BYTE *pbyData, size_t uDataLen, BYTE *pbyXOR, size_t uXORLen, HCURSOR &rhRetCursor);

	private:
		HCURSOR m_aCursor[CURSOR_END];
		int	m_nCurrentCursor;
		
		KG3DUIImage2 m_ImageArrow;
		char m_szArrowPath[MAX_PATH];

		KUSERSINGLEFRAMECURFILEHEADER m_CurFileHeader;
		bool m_bVisible;
	};
}

#endif // UI_DRIVER_K_POINTER_H

