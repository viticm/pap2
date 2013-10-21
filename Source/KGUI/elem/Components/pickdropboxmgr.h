////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : pickdropboxmgr.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-2-23 17:13:50
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_PICKDROPBOXMGR_H_
#define _INCLUDE_PICKDROPBOXMGR_H_

#include "KG3DEngine/KG3DInterface.h"

namespace UI
{

enum KPICKDROPBOXIMAGES
{
	PICKDROPBOX_IMAGE_BEGIN = 0,
	DISABLE_IMAGE = PICKDROPBOX_IMAGE_BEGIN, //不可用
	UNEQUIPABLE_IMAGE, //不能装备
	COOLDOWN_IMAGE, //冷却
	SELECTED_IMAGE, //选中
	PRESSED_IMAGE, //按下
	NO_OBJECT_MOUSEOVER_IMAGE, //没有物品时鼠标在上面
	MOUSEOVER_IMAGE, //鼠标在上面
	PICKDROPBOX_IMAGE_END,
	PICKDROPBOX_IMAGE_COUNT = PICKDROPBOX_IMAGE_END,
};

class KPickDropBoxMgr
{
private:
	KPickDropBoxMgr();
	~KPickDropBoxMgr();
public:
	static bool Init();
	static void Exit();
	static KPickDropBoxMgr &GetSelf();

	void DrawBoxAddOnLayer(float fX, float fY, float fWidth, float fHeight, DWORD dwBoxStatus, float fCoolingPercentage);

	void DrawBoxMouseOverLayer(float fX, float fY, float fWidth, float fHeight);

	int AppendSparkingAnimate();
	int RemoveSparkingAnimate(int nAnimateIndex);

	int AppendInUseAnimate();
	int RemoveInUseAnimate(int nAnimateIndex);

	int AppendStaringAnimate();
	int RemoveStaringAnimate(int nAnimateIndex);

private:
	int LoadSource();
	int FillImageData(int nIndex, int nFrame);
	int UnloadSource();

	int FillCollingImageData();

protected:
private:
	int m_nImageIndex;
	KG3DUIImage2 m_aImage[PICKDROPBOX_IMAGE_COUNT];
	char m_szImageFileName[MAX_PATH];
	char m_szAnimateFileName[MAX_PATH];
	int m_nSparkingAnimateGroup;
	int m_nInUseAnimateGroup;
	int m_nStaringAnimateGroup;

	KG3DUIImagePart2 m_CoolingImage;

	static KPickDropBoxMgr *ms_pSelf;
};

} //namespace UI


#endif //_INCLUDE_PICKDROPBOXMGR_H_
