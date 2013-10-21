# SQL Manager 2005 Lite for MySQL 3.6.5.1
# ---------------------------------------
# Host     : localhost
# Port     : 3306
# Database : fsonline2


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES gb2312 */;

SET FOREIGN_KEY_CHECKS=0;

DROP DATABASE IF EXISTS `fsonline2`;

CREATE DATABASE `fsonline2`
    CHARACTER SET 'gb2312'
    COLLATE 'gb2312_chinese_ci';

USE `fsonline2`;

#
# Structure for the `auction` table :
#

DROP TABLE IF EXISTS `auction`;

CREATE TABLE `auction` (
  `Id` int(6) unsigned NOT NULL auto_increment,
  `SellerName` varbinary(64) NOT NULL,
  `SellerGUID` varbinary(64) NOT NULL,
  `GoodsName` varbinary(64) NOT NULL,
  `GoodsGUID` varbinary(64) NOT NULL,
  `BuyerName` varbinary(64) default NULL,
  `BuyerGUID` varbinary(64) default NULL,
  `StartTime` datetime NOT NULL,
  `FinishTime` datetime NOT NULL,
  `CurrentPrice` int(4) unsigned default NULL,
  `PassPrice` int(4) unsigned default '0',
  `ItemData` blob NOT NULL,
  `OperationTime` datetime NOT NULL,
  `OperationMark` varbinary(64) default NULL,
  `QualityLabel` int(4) NOT NULL,
  `RequireLevel` int(4) NOT NULL,
  `ItemId1` int(4) NOT NULL,
  `ItemId2` int(4) NOT NULL,
  `RequireProfession` int(4) NOT NULL,
  PRIMARY KEY  (`Id`),
  KEY `SellerIndex` (`SellerName`),
  KEY `BuyerIndex` (`BuyerName`),
  KEY `FinishTimeIndex` (`FinishTime`),
  KEY `ItemId1Index` (`ItemId1`),
  KEY `ItemId2Index` (`ItemId2`),
  KEY `QualityLabelIndex` (`QualityLabel`),
  KEY `RequireLevelIndex` (`RequireLevel`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Structure for the `faction` table :
#

DROP TABLE IF EXISTS `faction`;

CREATE TABLE `faction` (
  `Field1` varchar(3) NOT NULL COMMENT '门派ID',
  `Field2` varchar(20) NOT NULL COMMENT '门派路线名',
  `Field3` varchar(3) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;

#
# Structure for the `global` table :
#

DROP TABLE IF EXISTS `global`;

CREATE TABLE `global` (
  `GlobalId` int(4) unsigned NOT NULL,
  `GlobalData` blob,
  PRIMARY KEY  (`GlobalId`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Structure for the `globalmail` table :
#

DROP TABLE IF EXISTS `globalmail`;

CREATE TABLE `globalmail` (
  `Id` int(4) unsigned NOT NULL auto_increment,
  `Sender` varbinary(64) NOT NULL,
  `Title` varbinary(64) NOT NULL,
  `PostTime` datetime NOT NULL,
  `ExpireTime` datetime NOT NULL,
  `SenderType` int(1) unsigned NOT NULL,
  `MailType` int(1) unsigned NOT NULL,
  `MailData` blob,
  `MailPlus` blob,
  `PostMoney` int(4) unsigned NOT NULL,
  `MailCost` int(4) unsigned NOT NULL,
  PRIMARY KEY  (`Id`),
  KEY `ExpireTimeIndex` (`ExpireTime`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Structure for the `ibitem` table :
#

DROP TABLE IF EXISTS `ibitem`;

CREATE TABLE `ibitem` (
  `ItemId` int(6) unsigned NOT NULL auto_increment,
  `Genre` int(4) NOT NULL default '0',
  `Detail` int(4) NOT NULL default '0',
  `Particular` int(4) NOT NULL default '0',
  `Level` int(4) NOT NULL default '0',
  `Price` int(11) unsigned NOT NULL default '0',
  `Discount` int(4) unsigned NOT NULL default '0',
  `SaleCount` int(11) unsigned NOT NULL default '0',
  `State` int(4) NOT NULL default '0',
  `Label` int(4) NOT NULL default '0',
  PRIMARY KEY  (`ItemId`),
  UNIQUE KEY `ItemTemplateID` (`Genre`,`Detail`,`Particular`,`Level`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Structure for the `ibpanel` table :
#

DROP TABLE IF EXISTS `ibpanel`;

CREATE TABLE `ibpanel` (
  `Id` int(4) unsigned NOT NULL auto_increment,
  `Left` smallint(4) NOT NULL default '0',
  `Top` smallint(4) NOT NULL default '0',
  `Width` smallint(4) unsigned NOT NULL default '0',
  `Height` smallint(4) unsigned NOT NULL default '0',
  `Margin` smallint(4) unsigned NOT NULL default '0',
  `CellWidth` smallint(4) unsigned NOT NULL default '0',
  `CellHeight` smallint(4) unsigned NOT NULL default '0',
  `TitleWidth` smallint(4) unsigned NOT NULL default '0',
  `TitleHeight` smallint(4) unsigned NOT NULL default '0',
  `TitleBody` blob NOT NULL,
  `Theme` smallint(4) unsigned NOT NULL default '0',
  PRIMARY KEY  (`Id`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Structure for the `ibrecord` table :
#

DROP TABLE IF EXISTS `ibrecord`;

CREATE TABLE `ibrecord` (
  `ItemGUID` varbinary(64) NOT NULL,
  `Genre` int(4) NOT NULL default '0',
  `Detail` int(4) NOT NULL default '0',
  `Particular` int(4) NOT NULL default '0',
  `Level` int(4) NOT NULL default '0',
  `State` int(4) NOT NULL default '0',
  `OwnerName` varbinary(64) NOT NULL,
  `BuyerName` varbinary(64) NOT NULL,
  `CreateDate` datetime NOT NULL,
  `DeleteDate` datetime default NULL,
  PRIMARY KEY  (`ItemGUID`),
  KEY `OwnerNameIndex` (`OwnerName`),
  KEY `BuyerNameIndex` (`BuyerName`),
  KEY `ItemTemplateIndex` (`Genre`,`Detail`,`Particular`,`Level`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Structure for the `ibshelf` table :
#

DROP TABLE IF EXISTS `ibshelf`;

CREATE TABLE `ibshelf` (
  `ShelfIndex` smallint(4) unsigned NOT NULL default '0',
  `Name` varbinary(64) NOT NULL,
  `Panel0` int(4) unsigned NOT NULL default '0',
  `Panel1` int(4) unsigned NOT NULL default '0',
  `Panel2` int(4) unsigned NOT NULL default '0',
  `Panel3` int(4) unsigned NOT NULL default '0',
  `Panel4` int(4) unsigned NOT NULL default '0',
  `Panel5` int(4) unsigned NOT NULL default '0',
  `Panel6` int(4) unsigned NOT NULL default '0',
  `Panel7` int(4) unsigned NOT NULL default '0',
  `Panel8` int(4) unsigned NOT NULL default '0',
  `Panel9` int(4) unsigned NOT NULL default '0',
  PRIMARY KEY  (`ShelfIndex`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Structure for the `ibshelfitem` table :
#

DROP TABLE IF EXISTS `ibshelfitem`;

CREATE TABLE `ibshelfitem` (
  `ShelfIndex` smallint(4) unsigned NOT NULL default '0',
  `PanelIndex` smallint(4) unsigned NOT NULL default '0',
  `IndexInPanel` smallint(4) unsigned NOT NULL default '0',
  `ItemId` int(6) unsigned NOT NULL default '0',
  `Style` int(4) unsigned NOT NULL default '0',
  PRIMARY KEY  (`ShelfIndex`,`PanelIndex`,`IndexInPanel`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Structure for the `ibstyle` table :
#

DROP TABLE IF EXISTS `ibstyle`;

CREATE TABLE `ibstyle` (
  `Id` int(4) unsigned NOT NULL auto_increment,
  `Template` blob NOT NULL,
  PRIMARY KEY  (`Id`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Structure for the `itembank` table :
#

DROP TABLE IF EXISTS `itembank`;

CREATE TABLE `itembank` (
  `PlayerName` varbinary(64) NOT NULL,
  `Genre` int(4) NOT NULL default '0',
  `Detail` int(4) NOT NULL default '0',
  `Particular` int(4) NOT NULL default '0',
  `Level` int(4) NOT NULL default '0',
  `ItemCount` int(11) unsigned NOT NULL default '0',
  PRIMARY KEY  (`PlayerName`,`Genre`,`Detail`,`Particular`,`Level`),
  KEY `ItemTemplateIndex` (`Genre`,`Detail`,`Particular`,`Level`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Structure for the `itemlist` table :
#

DROP TABLE IF EXISTS `itemlist`;

CREATE TABLE `itemlist` (
  `ItemId` varchar(48) NOT NULL,
  `CreateTime` datetime default NULL COMMENT '生成时间',
  `RoleId` varchar(48) NOT NULL,
  `Genre` int(11) default NULL,
  `DetailType` int(11) default NULL,
  `ParticularType` int(11) default NULL,
  `Level` tinyint(4) default NULL,
  `Series` tinyint(4) default NULL,
  `Seed` int(10) unsigned default NULL,
  `StackCount` int(11) default NULL,
  `StateFlag` tinyint(3) unsigned default NULL,
  `UnlockTime` int(10) unsigned default NULL,
  `PlaceRoom` tinyint(3) unsigned default NULL,
  `PlaceX` tinyint(3) unsigned default NULL,
  `PlaceY` tinyint(3) unsigned default NULL,
  `CustomType` tinyint(3) unsigned default NULL,
  `Custom` char(32) default NULL,
  `CurDur` smallint(5) unsigned default NULL,
  `MaxDur` smallint(5) unsigned default NULL,
  `Luck` int(10) unsigned default NULL,
  `Enhance` tinyint(3) unsigned default NULL,
  `Param1` int(10) unsigned default NULL,
  `Param2` int(10) unsigned default NULL,
  `Param3` int(10) unsigned default NULL,
  `Param4` int(10) unsigned default NULL,
  `Param5` int(10) unsigned default NULL,
  `Param6` int(10) unsigned default NULL,
  `Param7` int(10) unsigned default NULL,
  `Param8` int(10) unsigned default NULL,
  `Param9` int(10) unsigned default NULL,
  `Param10` int(10) unsigned default NULL,
  `Param11` int(10) unsigned default NULL,
  `Param12` int(10) unsigned default NULL,
  `TimeoutType` tinyint(3) unsigned default NULL,
  `TimeoutValue` int(10) unsigned default NULL,
  KEY `ItemId` (`ItemId`),
  KEY `RoleId` (`RoleId`),
  KEY `Genre` (`Genre`,`DetailType`,`ParticularType`,`Level`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;

#
# Structure for the `kin` table :
#

DROP TABLE IF EXISTS `kin`;

CREATE TABLE `kin` (
  `ID` int(11) NOT NULL auto_increment,
  `LogTime` datetime default NULL,
  `Source` tinyint(4) default NULL,
  `IP` varchar(128) default NULL,
  `Level` tinyint(4) default NULL,
  `Field1` varchar(128) NOT NULL COMMENT '??ID',
  `Field2` varchar(128) NOT NULL COMMENT '????',
  PRIMARY KEY  (`ID`),
  UNIQUE KEY `ID` (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;



#
# Structure for the `mails` table :
#

DROP TABLE IF EXISTS `mails`;

CREATE TABLE `mails` (
  `Id` int(4) unsigned NOT NULL auto_increment,
  `Sender` varbinary(64) NOT NULL,
  `Receiver` varbinary(64) NOT NULL,
  `Title` varbinary(64) NOT NULL,
  `PostTime` datetime NOT NULL,
  `ExpireTime` datetime NOT NULL,
  `State` int(1) unsigned NOT NULL,
  `SenderType` int(1) unsigned NOT NULL,
  `MailType` int(1) unsigned NOT NULL,
  `MailData` blob,
  `MailPlus` blob,
  `PostMoney` int(4) unsigned NOT NULL,
  `MailCost` int(4) unsigned NOT NULL,
  PRIMARY KEY  (`Id`),
  KEY `SenderIndex` (`Sender`),
  KEY `ReceiverIndex` (`Receiver`),
  KEY `ExpireTimeIndex` (`ExpireTime`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Structure for the `map` table :
#

DROP TABLE IF EXISTS `map`;

CREATE TABLE `map` (
  `MapID` int(11) NOT NULL,
  `MapName` varchar(48) NOT NULL,
  PRIMARY KEY  (`MapID`),
  UNIQUE KEY `MapID` (`MapID`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312 COMMENT='InnoDB free: 10240 kB';

#
# Structure for the `npcsave` table :
#

DROP TABLE IF EXISTS `npcsave`;

CREATE TABLE `npcsave` (
  `GUID` varbinary(64) NOT NULL,
  `MapId` int(10) unsigned NOT NULL,
  `PosX` int(10) unsigned NOT NULL,
  `PosY` int(10) unsigned NOT NULL,
  `LifePercentage` int(10) unsigned NOT NULL,
  `ManaPercentage` int(10) unsigned NOT NULL,
  `BuffData` blob,
  `LordGUID` varbinary(64) NOT NULL,
  `RobberGUID` varbinary(64) NOT NULL,
  `NpcRes` blob,
  `CityTaxRate` int(11) NOT NULL,
  PRIMARY KEY  (`GUID`,`MapId`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Structure for the `playeraction` table :
#

DROP TABLE IF EXISTS `playeraction`;

CREATE TABLE `playeraction` (
  `Id` int(10) unsigned NOT NULL auto_increment,
  `RecordTime` datetime NOT NULL,
  `PlayerGuid` varbinary(64) NOT NULL,
  `PlayerAction` int(1) unsigned NOT NULL,
  `ActionDesc` varbinary(64) NOT NULL,
  PRIMARY KEY  (`Id`),
  KEY `PlayerGuidIndex` (`PlayerGuid`),
  KEY `PlayerActionIndex` (`PlayerAction`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Structure for the `procedureerror` table :
#

DROP TABLE IF EXISTS `procedureerror`;

CREATE TABLE `procedureerror` (
  `Id` int(10) unsigned NOT NULL auto_increment,
  `LogTime` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  `LogMessage` varbinary(64) NOT NULL,
  PRIMARY KEY  (`Id`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Structure for the `rolesdead` table :
#

DROP TABLE IF EXISTS `rolesdead`;

CREATE TABLE `rolesdead` (
  `Id` int(4) unsigned NOT NULL,
  `Version` int(4) unsigned NOT NULL,
  `TongGUID` varbinary(64) NOT NULL,
  `AccountName` varbinary(64) NOT NULL,
  `RoleName` varbinary(64) NOT NULL,
  `RoleSex` int(1) unsigned NOT NULL,
  `RoleType` int(1) unsigned NOT NULL,
  `RoleLevel` int(1) unsigned NOT NULL,
  `Money` int(4) unsigned NOT NULL,
  `MoneyInBox` int(4) unsigned NOT NULL,
  `PlayedTime` int(4) unsigned NOT NULL,
  `CreateDate` datetime NOT NULL,
  `ExchangeDate` datetime NOT NULL,
  `LastPlayingDate` datetime NOT NULL,
  `LastPlayingIP` int(4) unsigned NOT NULL,
  `NoChatIn` int(4) unsigned NOT NULL,
  `NoLoginIn` int(4) unsigned NOT NULL,
  `UseRevivePosition` int(1) unsigned NOT NULL,
  `ReviveID` int(4) NOT NULL,
  `ReviveX` int(4) NOT NULL,
  `ReviveY` int(4) NOT NULL,
  `EnterMapID` int(4) NOT NULL,
  `BaseInfo` tinyblob,
  `NumericInfo` tinyblob,
  `PasswordInfo` tinyblob,
  `TongWarInfo` tinyblob,
  `PetInfo` tinyblob,
  `OfflinePlayingInfo` tinyblob,
  `CompensatoryInfo` tinyblob,
  `OtherInfo` tinyblob,
  `ReserveInfo` tinyblob,
  `SettingInfo` tinyblob,
  `SkillList` blob,
  `ItemList` blob,
  `TaskList` blob,
  `EnhanceList` blob,
  `FriendsList` blob,
  `ReserveList` blob,
  `GUID` varbinary(64) NOT NULL,
  `Portrait` int(4) NOT NULL,
  `Online` int(4) NOT NULL,
  `SpyLevel` int(4) NOT NULL,
  `SkillSeries` int(4) NOT NULL,
  `GlobalMailDate` datetime NOT NULL,
  PRIMARY KEY  (`Id`),
  KEY `RoleIndex` (`RoleName`),
  KEY `AccountIndex` (`AccountName`),
  KEY `LastPlayingIndex` (`LastPlayingDate`),
  KEY `GUIDIndex` (`GUID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;


#
# Structure for the `rolesdelete` table :
#

DROP TABLE IF EXISTS `rolesdelete`;

CREATE TABLE `rolesdelete` (
  `Id` int(4) unsigned NOT NULL,
  `Version` int(4) unsigned NOT NULL,
  `TongGUID` varbinary(64) NOT NULL,
  `AccountName` varbinary(64) NOT NULL,
  `RoleName` varbinary(64) NOT NULL,
  `RoleSex` int(1) unsigned NOT NULL,
  `RoleType` int(1) unsigned NOT NULL,
  `RoleLevel` int(1) unsigned NOT NULL,
  `Money` int(4) unsigned NOT NULL,
  `MoneyInBox` int(4) unsigned NOT NULL,
  `PlayedTime` int(4) unsigned NOT NULL,
  `CreateDate` datetime NOT NULL,
  `ExchangeDate` datetime NOT NULL,
  `LastPlayingDate` datetime NOT NULL,
  `LastPlayingIP` int(4) unsigned NOT NULL,
  `NoChatIn` int(4) unsigned NOT NULL,
  `NoLoginIn` int(4) unsigned NOT NULL,
  `UseRevivePosition` int(1) unsigned NOT NULL,
  `ReviveID` int(4) NOT NULL,
  `ReviveX` int(4) NOT NULL,
  `ReviveY` int(4) NOT NULL,
  `EnterMapID` int(4) NOT NULL,
  `BaseInfo` tinyblob,
  `NumericInfo` tinyblob,
  `PasswordInfo` tinyblob,
  `TongWarInfo` tinyblob,
  `PetInfo` tinyblob,
  `OfflinePlayingInfo` tinyblob,
  `CompensatoryInfo` tinyblob,
  `OtherInfo` tinyblob,
  `ReserveInfo` tinyblob,
  `SettingInfo` tinyblob,
  `SkillList` blob,
  `ItemList` blob,
  `TaskList` blob,
  `EnhanceList` blob,
  `FriendsList` blob,
  `ReserveList` blob,
  `GUID` varbinary(64) NOT NULL,
  `Portrait` int(4) NOT NULL,
  `Online` int(4) NOT NULL,
  `SpyLevel` int(4) NOT NULL,
  `SkillSeries` int(4) NOT NULL,
  `GlobalMailDate` datetime NOT NULL,
  PRIMARY KEY  (`Id`),
  KEY `RoleIndex` (`RoleName`),
  KEY `AccountIndex` (`AccountName`),
  KEY `LastPlayingIndex` (`LastPlayingDate`),
  KEY `GUIDIndex` (`GUID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;


#
# Structure for the `rolesfirst` table :
#

DROP TABLE IF EXISTS `rolesfirst`;

CREATE TABLE `rolesfirst` (
  `Id` int(4) unsigned NOT NULL auto_increment,
  `Version` int(4) unsigned NOT NULL,
  `TongGUID` varbinary(64) NOT NULL,
  `AccountName` varbinary(64) NOT NULL,
  `RoleName` varbinary(64) NOT NULL,
  `RoleSex` int(1) unsigned NOT NULL,
  `RoleType` int(1) unsigned NOT NULL,
  `RoleLevel` int(1) unsigned NOT NULL,
  `Money` int(4) unsigned NOT NULL,
  `MoneyInBox` int(4) unsigned NOT NULL,
  `PlayedTime` int(4) unsigned NOT NULL,
  `CreateDate` datetime NOT NULL,
  `ExchangeDate` datetime NOT NULL,
  `LastPlayingDate` datetime NOT NULL,
  `LastPlayingIP` int(4) unsigned NOT NULL,
  `NoChatIn` int(4) unsigned NOT NULL,
  `NoLoginIn` int(4) unsigned NOT NULL,
  `UseRevivePosition` int(1) unsigned NOT NULL,
  `ReviveID` int(4) NOT NULL,
  `ReviveX` int(4) NOT NULL,
  `ReviveY` int(4) NOT NULL,
  `EnterMapID` int(4) NOT NULL,
  `BaseInfo` tinyblob,
  `NumericInfo` tinyblob,
  `PasswordInfo` tinyblob,
  `TongWarInfo` tinyblob,
  `PetInfo` tinyblob,
  `OfflinePlayingInfo` tinyblob,
  `CompensatoryInfo` tinyblob,
  `OtherInfo` tinyblob,
  `ReserveInfo` tinyblob,
  `SettingInfo` tinyblob,
  `SkillList` blob,
  `ItemList` blob,
  `TaskList` blob,
  `EnhanceList` blob,
  `FriendsList` blob,
  `ReserveList` blob,
  `GUID` varbinary(64) NOT NULL,
  `Portrait` int(4) NOT NULL,
  `Online` int(4) NOT NULL,
  `SpyLevel` int(4) NOT NULL,
  `SkillSeries` int(4) NOT NULL,
  `GlobalMailDate` datetime NOT NULL,
  PRIMARY KEY  (`Id`),
  UNIQUE KEY `RoleIndex` (`RoleName`),
  KEY `AccountIndex` (`AccountName`),
  KEY `LastPlayingIndex` (`LastPlayingDate`),
  KEY `GUIDIndex` (`GUID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;


#
# Structure for the `rolessecond` table :
#

DROP TABLE IF EXISTS `rolessecond`;

CREATE TABLE `rolessecond` (
  `Id` int(4) unsigned NOT NULL,
  `Version` int(4) unsigned NOT NULL,
  `TongGUID` varbinary(64) NOT NULL,
  `AccountName` varbinary(64) NOT NULL,
  `RoleName` varbinary(64) NOT NULL,
  `RoleSex` int(1) unsigned NOT NULL,
  `RoleType` int(1) unsigned NOT NULL,
  `RoleLevel` int(1) unsigned NOT NULL,
  `Money` int(4) unsigned NOT NULL,
  `MoneyInBox` int(4) unsigned NOT NULL,
  `PlayedTime` int(4) unsigned NOT NULL,
  `CreateDate` datetime NOT NULL,
  `ExchangeDate` datetime NOT NULL,
  `LastPlayingDate` datetime NOT NULL,
  `LastPlayingIP` int(4) unsigned NOT NULL,
  `NoChatIn` int(4) unsigned NOT NULL,
  `NoLoginIn` int(4) unsigned NOT NULL,
  `UseRevivePosition` int(1) unsigned NOT NULL,
  `ReviveID` int(4) NOT NULL,
  `ReviveX` int(4) NOT NULL,
  `ReviveY` int(4) NOT NULL,
  `EnterMapID` int(4) NOT NULL,
  `BaseInfo` tinyblob,
  `NumericInfo` tinyblob,
  `PasswordInfo` tinyblob,
  `TongWarInfo` tinyblob,
  `PetInfo` tinyblob,
  `OfflinePlayingInfo` tinyblob,
  `CompensatoryInfo` tinyblob,
  `OtherInfo` tinyblob,
  `ReserveInfo` tinyblob,
  `SettingInfo` tinyblob,
  `SkillList` blob,
  `ItemList` blob,
  `TaskList` blob,
  `EnhanceList` blob,
  `FriendsList` blob,
  `ReserveList` blob,
  `GUID` varbinary(64) NOT NULL,
  `Portrait` int(4) NOT NULL,
  `Online` int(4) NOT NULL,
  `SpyLevel` int(4) NOT NULL,
  `SkillSeries` int(4) NOT NULL,
  `GlobalMailDate` datetime NOT NULL,
  PRIMARY KEY  (`Id`),
  UNIQUE KEY `RoleIndex` (`RoleName`),
  KEY `AccountIndex` (`AccountName`),
  KEY `LastPlayingIndex` (`LastPlayingDate`),
  KEY `GUIDIndex` (`GUID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Structure for the `socialrelation` table :
#

DROP TABLE IF EXISTS `socialrelation`;

CREATE TABLE `socialrelation` (
  `NodeGUID` varbinary(64) NOT NULL,
  `ParentGUID` varbinary(64) default NULL,
  `TemplateId` int(4) unsigned NOT NULL,
  `LayerId` int(4) unsigned NOT NULL,
  `NodeName` varbinary(64) NOT NULL,
  `LeaderName` varbinary(64) NOT NULL,
  `ChildCount` int(4) unsigned NOT NULL,
  `AttributeData` blob,
  `PlusData` blob,
  PRIMARY KEY  (`NodeGUID`),
  KEY `ParentGUIDIndex` (`ParentGUID`),
  KEY `NodeNameIndex` (`NodeName`),
  KEY `LeaderNameIndex` (`LeaderName`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Structure for the `systemlog` table :
#

DROP TABLE IF EXISTS `systemlog`;

CREATE TABLE `systemlog` (
  `Id` int(6) unsigned NOT NULL auto_increment,
  `LogTime` datetime NOT NULL,
  `LogEvent` int(4) NOT NULL default '0',
  `LogData` blob,
  PRIMARY KEY  (`Id`),
  KEY `EventIndex` (`LogEvent`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Structure for the `tong` table :
#

DROP TABLE IF EXISTS `tong`;

CREATE TABLE `tong` (
  `ID` int(11) unsigned NOT NULL auto_increment,
  `LogTime` datetime default NULL,
  `Source` tinyint(4) default NULL,
  `IP` varchar(128) default NULL,
  `Level` tinyint(4) default NULL,
  `Field1` varchar(128) default NULL COMMENT '帮会ID',
  `Field2` varchar(128) default NULL COMMENT '帮会名',
  PRIMARY KEY  (`ID`),
  UNIQUE KEY `ID` (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;

#
# Structure for the `trolebaseinfo` table :
#

DROP TABLE IF EXISTS `trolebaseinfo`;

CREATE TABLE `trolebaseinfo` (
  `id` int(11) NOT NULL auto_increment,
  `cRoleId` varchar(128) default NULL COMMENT '角色GUID',
  `cAccountName` char(255) NOT NULL,
  `cRoleName` varchar(256) NOT NULL,
  `iSex` int(11) NOT NULL default '0',
  `iSeries` int(11) NOT NULL default '0',
  `iLevel` mediumint(9) NOT NULL default '0',
  `iCamp` int(11) NOT NULL default '0',
  `iPkValue` int(11) NOT NULL default '0',
  `iExp` bigint(20) NOT NULL default '0',
  `iLife` int(11) NOT NULL default '0',
  `iMana` int(11) NOT NULL default '0',
  `iStamina` int(11) NOT NULL default '0',
  `iGatherPoint` int(11) NOT NULL default '0',
  `iMakePoint` int(11) NOT NULL default '0',
  `iRemainPotential` mediumint(9) NOT NULL default '0',
  `iStrength` mediumint(9) NOT NULL default '0',
  `iDexterity` mediumint(9) NOT NULL default '0',
  `iVitality` mediumint(9) NOT NULL default '0',
  `iEnergy` mediumint(9) NOT NULL default '0',
  `iSkillPoint` mediumint(9) NOT NULL default '0',
  `dCreateTime` datetime NOT NULL,
  `dLastLoginTime` datetime NOT NULL,
  `iGameTime` int(11) NOT NULL,
  `cLastLoginIP` char(15) NOT NULL default '0.0.0.0',
  `iFaction` int(11) default NULL,
  `iFactionRoute` int(11) default NULL,
  `iMoney` bigint(20) default NULL,
  `iTongID` int(11) default NULL,
  `SERVER_TYPE` int(11) default NULL,
  `ConfigFileName` varchar(3500) default NULL,
  PRIMARY KEY  (`id`),
  UNIQUE KEY `id` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312 COMMENT='InnoDB free: 4096 kB';



