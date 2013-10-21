#include "stdafx.h"
#include "Global.h"
#include "KSO3GameCenterSettings.h"

#define GCS_SECTION_GLOBAL      "Global"
#define GCS_SECTION_GATEWAY     "Gateway"
#define GCS_SECTION_GAMESERVER  "GameServer"
#define GCS_SECTION_MYSQL       "MySQL"
#define GCS_SECTION_MAIL        "Mail"
#define GCS_SECTION_FELLOWSHIP  "Fellowship"
#define GCS_SECTION_AUCTION     "Auction"
#define GCS_SECTION_ROLE        "Role"
#define GCS_SECTION_CAMP        "Camp"
#define GCS_SECTION_TONG        "Tong"
#define GCS_SECTION_APEXPROXY   "ApexProxy"
#define GCS_SECTION_LOGSERVER   "LogServer"
#define GCS_SECTION_ANTI_FARMER "AntiFarmer"

KSO3GameCenterSettings::KSO3GameCenterSettings(void)
{
    m_szLocale[0]                                = '\0';

    m_nMemoryLimitForCreateMap                   = 1400;
    m_fAveragePerformanceLimitForCreateMap       = 6.0f;
    m_fImmediatePerformanceLimitForCreateMap     = 8.0f;
    m_fAveragePerformanceLimitForNewPlayer       = 2.0f;
    m_fImmediatePerformanceLimitForNewPlayer     = 4.0f;
}

BOOL KSO3GameCenterSettings::Load()
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    bRetCode = LoadRelaySettings();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = LoadCenterConst();
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;

}
BOOL KSO3GameCenterSettings::LoadRelaySettings()
{
    BOOL        bResult     = false;
    int         nRetCode    = false;	
    IIniFile*	piIniFile   = NULL;
    int			nValue      = 0;

    piIniFile = g_OpenIniFile(RELAY_SETTINGS_FILENAME);
    KGLOG_PROCESS_ERROR(piIniFile);

    // Global
    nRetCode = piIniFile->GetString(GCS_SECTION_GLOBAL, "Locale", "", m_szLocale, sizeof(m_szLocale));
    KGLOG_PROCESS_ERROR(nRetCode);

    // Gateway config
    nRetCode = piIniFile->GetString(GCS_SECTION_GATEWAY, "IP", "127.0.0.1", m_szGatewayIP, sizeof(m_szGatewayIP));
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = piIniFile->GetInteger(GCS_SECTION_GATEWAY, "Port", 9001, &m_nGatewayPort);
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = piIniFile->GetInteger(GCS_SECTION_GATEWAY, "PingCycle", 30, (int*)&m_nGatewayPingCycle);
    KGLOG_PROCESS_ERROR(nRetCode);

    // GS config
    nRetCode = piIniFile->GetInteger(GCS_SECTION_GAMESERVER, "Port", 5003, &m_nGameServerPort);
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = piIniFile->GetInteger(GCS_SECTION_GAMESERVER, "ConnectionTimeout", 20, &m_nConnectionTimeout);
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = piIniFile->GetInteger(GCS_SECTION_GAMESERVER, "MemoryLimitForCreateMap", 1400, &m_nMemoryLimitForCreateMap);
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = piIniFile->GetFloat(GCS_SECTION_GAMESERVER, "AveragePerformanceLimitForCreateMap", 6.0f, &m_fAveragePerformanceLimitForCreateMap);
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = piIniFile->GetFloat(GCS_SECTION_GAMESERVER, "ImmediatePerformanceLimitForCreateMap", 8.0f, &m_fImmediatePerformanceLimitForCreateMap);
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = piIniFile->GetFloat(GCS_SECTION_GAMESERVER, "AveragePerformanceLimitForNewPlayer", 4.0f, &m_fAveragePerformanceLimitForNewPlayer);
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = piIniFile->GetFloat(GCS_SECTION_GAMESERVER, "ImmediatePerformanceLimitForNewPlayer", 6.0f, &m_fImmediatePerformanceLimitForNewPlayer);
    KGLOG_PROCESS_ERROR(nRetCode);

    // MySQL
    nRetCode = piIniFile->GetString(GCS_SECTION_MYSQL, "IP", "127.0.0.1", m_szDBIP, sizeof(m_szDBIP));
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = piIniFile->GetInteger(GCS_SECTION_MYSQL, "Port", 3306, &m_nDBPort);
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = piIniFile->GetString(GCS_SECTION_MYSQL, "Account", "", m_szDBAcc, sizeof(m_szDBAcc));
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = piIniFile->GetString(GCS_SECTION_MYSQL, "Password", "", m_szDBPsw, sizeof(m_szDBPsw));
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = piIniFile->GetString(GCS_SECTION_MYSQL, "Database", "", m_szDBName, sizeof(m_szDBName));
    KGLOG_PROCESS_ERROR(nRetCode);

    // Apex Proxy
    nRetCode = piIniFile->GetString(
        GCS_SECTION_APEXPROXY, "IP", "127.0.0.1", m_szApexServerIP, sizeof(m_szApexServerIP)
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piIniFile->GetInteger(GCS_SECTION_APEXPROXY, "Port", 10001, &m_nApexServerPort);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piIniFile->GetInteger(GCS_SECTION_APEXPROXY, "PingCycle", 5, &m_nApexPingCycle);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piIniFile->GetInteger(GCS_SECTION_APEXPROXY, "MaxKickCount", 1, &m_nMaxKickCount);
    KGLOG_PROCESS_ERROR(nRetCode);

    // LogServer
    nRetCode = piIniFile->GetString(GCS_SECTION_LOGSERVER, "IP", "127.0.0.1", m_szLogServerIP, sizeof(m_szLogServerIP));
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piIniFile->GetInteger(GCS_SECTION_LOGSERVER, "Port", 5100, &m_nLogServerPort);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piIniFile->GetInteger(GCS_SECTION_LOGSERVER, "PingCycle", 30, &m_nLogServerPingCycle);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piIniFile->GetString(GCS_SECTION_LOGSERVER, "Identity", "", m_szLogServerIdentity, sizeof(m_szLogServerIdentity));
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piIniFile);
    return bResult;
}

BOOL KSO3GameCenterSettings::LoadCenterConst()
{
    BOOL        bResult     = false;
	int         nRetCode    = false;	
    IIniFile*	piIniFile   = NULL;
	int			nValue      = 0;

	piIniFile = g_OpenIniFile(CENTER_CONST_FILENAME);
	KGLOG_PROCESS_ERROR(piIniFile);

    // Mail System config
    nRetCode = piIniFile->GetInteger(GCS_SECTION_MAIL, "SurvivalTime", 3600, &m_nMailSurvivalTime);
    KGLOG_PROCESS_ERROR(nRetCode && m_nMailSurvivalTime > 0);
    nRetCode = piIniFile->GetInteger(GCS_SECTION_MAIL, "MaxPlayerMailCount", 100, &m_nMaxPlayerMailCount);
    KGLOG_PROCESS_ERROR(nRetCode && m_nMaxPlayerMailCount > 0);
    nRetCode = piIniFile->GetInteger(GCS_SECTION_MAIL, "MaxAuctionMailCount", 100, &m_nMaxAuctionMailCount);
    KGLOG_PROCESS_ERROR(nRetCode && m_nMaxAuctionMailCount > 0);
    nRetCode = piIniFile->GetInteger(GCS_SECTION_MAIL, "MaxSystemMailCount", 100, &m_nMaxSystemMailCount);
    KGLOG_PROCESS_ERROR(nRetCode && m_nMaxSystemMailCount > 0);
    nRetCode = piIniFile->GetInteger(GCS_SECTION_MAIL, "MaxGmMsgMailCount", 50, &m_nMaxGmMsgMailCount);
    KGLOG_PROCESS_ERROR(nRetCode && m_nMaxGmMsgMailCount > 0);
    nRetCode = piIniFile->GetInteger(GCS_SECTION_MAIL, "MaxPlayerMsgMailCount", 50, &m_nMaxPlayerMsgMailCount);
    KGLOG_PROCESS_ERROR(nRetCode && m_nMaxPlayerMsgMailCount > 0);
    nRetCode = piIniFile->GetString(GCS_SECTION_MAIL, "SendFailedName", "System", m_szMailSendFailedName, sizeof(m_szMailSendFailedName));
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = piIniFile->GetString(GCS_SECTION_MAIL, "WithdrawName", "System", m_szMailWithdrawName, sizeof(m_szMailWithdrawName));
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = piIniFile->GetString(GCS_SECTION_MAIL, "UserReturnTitle", "Return", m_szMailUserReturnTitle, sizeof(m_szMailUserReturnTitle));
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = piIniFile->GetString(GCS_SECTION_MAIL, "SystemMailSender", "[SYSTEM]", m_szSystemMailSender, sizeof(m_szSystemMailSender));
    KGLOG_PROCESS_ERROR(nRetCode);

    // PlayerRelation System config
    nRetCode = piIniFile->GetInteger(GCS_SECTION_FELLOWSHIP, "SaveIntervalFrame", 60, &m_nFellowshipSaveInterval);
    KGLOG_PROCESS_ERROR(nRetCode && m_nFellowshipSaveInterval > 0);
    
    // Auction config
    nRetCode = piIniFile->GetString(GCS_SECTION_AUCTION, "AuctionName", "Auction", m_szAuctionName, sizeof(m_szAuctionName));
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = piIniFile->GetString(GCS_SECTION_AUCTION, "Gold", "Gold", m_szGold, sizeof(m_szGold));
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = piIniFile->GetString(GCS_SECTION_AUCTION, "Silver", "Silver", m_szSilver, sizeof(m_szSilver));
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = piIniFile->GetString(GCS_SECTION_AUCTION, "Copper", "Copper", m_szCopper, sizeof(m_szCopper));
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = piIniFile->GetString(GCS_SECTION_AUCTION, "SellSucceed", "SellSucceed", m_szSellSucceed, sizeof(m_szSellSucceed));
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = piIniFile->GetString(GCS_SECTION_AUCTION, "Buyyer", "Buyyer", m_szBuyyer, sizeof(m_szBuyyer));
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = piIniFile->GetString(GCS_SECTION_AUCTION, "Price", "Price", m_szPrice, sizeof(m_szPrice));
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = piIniFile->GetString(GCS_SECTION_AUCTION, "CustodyCharges", "CustodyCharges", m_szCustodyCharges, sizeof(m_szCustodyCharges));
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = piIniFile->GetString(GCS_SECTION_AUCTION, "TaxMoney", "TaxMoney", m_szTaxMoney, sizeof(m_szTaxMoney));
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = piIniFile->GetString(GCS_SECTION_AUCTION, "GetMoney", "GetMoney", m_szGetMoney, sizeof(m_szGetMoney));
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = piIniFile->GetString(GCS_SECTION_AUCTION, "CancelSucceed", "CancelSucceed", m_szCancelSucceed, sizeof(m_szCancelSucceed));
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = piIniFile->GetString(GCS_SECTION_AUCTION, "BidFailed", "BidFailed", m_szBidFailed, sizeof(m_szBidFailed));
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = piIniFile->GetString(GCS_SECTION_AUCTION, "BuySucceed", "BuySucceed", m_szBuySucceed, sizeof(m_szBuySucceed));
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = piIniFile->GetString(GCS_SECTION_AUCTION, "BuyItNowPrice", "BuyItNowPrice", m_szBuyItNowPrice, sizeof(m_szBuyItNowPrice));
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = piIniFile->GetString(GCS_SECTION_AUCTION, "TopPrice", "TopPrice", m_szTopPrice, sizeof(m_szTopPrice));
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = piIniFile->GetString(GCS_SECTION_AUCTION, "Seller", "Seller", m_szSeller, sizeof(m_szSeller));
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = piIniFile->GetString(GCS_SECTION_AUCTION, "Coin", "Coin", m_szCoin, sizeof(m_szCoin));
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = piIniFile->GetInteger(GCS_SECTION_AUCTION, "AuctionTaxRate", 5, &m_nAuctionTaxRate);
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = piIniFile->GetInteger(GCS_SECTION_AUCTION, "GameCardTaxRate", 5, &m_nGameCardTaxRate);
    KGLOG_PROCESS_ERROR(nRetCode);

    // Role config
    nRetCode = piIniFile->GetInteger(GCS_SECTION_ROLE, "MaxNameLen", 0, &m_nMaxRoleNameLength);
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = piIniFile->GetInteger(GCS_SECTION_ROLE, "MinNameLen", 0, &m_nMinRoleNameLength);
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = piIniFile->GetInteger(GCS_SECTION_ROLE, "MaxRoleCount", 0, &m_nMaxRoleCount);
    KGLOG_PROCESS_ERROR(nRetCode);
    /*
    nRetCode = piIniFile->GetInteger(GCS_SECTION_ROLE, "DeleteProtectLevel", 0, &m_nDeleteProtectLevel);
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = piIniFile->GetInteger(GCS_SECTION_ROLE, "DeleteDelayTime", 0, &m_nDeleteDelayTime);
    KGLOG_PROCESS_ERROR(nRetCode);
    */
    m_nDeleteProtectLevel   = 20;
    m_nDeleteDelayTime      = 7 * 24 * 60 * 60;

    // Tong
    nRetCode = piIniFile->GetString(
        GCS_SECTION_TONG, "TongReturnMoneyMailTitle", "tong system return money", 
        m_szTongReturnMoneyMailTitle, sizeof(m_szTongReturnMoneyMailTitle)
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    // AntiFarmer
    nRetCode = piIniFile->GetBool(GCS_SECTION_ANTI_FARMER, "IsEnable", &m_bAntiFarmerEnable);
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
	KG_COM_RELEASE(piIniFile);
	return bResult;
}
