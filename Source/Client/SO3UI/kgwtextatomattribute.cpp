#include "stdafx.h"
#include "./kgwtextatomattribute.h"


KGWTextAtomAttribute::KGWTextAtomAttribute()
{
}


KGWTextAtomAttribute::KGWTextAtomAttribute(LPCWSTR wszKey, LPCWSTR wszValue)
	: m_strKey(wszKey)
	, m_strValue(wszValue)
{
	ASSERT(wszKey && wszKey[0] != L'\0');
	ASSERT(wszValue);
}


KGWTextAtomAttribute::~KGWTextAtomAttribute()
{
}


KGWTextAtomAttribute::KGWTextAtomAttribute(KGWTextAtomAttribute const& rhs)
: m_strKey(rhs.m_strKey)
, m_strValue(rhs.m_strValue)
{
}


KGWTextAtomAttribute& KGWTextAtomAttribute::operator=(KGWTextAtomAttribute const& rhs)
{
	if (this != &rhs)
	{
		m_strKey = rhs.m_strKey;
		m_strValue = rhs.m_strValue;
	}
	return *this;
}


LPCWSTR KGWTextAtomAttribute::GetKey() const
{
	return m_strKey.c_str();
}


LPCWSTR KGWTextAtomAttribute::GetValue() const
{
	return m_strValue.c_str();
}


void KGWTextAtomAttribute::SetKey(LPCWSTR wszKey)
{
	ASSERT(wszKey && wszKey[0] != L'\0');
	m_strKey = wszKey;
}


void KGWTextAtomAttribute::SetValue(LPCWSTR wszValue)
{
	ASSERT(wszValue);
	m_strValue = wszValue;
}

bool operator==(KGWTextAtomAttribute const& lhs, KGWTextAtomAttribute const& rhs)
{
	return &lhs == &rhs || lhs.m_strKey == rhs.m_strKey && lhs.m_strValue == rhs.m_strValue;
}