#pragma once
#include "kg3drepresentobject.h"

class KG3DDataFlowProcessor;
class KG3DSceneDataFlowEditor;

class KG3DRepresentObjectDataFlowProcessor :
	public KG3DRepresentObject
{
	enum ShapeType
	{
		SHAPETYPE_ROUNDRECT,
		SHAPETYPE_TRIANGLE,
		SHAPETYPE_CIRCLE,
		SHAPETYPE_DATAMODIFY,
	}m_eShapeType;

	float m_fSize;

protected:
	KG3DDataFlowProcessor* m_lpProcessor;
public:
	HRESULT Refresh(KG3DSceneDataFlowEditor* pEditor);

	KG3DDataFlowProcessor* GetProcessor()
	{
		return m_lpProcessor;
	}

	virtual HRESULT RenderSelectMark(DWORD dwColor /* = 0xFFFF0000 */);

	virtual HRESULT ComputeBBox();
	HRESULT SetProcessor(KG3DDataFlowProcessor* pProcessor);

	HRESULT RenderShapeDataModify();

	HRESULT RenderShapeRoundRect();
	HRESULT RenderShapeTriangle();
	HRESULT RenderShapeCircle();
	HRESULT RenderShapeConnectionPoint(D3DXVECTOR3 A,D3DXVECTOR3 B,DWORD color);

	virtual HRESULT GetInputConnectionPoint(DWORD dwIndex,D3DXVECTOR3& vConnectionPos,D3DXVECTOR3& vStartPos);
	virtual HRESULT GetOutputConnectionPoint(DWORD dwIndex,D3DXVECTOR3& vConnectionPos,D3DXVECTOR3& vStartPos);

	virtual HRESULT Render(DWORD dwOption);

	KG3DRepresentObjectDataFlowProcessor(void);
	virtual ~KG3DRepresentObjectDataFlowProcessor(void);
};
