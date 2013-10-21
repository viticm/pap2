

#ifndef _KGSFXTreeCtrl_H_
#define _KGSFXTreeCtrl_H_

class IEKG3DSFX;

enum {
    SFX_ELEMENT_SFX,
    SFX_ELEMENT_LAU,
    SFX_ELEMENT_BIL,
    SFX_ELEMENT_MDL,
    SFX_ELEMENT_TRA
};

class KGSfxTreeCtrl : public CTreeCtrl
{
	DECLARE_DYNAMIC(KGSfxTreeCtrl)

public :

    /* bind the sfx to tree */
    void BindSfx(IEKG3DSFX* pSfx)   { m_pSfx = pSfx; UpdateUI(); }
    
    /* update the ui */
    void UpdateUI();

	KGSfxTreeCtrl();
	virtual ~KGSfxTreeCtrl();

private :
    IEKG3DSFX*  m_pSfx;

protected :
	DECLARE_MESSAGE_MAP()
};

#endif
