#include "stdafx.h"
#include "./kgwtextatom.h"


KGWTextAtom::KGWTextAtom()
{
}


KGWTextAtom::KGWTextAtom(KGWTextAtom const& rhs)
	: m_strName(rhs.m_strName)
	, m_strContext(rhs.m_strContext)
	, m_aAttribute(rhs.m_aAttribute)
{
}


KGWTextAtom::~KGWTextAtom()
{
}


KGWTextAtom& KGWTextAtom::operator=(KGWTextAtom const& rhs)
{
	if (this != &rhs)
	{
		m_strName = rhs.m_strName;
		m_strContext = rhs.m_strContext;
		m_aAttribute = rhs.m_aAttribute;
	}
	return *this;
}


bool KGWTextAtom::operator<(KGWTextAtom const& rhs) const
{
	ASSERT(0 && "bool KGWTextAtom::operator<(KGWTextAtom const& rhs) const\n");
	return false;
}


void KGWTextAtom::SetName(LPCWSTR wszName)
{
	ASSERT(wszName);

	m_strName = wszName;
}


void KGWTextAtom::SetName(LPCWSTR wszName, size_t nLength)
{
	ASSERT(wszName);
	ASSERT(nLength);

	m_strName.assign(wszName, wszName + nLength);
}


LPCWSTR	KGWTextAtom::GetName() const
{
	return m_strName.c_str();
}


void KGWTextAtom::SetContext(LPCWSTR wszContext)
{
	ASSERT(wszContext);

	m_strContext = wszContext;
}


void KGWTextAtom::SetContext(LPCWSTR wszContext, size_t nLength)
{
	ASSERT(wszContext);

	m_strContext.assign(wszContext, wszContext + nLength);
}


LPCWSTR	KGWTextAtom::GetContext() const
{
	return m_strContext.c_str();
}


bool KGWTextAtom::IsEmpty() const
{
	return m_strName.empty() && m_strContext.empty() && m_aAttribute.empty();
}


void KGWTextAtom::Clear()
{
	m_strName.clear();
	m_strContext.clear();
	m_aAttribute.clear();
}


void KGWTextAtom::PushAttribute(LPCWSTR wszKey, LPCWSTR wszValue)
{
	ASSERT(wszKey);
	ASSERT(wszValue);

	PushAttribute(KGWTextAtomAttribute(wszKey, wszValue));
}


void KGWTextAtom::PushAttribute(KGWTextAtomAttribute const& attribute)
{
	m_aAttribute.push_back(attribute);
}


void KGWTextAtom::PushAttribute(LPCWSTR wszKey, LPCWSTR wszValue, size_t nKeyLength, size_t nValueLength)
{
	ASSERT(wszKey);
	ASSERT(wszValue);
	ASSERT(nKeyLength);
	ASSERT(nValueLength);

    std::wstring strKey(wszKey, wszKey + nKeyLength);
    std::wstring strValue(wszValue, wszValue + nValueLength);

	PushAttribute(strKey.c_str(), strValue.c_str());
}


size_t KGWTextAtom::GetAttributeCount() const
{
	return static_cast<size_t>(m_aAttribute.size());
}


LPCWSTR	KGWTextAtom::GetAttributeKey(size_t nIndex) const
{
	ASSERT(nIndex < GetAttributeCount());
	return m_aAttribute[nIndex].GetKey();
}


LPCWSTR	KGWTextAtom::GetAttributeValue(size_t nIndex) const
{
	ASSERT(nIndex < GetAttributeCount());
	return m_aAttribute[nIndex].GetValue();
}


bool operator==(KGWTextAtom const& lhs, KGWTextAtom const& rhs)
{
	// TODO:
	return false;
}

