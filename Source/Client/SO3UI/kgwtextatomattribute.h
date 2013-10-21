#ifndef UI_TEXT_K_GW_TEXT_ATOM_ATTRIBUTE_H
#define UI_TEXT_K_GW_TEXT_ATOM_ATTRIBUTE_H

class KGWTextAtomAttribute
{
	friend bool operator==(KGWTextAtomAttribute const& lhs, KGWTextAtomAttribute const& rhs);

public:
	KGWTextAtomAttribute();
	KGWTextAtomAttribute(LPCWSTR wszKey, LPCWSTR wszValue);

	~KGWTextAtomAttribute();

	KGWTextAtomAttribute(KGWTextAtomAttribute const& rhs);
	KGWTextAtomAttribute& operator=(KGWTextAtomAttribute const& rhs);

	LPCWSTR GetKey() const;
	LPCWSTR GetValue() const;
	void	SetKey(LPCWSTR wszKey);
	void	SetValue(LPCWSTR wszValue);

private:
    std::wstring m_strKey;
    std::wstring m_strValue;
};


bool operator==(KGWTextAtomAttribute const& lhs, KGWTextAtomAttribute const& rhs);

#endif // UI_TEXT_K_GW_TEXT_ATOM_ATTRIBUTE_H

