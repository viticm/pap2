#ifndef CASE_RENDER_K_MODEL_GC_MGR_H
#define CASE_RENDER_K_MODEL_GC_MGR_H

class IKG3DModel;
class KRLScene;

class KModelGCMgr : public IKG3DAnimationEventHandler
{
private:
	struct KRenderEntity
	{
        KRLScene* pRLScene;
		IKG3DModel* p3DModel;
		BOOL bSelectable;
	};

	typedef std::vector<IKG3DModel*> IKG3DModePtrlList;
	typedef std::vector<KRenderEntity> KRenderEntityList;

public:
	KModelGCMgr();
	~KModelGCMgr();

	int Init();
	void Exit();
	int Reset();
	int Activate(double fTime, double fTimeLast, DWORD dwGameLoop, BOOL bFrame);

	IKG3DModel* Create(LPCTSTR szFilePath);
	IKG3DModel* CreateAutoRemoveModel(LPCTSTR szFilePath, KRLScene* pRLScene);
	int Release(IKG3DModel* p3DModel);

protected:
	virtual HRESULT OnPlayAnimationFinish(IKG3DAnimationController* pController);
	virtual HRESULT OnTagPlayingSFX(IKG3DAnimationController* pController, IKG3DModel* pModel, TagSFXCallBackExtraInfo* pCallBackExtraInfo);
    virtual HRESULT OnMotionNotify(IKG3DAnimationController* pController, DWORD dwMotionInfo, TagMotionCallBackExtraInfo* pMotionCallBackExtraInfo);

	int Purge3DModelGC();

    int PurgeAutoRemoveRenderEntity();
    int PurgeAutoRemoveRenderEntityGC();

private:
    IKG3DModePtrlList m_a3DModelGC;

    KRenderEntityList m_aAutoRemoveRenderEntity;
	KRenderEntityList m_aAutoRemoveRenderEntityGC;
};

#endif // CASE_RENDER_K_MODEL_GC_MGR_H