#ifndef UI_CASE_K_GAME_WORLD_TEXT_ENCODER_H
#define UI_CASE_K_GAME_WORLD_TEXT_ENCODER_H

#include "./kgwtextencoder.h"

class KGameWorldTextEncoder
{
public:
	static int Init();
	static void Exit();

	static bool Reset();
	static bool ResetTalk();

	static bool EncodeParam(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom);
	static bool EncodeString(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom);
	static bool EncodePlayer(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom);
	static bool EncodeItem(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom);
	static bool EncodeQuest(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom);
	static bool EncodeOwner(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom);
	static bool EncodeShop(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom);
	static bool EncodeSelf(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom);
	static bool EncodeRoleType(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom);

	static bool EncodeMail(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom);
	static bool EncodeGuildBank(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom);
	static bool EncodeBank(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom);

	static bool EncodeFont(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom);
	static bool EncodePic(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom);
	static bool EncodeAnimate(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom);
	static bool EncodeHeightControl(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom);
    static bool EncodeBattleField(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom);

	static bool EncodeSkillFormula(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom);
	static bool EncodeCraftFormula(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom);
	static bool EncodeSpace(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom);
	static bool EncodeMoney(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom);
	static bool EncodeTraffic(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom);
	static bool EncodeSureParam(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom);

	static bool EncodeAuction(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom);
	static bool EncodeEdit(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom);

	static bool EncodeSound(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom);
	static bool EncodeAction(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom);
	static bool EncodeWait(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom);
	static bool EncodePage(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom);

    static bool EncodeCardSell(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom);
    static bool EncodeCardBuy(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom);

	KGameWorldTextEncoder();
	virtual ~KGameWorldTextEncoder();
private:
	static int	ms_nEncoded;
	static int	ms_nParam;
	static int	ms_nString;
	static int	ms_nPlayer;
	static int	ms_nItem;
	static int	ms_nQuest;
	static int  ms_nOwner;
	static int  ms_nShop;
	static int  ms_nSelf;
	static int  ms_nRoleType;

	static int  ms_nMail;
	static int	ms_GuildBank;
	static int  ms_nBank;

	static int  ms_nFont;
	static int  ms_nPic;
	static int  ms_nAnimate;
	static int	ms_nHeightControl;
    static int  ms_nBattleField;
	static int	ms_nSkillFormula;
	static int	ms_nCraftFormula;
	static int	ms_nSpace;
	static int	ms_nMoney;
	static int	ms_nTraffic;
	static int	ms_nAuction;
	static int	ms_nEdit;

	static int	ms_nSound;
	static int	ms_nAction;
	static int	ms_nWait;
	static int	ms_nPage;

    static int ms_nCardSell;
    static int ms_nCardBuy;
};

#endif // UI_CASE_K_GAME_WORLD_TEXT_ENCODER_H


