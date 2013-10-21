#ifndef UI_TEXT_K_GW_TEXT_ENCODER_H
#define UI_TEXT_K_GW_TEXT_ENCODER_H

#include "./kgwtextatom.h"

class KGWTextEncoder
{
public:
	enum { PATTERN_LENGTH = 32 };

	typedef bool (*FnEncoder)(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom);
	typedef bool (*FnReset)();

	class KEncoderItem
	{
	public:
		bool operator<(KEncoderItem const& rhs) const
		{
			return wcscmp(wszBegin, rhs.wszBegin) > 0;
		}

	public:
		WCHAR		wszBegin[PATTERN_LENGTH];
		WCHAR		wszEnd[PATTERN_LENGTH];
		size_t		nBegin;
		size_t		nEnd;
		FnEncoder	fnEncoder;
	};

	typedef std::vector<KGWTextAtom>	KGWTextAtomList;
	typedef std::vector<KEncoderItem>	KGWTexEncoderItemList;

protected:
	KGWTextEncoder();
	~KGWTextEncoder();

public:
	static KGWTextEncoder& GetSelf();
	static KGWTextEncoder& GetTalkEncoder();

	void Reset();

	bool RegisterPattern(LPCWSTR wszPatternBegin, LPCWSTR wszPatternEnd, FnEncoder fn);

	bool RegisterReset(FnReset fn);

	bool Encode(LPCWSTR wszStream, size_t nStreamLength, KGWTextAtomList& aEncodedAtom, size_t& nEncodedLength) const;

private:
	bool IsExist(LPCWSTR wszPatternBegin) const;
	bool EncodePattern(LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom, size_t& nEncodedLength) const;
	bool EncodePlainText(LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom, size_t& nEncodedLength) const;
	LPCWSTR	FindPattern(LPCWSTR wszStream, size_t nStreamLength) const;

private:
	KGWTexEncoderItemList	m_aEncoder;

	FnReset					m_fnReset;
};


#endif // UI_TEXT_K_GW_TEXT_ENCODER_H



