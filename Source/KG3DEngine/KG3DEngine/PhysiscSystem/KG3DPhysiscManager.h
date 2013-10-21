#pragma once

class KG3DPhysiscScene;
class NxPhysicsSDK;

class KG3DPhysisErrorReport : public NxUserOutputStream
{
    virtual void reportError(NxErrorCode code, const char *message, const char* file, int line);
    virtual NxAssertResponse reportAssertViolation(const char *message, const char *file,int line);
    virtual void print(const char *message);
};

class KG3DPhysiscManager
{
public:
    HRESULT Init();
    HRESULT UnInit();

	HRESULT NewOnePhysiscScene(KG3DPhysiscScene **ppScene);
    HRESULT DeleteOnePhysiscScene(KG3DPhysiscScene *pScene);

    HRESULT UpdateRemoteDebuggerCamera();

	KG3DPhysiscManager(void);
	virtual ~KG3DPhysiscManager(void);

private:
    // when create or release the physic scene, it need all of the scene not in simulate state
    // the NVIDIA said that is for better performance, but I think this is not a good solution,
    // but now we only think we can do is call the following
    HRESULT _ForceAllSceneFinishSimulate();

private:
    NxPhysicsSDK *m_pPhysicsSDK;
    KG3DPhysisErrorReport m_ErrorReport;
    int m_nDebuggerIsConnectedFlag;

    list<KG3DPhysiscScene *> m_listPhysicsScene;
};

