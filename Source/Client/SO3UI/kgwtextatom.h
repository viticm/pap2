#ifndef UI_TEXT_K_GW_TEXT_ATOM_H
#define UI_TEXT_K_GW_TEXT_ATOM_H

#include "./kgwtextatomattribute.h"


class KGWTextAtom
{
private:
	typedef std::vector<KGWTextAtomAttribute>	KAttributeList;

public:
	KGWTextAtom();
	KGWTextAtom(KGWTextAtom const& rhs);
	~KGWTextAtom();

	KGWTextAtom& operator=(KGWTextAtom const& rhs);

protected:
	bool operator<(KGWTextAtom const& rhs) const;

public:
	void	SetName(LPCWSTR wszName);
	void	SetName(LPCWSTR wszName, size_t nLength);
	LPCWSTR	GetName() const;

	void	SetContext(LPCWSTR wszContext);
	void	SetContext(LPCWSTR wszContext, size_t nLength);
	LPCWSTR	GetContext() const;

	bool	IsEmpty() const;
	void	Clear();

	void	PushAttribute(LPCWSTR wszKey, LPCWSTR wszValue);
	void	PushAttribute(LPCWSTR wszKey, LPCWSTR wszValue, size_t nKeyLength, size_t nValueLength);
	void	PushAttribute(KGWTextAtomAttribute const& attribute);

	size_t	GetAttributeCount() const;
	LPCWSTR	GetAttributeKey(size_t nIndex) const;
	LPCWSTR	GetAttributeValue(size_t nIndex) const;

private:
    std::wstring	m_strName;
    std::wstring	m_strContext;
	KAttributeList	m_aAttribute;
};


bool operator==(KGWTextAtom const& lhs, KGWTextAtom const& rhs);

#endif // UI_TEXT_K_GW_TEXT_ATOM_H


