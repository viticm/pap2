#pragma once

class KG3DMesh;
class KG3DBip;
class KG3DAni;
class KG3DClip;


class KG3DClipTools
{
public:
	
	KG3DClipTools(void);
	~KG3DClipTools(void);
	HRESULT CheckClipByBip(KG3DClip* pClip, KG3DBip* pBip);
	HRESULT CheckClipByMesh(KG3DClip* pClip, KG3DMesh* pMesh);
	
	//多线程加载动画时候的初始检查， 看动画骨骼数目是否和目标模型一致， 详细的检查（骨骼名字）的检查需要加载完成之后调用不带Preload后缀的函数
	/*HRESULT CheckClipByBipPreload(KG3DClip *pClip,
		const KG3DBip *pBip);
	HRESULT CheckClipByMeshPreload(KG3DClip* pClip, 
		KG3DMesh* pMesh);*/
private:
	HRESULT Helper_CheckClipByMeshBone(KG3DClip* pClip, KG3DMesh* pMesh);
};

extern KG3DClipTools& GetClipTool();
