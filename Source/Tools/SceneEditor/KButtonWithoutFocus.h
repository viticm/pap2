#pragma once


// KButtonWithoutFocus
namespace PropertyBase
{

class KButtonWithoutFocus : public CButton
{
	DECLARE_DYNAMIC(KButtonWithoutFocus)

public:
	KButtonWithoutFocus();
	virtual ~KButtonWithoutFocus();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
};

}



