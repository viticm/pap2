#pragma once

class IEKG3DScene;

class KEditorDocumentBase
{
protected:
	IEKG3DScene* m_lpScene;
public:

	HRESULT SetScene(IEKG3DScene* pScene)
	{
		m_lpScene = pScene;
		return S_OK;
	}
	IEKG3DScene* GetScene()
	{
		return m_lpScene;
	}
	HRESULT OnClose();

public:
	KEditorDocumentBase(void);
	virtual ~KEditorDocumentBase(void);
};
