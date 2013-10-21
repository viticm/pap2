#ifndef __KG3DSFXELEMENT_H__
#define __KG3DSFXELEMENT_H__

class KG3DSFXElement
{
public:
	virtual HRESULT RenderInEditor();
	virtual HRESULT SetCurrentFrame(float fFrame);

public:
	KG3DSFXElement();
	virtual ~KG3DSFXElement();

protected:
    float m_fCurrentFrame;
};

#endif //__KG3DSFXELEMENT_H__