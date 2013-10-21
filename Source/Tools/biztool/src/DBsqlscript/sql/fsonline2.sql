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
# Structure for the `log_101_10` table : 
#

DROP TABLE IF EXISTS `log_101_10`;

CREATE TABLE `log_101_10` (
  `ID` int(10) unsigned NOT NULL auto_increment,
  `LogTime` datetime default NULL,
  `Source` tinyint(3) unsigned default NULL,
  `Ip` varchar(128) default NULL,
  `Level` tinyint(3) unsigned default NULL,
  `Field1` blob,
  `Field2` bigint(20) default NULL,
  `Field3` int(11) default NULL,
  `Field4` bigint(20) default NULL,
  `Field5` text,
  `Field6` varchar(128) default NULL,
  UNIQUE KEY `ID` (`ID`),
  KEY `LogTime` (`LogTime`),
  KEY `Field1` (`Field1`(255)),
  KEY `Field2` (`Field2`,`Field3`,`Field4`,`Field5`(255),`Field6`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;

#
# Structure for the `log_10_1` table : 
#

DROP TABLE IF EXISTS `log_10_1`;

CREATE TABLE `log_10_1` (
  `ID` int(10) unsigned NOT NULL auto_increment,
  `LogTime` datetime default NULL,
  `Source` tinyint(3) unsigned default NULL,
  `Ip` varchar(128) default NULL,
  `Level` tinyint(3) unsigned default NULL,
  `Field1` varchar(128) default NULL,
  `Field2` int(10) unsigned default NULL,
  `Field3` int(10) unsigned default NULL,
  `Field4` tinyint(3) unsigned default NULL,
  `Field5` int(10) unsigned default NULL,
  `Field6` tinyint(3) unsigned default NULL,
  UNIQUE KEY `ID` (`ID`),
  KEY `LogTime` (`LogTime`),
  KEY `Field1` (`Field1`),
  KEY `Field5` (`Field5`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;

#
# Structure for the `log_10_10` table : 
#

DROP TABLE IF EXISTS `log_10_10`;

CREATE TABLE `log_10_10` (
  `ID` int(10) unsigned NOT NULL auto_increment,
  `LogTime` datetime default NULL,
  `Source` tinyint(3) unsigned default NULL,
  `Ip` varchar(128) default NULL,
  `Level` tinyint(3) unsigned default NULL,
  `Field1` int(10) unsigned default NULL,
  `Field2` int(10) unsigned default NULL,
  `Field3` int(10) unsigned default NULL,
  `Field4` int(10) unsigned default NULL,
  `Field5` varchar(128) default NULL,
  `Field6` varchar(128) default NULL,
  UNIQUE KEY `ID` (`ID`),
  KEY `LogTime` (`LogTime`),
  KEY `Field1` (`Field1`),
  KEY `Field2` (`Field2`),
  KEY `Field5` (`Field5`),
  KEY `Field6` (`Field6`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;

#
# Structure for the `log_10_11` table : 
#

DROP TABLE IF EXISTS `log_10_11`;

CREATE TABLE `log_10_11` (
  `ID` int(10) unsigned NOT NULL auto_increment,
  `LogTime` datetime default NULL,
  `Source` tinyint(3) unsigned default NULL,
  `Ip` varchar(128) default NULL,
  `Level` tinyint(3) unsigned default NULL,
  `Field1` int(10) unsigned default NULL,
  `Field2` int(10) unsigned default NULL,
  `Field3` int(10) unsigned default NULL,
  `Field4` varchar(128) default NULL,
  `Field5` varchar(128) default NULL,
  UNIQUE KEY `ID` (`ID`),
  KEY `LogTime` (`LogTime`),
  KEY `Field1` (`Field1`),
  KEY `Field2` (`Field2`),
  KEY `Field4` (`Field4`),
  KEY `Field5` (`Field5`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;

#
# Structure for the `log_10_12` table : 
#

DROP TABLE IF EXISTS `log_10_12`;

CREATE TABLE `log_10_12` (
  `ID` int(10) unsigned NOT NULL auto_increment,
  `LogTime` datetime default NULL,
  `Source` tinyint(3) unsigned default NULL,
  `Ip` varchar(128) default NULL,
  `Level` tinyint(3) unsigned default NULL,
  `Field1` int(10) unsigned default NULL,
  `Field2` int(10) unsigned default NULL,
  UNIQUE KEY `ID` (`ID`),
  KEY `LogTime` (`LogTime`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;

#
# Structure for the `log_10_2` table : 
#

DROP TABLE IF EXISTS `log_10_2`;

CREATE TABLE `log_10_2` (
  `ID` int(10) unsigned NOT NULL auto_increment,
  `LogTime` datetime default NULL,
  `Source` tinyint(3) unsigned default NULL,
  `Ip` varchar(128) default NULL,
  `Level` tinyint(3) unsigned default NULL,
  `Field1` varchar(128) default NULL,
  `Field2` int(10) unsigned default NULL,
  `Field3` int(10) unsigned default NULL,
  `Field4` tinyint(3) unsigned default NULL,
  `Field5` int(10) unsigned default NULL,
  UNIQUE KEY `ID` (`ID`),
  KEY `LogTime` (`LogTime`),
  KEY `Field1` (`Field1`),
  KEY `Field5` (`Field5`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;

#
# Structure for the `log_10_3` table : 
#

DROP TABLE IF EXISTS `log_10_3`;

CREATE TABLE `log_10_3` (
  `ID` int(10) unsigned NOT NULL auto_increment,
  `LogTime` datetime default NULL,
  `Source` tinyint(3) unsigned default NULL,
  `Ip` varchar(128) default NULL,
  `Level` tinyint(3) unsigned default NULL,
  `Field1` int(10) unsigned default NULL,
  `Field2` tinyint(3) unsigned default NULL,
  UNIQUE KEY `ID` (`ID`),
  KEY `LogTime` (`LogTime`),
  KEY `Field1` (`Field1`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;

#
# Structure for the `log_10_4` table : 
#

DROP TABLE IF EXISTS `log_10_4`;

CREATE TABLE `log_10_4` (
  `ID` int(10) unsigned NOT NULL auto_increment,
  `LogTime` datetime default NULL,
  `Source` tinyint(3) unsigned default NULL,
  `Ip` varchar(128) default NULL,
  `Level` tinyint(3) unsigned default NULL,
  `Field1` varchar(128) default NULL,
  `Field2` int(10) unsigned default NULL,
  `Field3` int(10) unsigned default NULL,
  UNIQUE KEY `ID` (`ID`),
  KEY `LogTime` (`LogTime`),
  KEY `Field1` (`Field1`),
  KEY `Field3` (`Field3`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;

#
# Structure for the `log_10_5` table : 
#

DROP TABLE IF EXISTS `log_10_5`;

CREATE TABLE `log_10_5` (
  `ID` int(10) unsigned NOT NULL auto_increment,
  `LogTime` datetime default NULL,
  `Source` tinyint(3) unsigned default NULL,
  `Ip` varchar(128) default NULL,
  `Level` tinyint(3) unsigned default NULL,
  `Field1` int(10) unsigned default NULL,
  `Field2` int(10) unsigned default NULL,
  `Field3` varchar(128) default NULL,
  UNIQUE KEY `ID` (`ID`),
  KEY `LogTime` (`LogTime`),
  KEY `Field1` (`Field1`),
  KEY `Field3` (`Field3`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;

#
# Structure for the `log_10_6` table : 
#

DROP TABLE IF EXISTS `log_10_6`;

CREATE TABLE `log_10_6` (
  `ID` int(10) unsigned NOT NULL auto_increment,
  `LogTime` datetime default NULL,
  `Source` tinyint(3) unsigned default NULL,
  `Ip` varchar(128) default NULL,
  `Level` tinyint(3) unsigned default NULL,
  `Field1` int(10) unsigned default NULL,
  `Field2` tinyint(3) unsigned default NULL,
  UNIQUE KEY `ID` (`ID`),
  KEY `LogTime` (`LogTime`),
  KEY `Field1` (`Field1`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;

#
# Structure for the `log_10_7` table : 
#

DROP TABLE IF EXISTS `log_10_7`;

CREATE TABLE `log_10_7` (
  `ID` int(10) unsigned NOT NULL auto_increment,
  `LogTime` datetime default NULL,
  `Source` tinyint(3) unsigned default NULL,
  `Ip` varchar(128) default NULL,
  `Level` tinyint(3) unsigned default NULL,
  `Field1` int(10) unsigned default NULL,
  `Field2` int(10) unsigned default NULL,
  `Field3` varchar(128) default NULL,
  UNIQUE KEY `ID` (`ID`),
  KEY `LogTime` (`LogTime`),
  KEY `Field1` (`Field1`),
  KEY `Field3` (`Field3`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;

#
# Structure for the `log_10_8` table : 
#

DROP TABLE IF EXISTS `log_10_8`;

CREATE TABLE `log_10_8` (
  `ID` int(10) unsigned NOT NULL auto_increment,
  `LogTime` datetime default NULL,
  `Source` tinyint(3) unsigned default NULL,
  `Ip` varchar(128) default NULL,
  `Level` tinyint(3) unsigned default NULL,
  `Field1` int(10) unsigned default NULL,
  `Field2` tinyint(3) unsigned default NULL,
  UNIQUE KEY `ID` (`ID`),
  KEY `LogTime` (`LogTime`),
  KEY `Field1` (`Field1`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;

#
# Structure for the `log_10_9` table : 
#

DROP TABLE IF EXISTS `log_10_9`;

CREATE TABLE `log_10_9` (
  `ID` int(10) unsigned NOT NULL auto_increment,
  `LogTime` datetime default NULL,
  `Source` tinyint(3) unsigned default NULL,
  `Ip` varchar(128) default NULL,
  `Level` tinyint(3) unsigned default NULL,
  `Field1` int(10) unsigned default NULL,
  `Field2` int(10) unsigned default NULL,
  `Field3` int(10) unsigned default NULL,
  `Field4` varchar(128) default NULL,
  `Field5` varchar(128) default NULL,
  UNIQUE KEY `ID` (`ID`),
  KEY `LogTime` (`LogTime`),
  KEY `Field1` (`Field1`),
  KEY `Field2` (`Field2`),
  KEY `Field4` (`Field4`),
  KEY `Field5` (`Field5`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;

#
# Structure for the `log_120_20` table : 
#

DROP TABLE IF EXISTS `log_120_20`;

CREATE TABLE `log_120_20` (
  `ID` int(10) unsigned NOT NULL auto_increment,
  `LogTime` datetime default NULL,
  `Source` tinyint(3) unsigned default NULL,
  `Ip` varchar(128) default NULL,
  `Level` tinyint(3) unsigned default NULL,
  `Field1` bigint(20) default NULL,
  `Field2` int(11) default NULL,
  `Field3` bigint(20) default NULL,
  `Field4` text,
  UNIQUE KEY `ID` (`ID`),
  KEY `LogTime` (`LogTime`),
  KEY `Field1` (`Field1`,`Field2`,`Field3`,`Field4`(255))
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;

#
# Structure for the `log_121_20` table : 
#

DROP TABLE IF EXISTS `log_121_20`;

CREATE TABLE `log_121_20` (
  `ID` int(10) unsigned NOT NULL auto_increment,
  `LogTime` datetime default NULL,
  `Source` tinyint(3) unsigned default NULL,
  `Ip` varchar(128) default NULL,
  `Level` tinyint(3) unsigned default NULL,
  `Field1` bigint(20) default NULL,
  `Field2` int(11) default NULL,
  `Field3` bigint(20) default NULL,
  `Field4` text,
  UNIQUE KEY `ID` (`ID`),
  KEY `LogTime` (`LogTime`),
  KEY `Field1` (`Field1`,`Field2`,`Field3`,`Field4`(255))
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;

#
# Structure for the `log_12_20` table : 
#

DROP TABLE IF EXISTS `log_12_20`;

CREATE TABLE `log_12_20` (
  `ID` int(10) unsigned NOT NULL auto_increment,
  `LogTime` datetime default NULL,
  `Source` tinyint(3) unsigned default NULL,
  `Ip` varchar(128) default NULL,
  `Level` tinyint(3) unsigned default NULL,
  `Field1` blob,
  `Field2` bigint(20) default NULL,
  `Field3` int(11) default NULL,
  `Field4` bigint(20) default NULL,
  `Field5` text,
  UNIQUE KEY `ID` (`ID`),
  KEY `LogTime` (`LogTime`),
  KEY `Field1` (`Field1`(255)),
  KEY `Field2` (`Field2`,`Field3`,`Field4`,`Field5`(255))
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;

#
# Structure for the `log_3_1` table : 
#

DROP TABLE IF EXISTS `log_3_1`;

CREATE TABLE `log_3_1` (
  `ID` int(10) unsigned NOT NULL auto_increment,
  `LogTime` datetime default NULL,
  `Source` tinyint(3) unsigned default NULL,
  `Ip` varchar(128) default NULL,
  `Level` tinyint(3) unsigned default NULL,
  `Field1` varchar(128) default NULL,
  `Field2` int(11) default NULL,
  UNIQUE KEY `ID` (`ID`),
  KEY `LogTime` (`LogTime`),
  KEY `Field1` (`Field1`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;

#
# Structure for the `log_3_2` table : 
#

DROP TABLE IF EXISTS `log_3_2`;

CREATE TABLE `log_3_2` (
  `ID` int(10) unsigned NOT NULL auto_increment,
  `LogTime` datetime default NULL,
  `Source` tinyint(3) unsigned default NULL,
  `Ip` varchar(128) default NULL,
  `Level` tinyint(3) unsigned default NULL,
  `Field1` varchar(128) default NULL,
  `Field2` int(11) default NULL,
  `Field3` int(11) default NULL,
  UNIQUE KEY `ID` (`ID`),
  KEY `LogTime` (`LogTime`),
  KEY `Field1` (`Field1`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;

#
# Structure for the `log_3_3` table : 
#

DROP TABLE IF EXISTS `log_3_3`;

CREATE TABLE `log_3_3` (
  `ID` int(10) unsigned NOT NULL auto_increment,
  `LogTime` datetime default NULL,
  `Source` tinyint(3) unsigned default NULL,
  `Ip` varchar(128) default NULL,
  `Level` tinyint(3) unsigned default NULL,
  `Field1` varchar(128) default NULL,
  `Field2` int(11) default NULL,
  `Field3` int(11) default NULL,
  `Field4` varchar(3) default NULL,
  UNIQUE KEY `ID` (`ID`),
  KEY `LogTime` (`LogTime`),
  KEY `Field1` (`Field1`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;

#
# Structure for the `log_3_4` table : 
#

DROP TABLE IF EXISTS `log_3_4`;

CREATE TABLE `log_3_4` (
  `ID` int(10) unsigned NOT NULL auto_increment,
  `LogTime` datetime default NULL,
  `Source` tinyint(3) unsigned default NULL,
  `Ip` varchar(128) default NULL,
  `Level` tinyint(3) unsigned default NULL,
  `Field1` varchar(128) default NULL,
  `Field2` int(11) default NULL,
  `Field3` int(10) unsigned default NULL,
  UNIQUE KEY `ID` (`ID`),
  KEY `LogTime` (`LogTime`),
  KEY `Field1` (`Field1`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;

#
# Structure for the `log_3_5` table : 
#

DROP TABLE IF EXISTS `log_3_5`;

CREATE TABLE `log_3_5` (
  `ID` int(10) unsigned NOT NULL auto_increment,
  `LogTime` datetime default NULL,
  `Source` tinyint(3) unsigned default NULL,
  `Ip` varchar(128) default NULL,
  `Level` tinyint(3) unsigned default NULL,
  `Field1` varchar(128) default NULL,
  `Field2` int(11) default NULL,
  `Field3` int(10) unsigned default NULL,
  UNIQUE KEY `ID` (`ID`),
  KEY `LogTime` (`LogTime`),
  KEY `Field1` (`Field1`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;

#
# Structure for the `log_3_6` table : 
#

DROP TABLE IF EXISTS `log_3_6`;

CREATE TABLE `log_3_6` (
  `ID` int(10) unsigned NOT NULL auto_increment,
  `LogTime` datetime default NULL,
  `Source` tinyint(3) unsigned default NULL,
  `Ip` varchar(128) default NULL,
  `Level` tinyint(3) unsigned default NULL,
  `Field1` varchar(128) default NULL,
  `Field2` int(11) default NULL,
  `Field3` int(11) default NULL,
  `Field4` varchar(128) default NULL,
  `Field5` varchar(128) default NULL,
  UNIQUE KEY `ID` (`ID`),
  KEY `LogTime` (`LogTime`),
  KEY `Field1` (`Field1`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;

#
# Structure for the `log_3_7` table : 
#

DROP TABLE IF EXISTS `log_3_7`;

CREATE TABLE `log_3_7` (
  `ID` int(10) unsigned NOT NULL auto_increment,
  `LogTime` datetime default NULL,
  `Source` tinyint(3) unsigned default NULL,
  `Ip` varchar(128) default NULL,
  `Level` tinyint(3) unsigned default NULL,
  `Field1` varchar(128) default NULL,
  `Field2` tinyint(3) unsigned default NULL,
  `Field3` varchar(128) default NULL,
  UNIQUE KEY `ID` (`ID`),
  KEY `LogTime` (`LogTime`),
  KEY `Field1` (`Field1`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;

#
# Structure for the `log_4_1` table : 
#

DROP TABLE IF EXISTS `log_4_1`;

CREATE TABLE `log_4_1` (
  `ID` int(10) unsigned NOT NULL auto_increment,
  `LogTime` datetime default NULL,
  `Source` tinyint(3) unsigned default NULL,
  `Ip` varchar(128) default NULL,
  `Level` tinyint(3) unsigned default NULL,
  `Field1` varchar(128) default NULL,
  `Field2` varchar(128) default NULL,
  `Field3` int(10) unsigned default NULL,
  `Field4` int(11) default NULL,
  `Field5` int(11) default NULL,
  `Field6` int(11) default NULL,
  `Field7` int(11) default NULL,
  `Field8` tinyint(3) unsigned default NULL,
  `Field9` datetime default NULL,
  `Field10` int(10) unsigned default NULL,
  `Field11` int(11) default NULL,
  `Field12` int(11) default NULL,
  `Field13` int(11) default NULL,
  `Field14` int(10) unsigned default NULL,
  `Field15` tinyint(3) unsigned default NULL,
  `Field16` datetime default NULL,
  `Field17` int(11) default NULL,
  `Field18` int(10) unsigned default NULL,
  `Field19` int(10) unsigned default NULL,
  `Field20` int(10) unsigned default NULL,
  `Field21` int(10) unsigned default NULL,
  `Field22` int(10) unsigned default NULL,
  `Field23` int(10) unsigned default NULL,
  `Field24` int(10) unsigned default NULL,
  `Field25` int(10) unsigned default NULL,
  `Field26` int(10) unsigned default NULL,
  `Field27` int(10) unsigned default NULL,
  `Field28` int(10) unsigned default NULL,
  `Field29` int(10) unsigned default NULL,
  `Field30` varchar(128) default NULL,
  `Field31` text,
  UNIQUE KEY `ID` (`ID`),
  KEY `LogTime` (`LogTime`),
  KEY `Field1` (`Field1`),
  KEY `Field2` (`Field2`),
  KEY `Field9` (`Field9`),
  KEY `Field4` (`Field4`,`Field5`,`Field6`,`Field7`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;

#
# Structure for the `log_4_2` table : 
#

DROP TABLE IF EXISTS `log_4_2`;

CREATE TABLE `log_4_2` (
  `ID` int(10) unsigned NOT NULL auto_increment,
  `LogTime` datetime default NULL,
  `Source` tinyint(3) unsigned default NULL,
  `Ip` varchar(128) default NULL,
  `Level` tinyint(3) unsigned default NULL,
  `Field1` varchar(128) default NULL,
  `Field2` varchar(128) default NULL,
  `Field3` tinyint(3) unsigned default NULL,
  `Field4` int(11) default NULL,
  `Field5` int(11) default NULL,
  `Field6` int(11) default NULL,
  UNIQUE KEY `ID` (`ID`),
  KEY `LogTime` (`LogTime`),
  KEY `Field1` (`Field1`),
  KEY `Field2` (`Field2`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;

#
# Structure for the `log_4_3` table : 
#

DROP TABLE IF EXISTS `log_4_3`;

CREATE TABLE `log_4_3` (
  `ID` int(10) unsigned NOT NULL auto_increment,
  `LogTime` datetime default NULL,
  `Source` tinyint(3) unsigned default NULL,
  `Ip` varchar(128) default NULL,
  `Level` tinyint(3) unsigned default NULL,
  `Field1` varchar(128) default NULL,
  `Field2` varchar(128) default NULL,
  `Field3` tinyint(3) unsigned default NULL,
  `Field4` int(11) default NULL,
  `Field5` int(11) default NULL,
  `Field6` int(11) default NULL,
  UNIQUE KEY `ID` (`ID`),
  KEY `LogTime` (`LogTime`),
  KEY `Field1` (`Field1`),
  KEY `Field2` (`Field2`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;

#
# Structure for the `log_4_4` table : 
#

DROP TABLE IF EXISTS `log_4_4`;

CREATE TABLE `log_4_4` (
  `ID` int(10) unsigned NOT NULL auto_increment,
  `LogTime` datetime default NULL,
  `Source` tinyint(3) unsigned default NULL,
  `Ip` varchar(128) default NULL,
  `Level` tinyint(3) unsigned default NULL,
  `Field1` varchar(128) default NULL,
  `Field2` varchar(128) default NULL,
  `Field3` tinyint(3) unsigned default NULL,
  `Field4` int(11) default NULL,
  `Field5` int(11) default NULL,
  `Field6` int(11) default NULL,
  UNIQUE KEY `ID` (`ID`),
  KEY `LogTime` (`LogTime`),
  KEY `Field1` (`Field1`),
  KEY `Field2` (`Field2`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;

#
# Structure for the `log_4_5` table : 
#

DROP TABLE IF EXISTS `log_4_5`;

CREATE TABLE `log_4_5` (
  `ID` int(10) unsigned NOT NULL auto_increment,
  `LogTime` datetime default NULL,
  `Source` tinyint(3) unsigned default NULL,
  `Ip` varchar(128) default NULL,
  `Level` tinyint(3) unsigned default NULL,
  `Field1` varchar(128) default NULL,
  `Field2` varchar(128) default NULL,
  `Field3` varchar(128) default NULL,
  `Field4` tinyint(3) unsigned default NULL,
  `Field5` int(11) default NULL,
  `Field6` int(11) default NULL,
  `Field7` int(11) default NULL,
  `Field8` int(11) default NULL,
  `Field9` int(11) default NULL,
  `Field10` int(11) default NULL,
  UNIQUE KEY `ID` (`ID`),
  KEY `LogTime` (`LogTime`),
  KEY `Field1` (`Field1`),
  KEY `Field2` (`Field2`),
  KEY `Field3` (`Field3`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;

#
# Structure for the `log_7_1` table : 
#

DROP TABLE IF EXISTS `log_7_1`;

CREATE TABLE `log_7_1` (
  `ID` int(10) unsigned NOT NULL auto_increment,
  `LogTime` datetime default NULL,
  `Source` tinyint(3) unsigned default NULL,
  `Ip` varchar(128) default NULL,
  `Level` tinyint(3) unsigned default NULL,
  `Field1` varchar(128) default NULL,
  `Field2` text,
  UNIQUE KEY `ID` (`ID`),
  KEY `LogTime` (`LogTime`),
  KEY `Field1` (`Field1`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;

#
# Structure for the `log_7_2` table : 
#

DROP TABLE IF EXISTS `log_7_2`;

CREATE TABLE `log_7_2` (
  `ID` int(10) unsigned NOT NULL auto_increment,
  `LogTime` datetime default NULL,
  `Source` tinyint(3) unsigned default NULL,
  `Ip` varchar(128) default NULL,
  `Level` tinyint(3) unsigned default NULL,
  `Field1` text,
  UNIQUE KEY `ID` (`ID`),
  KEY `LogTime` (`LogTime`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;

#
# Structure for the `log_8_1` table : 
#

DROP TABLE IF EXISTS `log_8_1`;

CREATE TABLE `log_8_1` (
  `ID` int(10) unsigned NOT NULL auto_increment,
  `LogTime` datetime default NULL,
  `Source` tinyint(3) unsigned default NULL,
  `Ip` varchar(128) default NULL,
  `Level` tinyint(3) unsigned default NULL,
  `Field1` int(11) default NULL,
  `Field2` varchar(128) default NULL,
  `Field3` int(11) default NULL,
  `Field4` int(11) default NULL,
  `Field5` int(11) default NULL,
  `Field6` varchar(128) default NULL,
  `Field7` int(11) default NULL,
  `Field8` int(11) default NULL,
  `Field9` int(11) default NULL,
  UNIQUE KEY `ID` (`ID`),
  KEY `LogTime` (`LogTime`),
  KEY `Field2` (`Field2`),
  KEY `Field6` (`Field6`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;

#
# Structure for the `log_8_2` table : 
#

DROP TABLE IF EXISTS `log_8_2`;

CREATE TABLE `log_8_2` (
  `ID` int(10) unsigned NOT NULL auto_increment,
  `LogTime` datetime default NULL,
  `Source` tinyint(3) unsigned default NULL,
  `Ip` varchar(128) default NULL,
  `Level` tinyint(3) unsigned default NULL,
  `Field1` int(11) default NULL,
  `Field2` varchar(128) default NULL,
  `Field3` varchar(128) default NULL,
  `Field4` varchar(128) default NULL,
  `Field5` varchar(128) default NULL,
  `Field6` int(11) default NULL,
  UNIQUE KEY `ID` (`ID`),
  KEY `LogTime` (`LogTime`),
  KEY `Field2` (`Field2`),
  KEY `Field3` (`Field3`),
  KEY `Field4` (`Field4`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;

#
# Structure for the `log_8_3` table : 
#

DROP TABLE IF EXISTS `log_8_3`;

CREATE TABLE `log_8_3` (
  `ID` int(10) unsigned NOT NULL auto_increment,
  `LogTime` datetime default NULL,
  `Source` tinyint(3) unsigned default NULL,
  `Ip` varchar(128) default NULL,
  `Level` tinyint(3) unsigned default NULL,
  `Field1` int(11) default NULL,
  `Field2` varchar(128) default NULL,
  `Field3` varchar(128) default NULL,
  `Field4` int(11) default NULL,
  UNIQUE KEY `ID` (`ID`),
  KEY `LogTime` (`LogTime`),
  KEY `Field2` (`Field2`),
  KEY `Field3` (`Field3`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;

#
# Structure for the `log_9_1` table : 
#

DROP TABLE IF EXISTS `log_9_1`;

CREATE TABLE `log_9_1` (
  `ID` int(10) unsigned NOT NULL auto_increment,
  `LogTime` datetime default NULL,
  `Source` tinyint(3) unsigned default NULL,
  `Ip` varchar(128) default NULL,
  `Level` tinyint(3) unsigned default NULL,
  `Field1` tinyint(3) unsigned default NULL,
  `Field2` int(11) default NULL,
  `Field3` varchar(128) default NULL,
  `Field4` int(11) default NULL,
  `Field5` int(11) default NULL,
  `Field6` int(11) default NULL,
  `Field7` varchar(128) default NULL,
  `Field8` int(11) default NULL,
  `Field9` int(11) default NULL,
  `Field10` int(11) default NULL,
  UNIQUE KEY `ID` (`ID`),
  KEY `LogTime` (`LogTime`),
  KEY `Field3` (`Field3`),
  KEY `Field7` (`Field7`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;

#
# Structure for the `log_9_2` table : 
#

DROP TABLE IF EXISTS `log_9_2`;

CREATE TABLE `log_9_2` (
  `ID` int(10) unsigned NOT NULL auto_increment,
  `LogTime` datetime default NULL,
  `Source` tinyint(3) unsigned default NULL,
  `Ip` varchar(128) default NULL,
  `Level` tinyint(3) unsigned default NULL,
  `Field1` tinyint(3) unsigned default NULL,
  `Field2` int(11) default NULL,
  `Field3` varchar(128) default NULL,
  `Field4` varchar(128) default NULL,
  `Field5` varchar(128) default NULL,
  `Field6` varchar(128) default NULL,
  `Field7` varchar(128) default NULL,
  `Field8` int(11) default NULL,
  UNIQUE KEY `ID` (`ID`),
  KEY `LogTime` (`LogTime`),
  KEY `Field3` (`Field3`),
  KEY `Field4` (`Field4`),
  KEY `Field6` (`Field6`),
  KEY `Field7` (`Field7`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;

#
# Structure for the `log_9_3` table : 
#

DROP TABLE IF EXISTS `log_9_3`;

CREATE TABLE `log_9_3` (
  `ID` int(10) unsigned NOT NULL auto_increment,
  `LogTime` datetime default NULL,
  `Source` tinyint(3) unsigned default NULL,
  `Ip` varchar(128) default NULL,
  `Level` tinyint(3) unsigned default NULL,
  `Field1` tinyint(3) unsigned default NULL,
  `Field2` int(11) default NULL,
  `Field3` varchar(128) default NULL,
  `Field4` varchar(128) default NULL,
  `Field5` int(11) default NULL,
  UNIQUE KEY `ID` (`ID`),
  KEY `LogTime` (`LogTime`),
  KEY `Field3` (`Field3`),
  KEY `Field4` (`Field4`)
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

CREATE TRIGGER `updaterolesdead` BEFORE INSERT ON `rolesdead`
  FOR EACH ROW
SET NEW.ExchangeDate = NOW();

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

CREATE TRIGGER `updaterolesdelete` BEFORE INSERT ON `rolesdelete`
  FOR EACH ROW
SET NEW.ExchangeDate = NOW();

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

CREATE TRIGGER `updaterolesfirst` BEFORE INSERT ON `rolesfirst`
  FOR EACH ROW
SET NEW.ExchangeDate = NOW();

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

CREATE TRIGGER `updaterolessecond` BEFORE INSERT ON `rolessecond`
  FOR EACH ROW
SET NEW.ExchangeDate = NOW();

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

#
# Definition for the `f` procedure : 
#

DROP PROCEDURE IF EXISTS `f`;

CREATE PROCEDURE `f`(OUT param1 INT)
    NOT DETERMINISTIC
    SQL SECURITY DEFINER
    COMMENT ''
BEGIN
SELECT croleName INTO param1
FROM trolebaseinfo;
END;

#
# Definition for the `new_proc` procedure : 
#

DROP PROCEDURE IF EXISTS `new_proc`;

CREATE PROCEDURE `new_proc`()
    NOT DETERMINISTIC
    SQL SECURITY DEFINER
    COMMENT ''
BEGIN
select logtime as time, '升级' as event, CONCAT('升到<', CAST(field2 as char(8)),'>级') as remark
from log_3_1
where field1='asdasdasd-7e91dasdasdo823'
union
select logtime as time, '加入门派' as event, CONCAT('加入<', (select field2 from faction where log_3_2.field3=faction.field1), '>，当时为<', CAST(field2 as char(8)), '>级')   as remark
from log_3_2
where field1='asdasdasd-7e91dasdasdo823'
union
select logtime as time, '出师' as event, CONCAT('出师自<', (select field2 from faction where log_3_3.field3=faction.field1), '>，当时为<', CAST(field2 as char(8)), '>级')   as remark
from log_3_3
where field1='asdasdasd-7e91dasdasdo823'
union
select logtime as time, '加入家族' as event, CONCAT('加入<', (select field2 from kin where log_3_4.field3=kin.field1), '>，当时为<', CAST(field2 as char(8)), '>级')   as remark
from log_3_4
where field1='asdasdasd-7e91dasdasdo823'
union
select logtime as time, '退出家族' as event, CONCAT('退出<', (select field2 from kin where log_3_5.field3=kin.field1), '>，当时为<', CAST(field2 as char(8)), '>级')   as remark
from log_3_5
where field1='asdasdasd-7e91dasdasdo823';
END;

#
# Definition for the `sp_itemquery` procedure : 
#

DROP PROCEDURE IF EXISTS `sp_itemquery`;

CREATE PROCEDURE `sp_itemquery`(IN begintime DATETIME, IN endtime DATETIME, IN goodsName VARCHAR(48), IN roleid VARCHAR(128))
    NOT DETERMINISTIC
    SQL SECURITY DEFINER
    COMMENT ''
BEGIN
select Field1
from log_4_1;
END;

#
# Data for the `faction` table  (LIMIT 0,500)
#

INSERT INTO `faction` (`Field1`, `Field2`, `Field3`) VALUES 
  ('0','无门派','0'),
  ('1','少林','0'),
  ('1','刀少林','1'),
  ('1','棍少林','2'),
  ('2','天王','0'),
  ('2','枪天王','1'),
  ('2','锤天王','2'),
  ('3','唐门','0'),
  ('3','陷阱唐门','1'),
  ('3','袖箭唐门','2'),
  ('4','五毒','0'),
  ('4','刀五毒','1'),
  ('4','掌五毒','2'),
  ('5','峨嵋','0'),
  ('5','辅助峨嵋','2'),
  ('5','掌峨嵋','1'),
  ('6','翠烟','0'),
  ('6','剑翠烟','1'),
  ('6','刀翠烟','2'),
  ('7','丐帮','0'),
  ('7','掌丐帮','1'),
  ('7','棍丐帮','2'),
  ('8','天忍','0'),
  ('8','战天忍','1'),
  ('8','魔天忍','2'),
  ('9','武当','0'),
  ('9','气武当','1'),
  ('9','剑武当','2'),
  ('10','昆仑','0'),
  ('10','刀昆仑','1'),
  ('10','剑昆仑','2'),
  ('11','明教','0'),
  ('11','锤明教','1'),
  ('11','剑明教','2'),
  ('12','大理段氏','0'),
  ('12','指段氏','1'),
  ('12','气段氏','2');

COMMIT;

#
# Data for the `itemlist` table  (LIMIT 0,500)
#

INSERT INTO `itemlist` (`ItemId`, `CreateTime`, `RoleId`, `Genre`, `DetailType`, `ParticularType`, `Level`, `Series`, `Seed`, `StackCount`, `StateFlag`, `UnlockTime`, `PlaceRoom`, `PlaceX`, `PlaceY`, `CustomType`, `Custom`, `CurDur`, `MaxDur`, `Luck`, `Enhance`, `Param1`, `Param2`, `Param3`, `Param4`, `Param5`, `Param6`, `Param7`, `Param8`, `Param9`, `Param10`, `Param11`, `Param12`, `TimeoutType`, `TimeoutValue`) VALUES 
  ('{01000000D780A3C800142AA7AB810000}','2008-03-26 18:31:15','{0A000000BE768BD300142AA7AB810000}',17,3,1,5,0,1690655504,1,0,0,2,3,0,0,'',1000,1000,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{11000000E497D15500142AA7AB810000}','2008-03-26 18:22:21','{0A000000BE768BD300142AA7AB810000}',17,4,1,4,0,4283755463,1,0,0,2,2,0,0,'',1000,1000,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{10000000F508873600142AA7AB810000}','2008-03-26 18:22:19','{0A000000BE768BD300142AA7AB810000}',17,4,1,2,0,4154272922,1,0,0,2,1,0,0,'',1000,1000,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{0F0000009AD44CE500142AA7AB810000}','2008-03-26 18:22:17','{0A000000BE768BD300142AA7AB810000}',17,3,1,2,0,3314521905,1,0,0,2,0,0,0,'',1000,1000,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{11000000BC89956C00142AA7AB810000}','2008-03-26 21:20:45','{010000007CC0C17B00142AA7AB810000}',17,1,1,3,0,12634684,1,0,0,2,1,2,0,'',1000,1000,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{140000008124BD5900142AA7AB810000}','2008-03-26 21:20:45','{010000007CC0C17B00142AA7AB810000}',17,1,1,3,0,439537351,1,0,0,2,4,3,0,'',1000,1000,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{1300000016C66D6000142AA7AB810000}','2008-03-26 21:20:45','{010000007CC0C17B00142AA7AB810000}',17,1,1,3,0,3110835098,1,0,0,2,3,3,0,'',1000,1000,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{120000008FF12F1000142AA7AB810000}','2008-03-26 21:20:45','{010000007CC0C17B00142AA7AB810000}',17,1,1,3,0,1740059185,1,0,0,2,2,3,0,'',1000,1000,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{10000000ADADE87800142AA7AB810000}','2008-03-26 21:20:45','{010000007CC0C17B00142AA7AB810000}',17,1,1,3,0,167282091,1,0,0,2,0,3,0,'',1000,1000,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{0F000000B2BDE7C400142AA7AB810000}','2008-03-26 21:20:45','{010000007CC0C17B00142AA7AB810000}',17,1,1,3,0,1994095534,1,0,0,2,4,2,0,'',1000,1000,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{0E0000005B0428A300142AA7AB810000}','2008-03-26 21:20:45','{010000007CC0C17B00142AA7AB810000}',17,1,1,3,0,4016314293,1,0,0,2,3,2,0,'',1000,1000,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{0A00000067A3E6DF00142AA7AB810000}','2008-03-26 21:19:56','{010000007CC0C17B00142AA7AB810000}',18,1,24,1,0,351849931,1,2,0,2,4,1,0,'',1000,1000,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{0900000074EE50CE00142AA7AB810000}','2008-03-26 21:19:56','{010000007CC0C17B00142AA7AB810000}',18,1,24,1,0,2874849614,1,2,0,2,3,1,0,'',1000,1000,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{08000000C556102F00142AA7AB810000}','2008-03-26 21:19:56','{010000007CC0C17B00142AA7AB810000}',18,1,24,1,0,3513597141,1,2,0,2,2,1,0,'',1000,1000,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{07000000AA7A386B00142AA7AB810000}','2008-03-26 21:19:56','{010000007CC0C17B00142AA7AB810000}',18,1,24,1,0,3058453264,1,2,0,2,1,1,0,'',1000,1000,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{06000000B33A8AA900142AA7AB810000}','2008-03-26 21:19:56','{010000007CC0C17B00142AA7AB810000}',18,1,24,1,0,558015727,1,2,0,2,0,1,0,'',1000,1000,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{05000000B0B6BBC700142AA7AB810000}','2008-03-26 21:19:56','{010000007CC0C17B00142AA7AB810000}',18,1,24,1,0,2784062626,1,2,0,2,4,0,0,'',1000,1000,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{04000000B14ECCCD00142AA7AB810000}','2008-03-26 21:19:56','{010000007CC0C17B00142AA7AB810000}',18,1,24,1,0,1353350297,1,2,0,2,3,0,0,'',1000,1000,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{0300000006F8190000142AA7AB810000}','2008-03-26 21:19:56','{010000007CC0C17B00142AA7AB810000}',18,1,24,1,0,3790156420,1,2,0,2,2,0,0,'',1000,1000,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{020000003F76BB6400142AA7AB810000}','2008-03-26 21:19:56','{010000007CC0C17B00142AA7AB810000}',18,1,24,1,0,1374658131,1,2,0,2,1,0,0,'',1000,1000,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{010000002C485E2A00142AA7AB810000}','2008-03-26 21:19:56','{010000007CC0C17B00142AA7AB810000}',18,1,24,1,0,105596214,1,2,0,2,0,0,0,'',1000,1000,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{01000000521665E800142AA7AB810000}','2008-03-26 22:40:29','{0500000000C87D8500142AA7AB810000}',2,1,1,1,2,3875451853,1,2,0,2,0,0,0,'',1000,1000,4294967295,0,119,1,141,1,85,1,0,0,0,0,0,1,0,0),
  ('{02000000CD23DB6300142AA7AB810000}','2008-03-26 23:05:47','{0500000000C87D8500142AA7AB810000}',1,1,1,5,5,720883736,1,0,0,2,1,0,0,'',1000,1000,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{030000005CA77E7500142AA7AB810000}','2008-03-26 23:05:49','{07000000E91355A3000AEBF4783E0000}',1,1,1,3,5,844983474,1,0,0,2,0,0,0,'',1000,1000,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{0100000071FC1E4F00142AA7AB810000}','2008-03-26 20:05:45','{010000003DA5D05C00142AA7AB810000}',1,3,2,1,3,277933812,1,0,0,2,0,0,0,'',1000,1000,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{010000001AD1B5FB00142AA7AB810000}','2008-03-26 19:52:36','{010000003DA5D05C00142AA7AB810000}',2,1,1,1,2,2477002373,1,2,0,0,3,0,0,'',955,1000,4294967295,0,119,1,141,1,85,1,0,0,0,0,0,1,0,0),
  ('{05000000B6D0282E00142AA7AB810000}','2008-03-27 11:00:00','{06000000C346CD3000142AA7AB810000}',1,1,2,1,1,3575372131,1,0,0,2,1,0,0,'',1000,1000,255,0,141,3,108,3,0,0,0,0,0,0,0,1,0,0),
  ('{04000000AF18333A00142AA7AB810000}','2008-03-27 10:59:52','{06000000C346CD3000142AA7AB810000}',1,12,5,3,0,2185660161,1,0,0,2,0,0,0,'',1000,1000,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{020000001456EF7100142AA7AB810000}','2008-03-27 14:45:11','{01000000C42D4EFC000AEBF4783E0000}',1,3,2,2,3,3579855580,1,0,0,2,2,0,0,'',1000,1000,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{010000006087B44D00142AA7AB810000}','2008-03-27 11:44:58','{01000000C42D4EFC000AEBF4783E0000}',1,3,2,1,5,765418739,1,0,0,2,1,0,0,'',1000,1000,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{010000009DA2857700142AA7AB810000}','2008-03-25 10:44:39','{01000000C42D4EFC000AEBF4783E0000}',1,3,2,2,5,2852235671,1,0,0,2,0,0,0,'',1000,1000,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{0500000039B7239500142AA7AB810000}','2008-03-27 14:55:53','{09000000C468D1CC00142AA7AB810000}',2,1,1,1,4,1500657120,1,2,0,2,3,0,0,'',1000,1000,4294967295,0,119,1,141,1,85,1,0,0,0,0,0,1,0,0),
  ('{01000000E55E5EE900142AA7AB810000}','2008-03-27 14:44:07','{09000000C468D1CC00142AA7AB810000}',1,12,5,3,0,876380296,1,0,0,2,0,0,0,'',1000,1000,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{0300000087EC713F00142AA7AB810000}','2008-03-27 14:49:56','{09000000C468D1CC00142AA7AB810000}',1,1,1,1,2,2760089712,1,0,0,2,1,0,0,'',1000,1000,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{040000002EB5394A00142AA7AB810000}','2008-03-27 14:51:03','{09000000C468D1CC00142AA7AB810000}',22,1,1,3,0,3808412253,1,0,0,2,2,0,0,'',1000,1000,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{070000007B27F4DB00142AA7AB810000}','2008-03-27 15:25:31','{0B0000005EDD03E900142AA7AB810000}',1,1,3,1,1,2217977628,1,0,0,0,3,0,0,'',1000,1000,255,0,90,2,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{06000000189D11E300142AA7AB810000}','2008-03-27 15:24:10','{0B0000005EDD03E900142AA7AB810000}',1,12,6,3,0,26607816,1,0,0,2,0,0,0,'',1000,1000,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{050000002DB3B47800142AA7AB810000}','2008-03-25 09:54:58','{020000007F8B517A00142AA7AB810000}',22,1,19,7,0,105315403,1,0,0,2,3,0,0,'',1000,1000,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{040000003211331900142AA7AB810000}','2008-03-25 09:54:57','{020000007F8B517A00142AA7AB810000}',22,1,19,2,0,1120019918,1,0,0,2,2,0,0,'',1000,1000,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{03000000DB3DA6ED00142AA7AB810000}','2008-03-25 09:54:55','{020000007F8B517A00142AA7AB810000}',22,1,19,3,0,2382073685,1,0,0,2,1,0,0,'',1000,1000,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{02000000F86A9BDE00142AA7AB810000}','2008-03-25 09:54:54','{020000007F8B517A00142AA7AB810000}',22,1,19,5,0,3965455248,1,0,0,2,0,0,0,'',1000,1000,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{0100000091B2C1BC00142AA7AB810000}','2008-03-27 15:56:58','{0700000035A1D54100142AA7AB810000}',1,10,1,4,5,2363907269,1,0,0,2,4,1,0,'',1000,1000,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{040000006DC8131B000AEBF4783E0000}','2008-03-26 16:18:48','{0700000035A1D54100142AA7AB810000}',20,1,413,1,0,3887708084,1,2,0,2,3,0,0,'',1000,1000,4294967295,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{03000000725F9651000AEBF4783E0000}','2008-03-26 16:18:48','{0700000035A1D54100142AA7AB810000}',1,7,1,1,3,3803855498,1,2,0,2,2,0,0,'',1000,1000,4294967295,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{020000001B79599F000AEBF4783E0000}','2008-03-26 16:13:37','{0700000035A1D54100142AA7AB810000}',22,1,3,10,0,2168063473,1,0,0,2,1,0,0,'',1000,1000,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{00000000000000000000000000000000}','2008-03-19 14:29:05','{0700000035A1D54100142AA7AB810000}',1,3,2,1,1,3397270378,1,0,0,0,0,0,0,'',0,0,0,0,0,0,0,16777216,0,0,0,0,0,0,0,1,0,0),
  ('{07000000F5C5CAAC00142AA7AB810000}','2008-03-26 10:53:36','{0700000035A1D54100142AA7AB810000}',18,1,3,1,0,1415371680,1,0,0,2,1,1,0,'',1000,1000,4294967295,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{0A0000007E96012400142AA7AB810000}','2008-03-26 10:55:10','{0700000035A1D54100142AA7AB810000}',1,10,1,1,3,1751746707,1,2,0,2,0,1,0,'',1000,1000,4294967295,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{0B000000495395C300142AA7AB810000}','2008-03-26 10:55:10','{0700000035A1D54100142AA7AB810000}',1,6,1,1,1,4125369790,1,2,0,2,3,1,0,'',1000,1000,4294967295,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{01000000EAA196B200142AA7AB810000}','2008-03-26 11:40:32','{0700000035A1D54100142AA7AB810000}',17,1,2,1,0,1086416836,1,0,0,2,0,0,0,'',1000,1000,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{08000000E4603A2800142AA7AB810000}','2008-03-26 10:53:36','{0700000035A1D54100142AA7AB810000}',18,1,3,1,0,2722795941,1,0,0,2,2,1,0,'',1000,1000,4294967295,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{05000000A5F00A5600142AA7AB810000}','2008-03-27 16:40:23','{030000008147D23800142AA7AB810000}',1,9,3,1,2,3089344992,1,0,0,2,2,0,0,'',1000,1000,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{0100000051F4BE5200142AA7AB810000}','2008-03-27 15:35:45','{030000008147D23800142AA7AB810000}',1,3,3,4,1,2873824930,1,0,0,2,0,0,0,'',1000,1000,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{01000000780068FB00142AA7AB810000}','2008-03-25 16:35:22','{030000008147D23800142AA7AB810000}',22,1,1,1,0,2005994880,12,0,0,2,1,0,0,'',1000,1000,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{06000000D4A0E0B700142AA7AB810000}','2008-03-27 16:55:29','{0C000000A9FE01E400142AA7AB810000}',1,1,5,1,1,4214311078,1,0,0,0,3,0,0,'',950,1000,255,0,119,2,0,0,0,0,0,0,0,0,0,1,0,0),
  ('{0300000093BD3DF100142AA7AB810000}','2008-03-27 16:08:14','{0C000000A9FE01E400142AA7AB810000}',2,1,1,1,4,1238340352,1,2,0,2,3,0,0,'',984,1000,4294967295,0,119,1,141,1,85,1,0,0,0,0,0,1,0,0),
  ('{07000000470CB96500142AA7AB810000}','2008-03-27 16:55:45','{0C000000A9FE01E400142AA7AB810000}',1,1,5,1,1,3466495211,1,0,0,2,2,0,0,'',1000,1000,255,0,141,3,1037,1,0,0,0,0,0,0,0,1,0,0),
  ('{02000000100B035500142AA7AB810000}','2008-03-27 16:00:09','{0C000000A9FE01E400142AA7AB810000}',1,12,10,4,0,306358087,1,0,0,2,0,0,0,'',1000,1000,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0);

COMMIT;

#
# Data for the `kin` table  (LIMIT 0,500)
#

INSERT INTO `kin` (`ID`, `LogTime`, `Source`, `IP`, `Level`, `Field1`, `Field2`) VALUES 
  (22,'2008-01-05',0,'0.0.0.0',0,'23','????'),
  (23,'2008-02-03',1,'127.1.1.1',1,'0','???');

COMMIT;

#
# Data for the `log_10_12` table  (LIMIT 0,500)
#

INSERT INTO `log_10_12` (`ID`, `LogTime`, `Source`, `Ip`, `Level`, `Field1`, `Field2`) VALUES 
  (1,'2008-03-28',3,'192.168.20.198',2,0,100);

COMMIT;

#
# Data for the `log_120_20` table  (LIMIT 0,500)
#

INSERT INTO `log_120_20` (`ID`, `LogTime`, `Source`, `Ip`, `Level`, `Field1`, `Field2`, `Field3`, `Field4`) VALUES 
  (1,'2008-04-25 17:04:02',103,'127.0.0.1',9,1234567890123,2000,9876543210765,'大家好大家早！');

COMMIT;

#
# Data for the `log_121_20` table  (LIMIT 0,500)
#

INSERT INTO `log_121_20` (`ID`, `LogTime`, `Source`, `Ip`, `Level`, `Field1`, `Field2`, `Field3`, `Field4`) VALUES 
  (1,'2008-04-25 17:48:00',103,'127.0.0.1',9,1234567890123,2000,9876543210765,'大家好大家早！');

COMMIT;

#
# Data for the `log_12_20` table  (LIMIT 0,500)
#

INSERT INTO `log_12_20` (`ID`, `LogTime`, `Source`, `Ip`, `Level`, `Field1`, `Field2`, `Field3`, `Field4`, `Field5`) VALUES 
  (9,'2008-03-27 15:39:46',10,'127.0.0.1',9,'\0',1234567890123,2000,987654321098765,'?????????'),
  (10,'2008-03-27 15:40:30',10,'127.0.0.1',9,'\0',1234567890123,2000,987654321098765,'大家好才是真的好！'),
  (11,'2008-03-27 15:41:40',10,'127.0.0.1',9,'\0',1234567890123,2000,987654321098765,'?ó????????????????'),
  (12,'2008-03-27 15:43:08',10,'127.0.0.1',9,'\0',1234567890123,2000,987654321098765,'?ó????????????????'),
  (13,'2008-03-27 15:43:44',10,'127.0.0.1',9,'\0',1234567890123,2000,987654321098765,'?ó????????????????'),
  (14,'2008-03-27 15:44:19',10,'127.0.0.1',9,'\0',1234567890123,2000,987654321098765,'?????????'),
  (15,'2008-03-27 15:45:22',10,'127.0.0.1',9,'\0',1234567890123,2000,987654321098765,'大家好才是真的好！'),
  (16,'2008-03-27 15:46:06',10,'127.0.0.1',9,'\0',1234567890123,2000,987654321098765,'大家好大家早！'),
  (17,'2008-03-27 15:47:05',10,'127.0.0.1',9,'\0',1234567890123,2000,987654321098765,'大家好大家早！'),
  (18,'2008-03-27 15:47:41',10,'127.0.0.1',9,'\0',1234567890123,2000,987654321098765,'???????'),
  (19,'2008-03-27 15:48:17',10,'127.0.0.1',9,'\0',1234567890123,2000,987654321098765,'大家好大家早！');

COMMIT;

#
# Data for the `log_3_1` table  (LIMIT 0,500)
#

INSERT INTO `log_3_1` (`ID`, `LogTime`, `Source`, `Ip`, `Level`, `Field1`, `Field2`) VALUES 
  (1,'2008-03-26 10:20:34',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',25),
  (2,'2008-03-26 10:20:35',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',26),
  (3,'2008-03-26 10:20:35',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',27),
  (4,'2008-03-26 10:20:36',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',28),
  (5,'2008-03-26 10:20:36',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',29),
  (6,'2008-03-26 10:20:36',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',30),
  (7,'2008-03-26 10:20:36',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',31),
  (8,'2008-03-26 10:20:36',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',32),
  (9,'2008-03-26 10:20:36',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',33),
  (10,'2008-03-26 10:20:37',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',34),
  (11,'2008-03-26 10:20:37',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',35),
  (12,'2008-03-26 10:20:37',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',36),
  (13,'2008-03-26 10:20:37',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',37),
  (14,'2008-03-26 10:20:38',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',38),
  (15,'2008-03-26 10:20:38',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',39),
  (16,'2008-03-26 10:20:38',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',40),
  (17,'2008-03-26 10:20:38',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',41),
  (18,'2008-03-26 10:20:39',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',42),
  (19,'2008-03-26 10:20:39',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',43),
  (20,'2008-03-26 10:20:39',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',44),
  (21,'2008-03-26 10:20:39',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',45),
  (22,'2008-03-26 10:20:40',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',46),
  (23,'2008-03-26 10:20:40',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',47),
  (24,'2008-03-26 10:20:40',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',48),
  (25,'2008-03-26 10:20:40',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',49),
  (26,'2008-03-26 10:20:41',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',50),
  (27,'2008-03-26 10:20:41',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',51),
  (28,'2008-03-26 10:20:41',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',52),
  (29,'2008-03-26 10:20:41',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',53),
  (30,'2008-03-26 10:20:41',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',54),
  (31,'2008-03-26 10:20:42',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',55),
  (32,'2008-03-26 10:20:42',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',56),
  (33,'2008-03-26 10:20:42',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',57),
  (34,'2008-03-26 10:20:42',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',58),
  (35,'2008-03-26 10:20:42',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',59),
  (36,'2008-03-26 10:20:42',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',60),
  (37,'2008-03-26 10:20:43',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',61),
  (38,'2008-03-26 10:20:44',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',62),
  (39,'2008-03-26 10:20:44',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',63),
  (40,'2008-03-26 10:20:44',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',64),
  (41,'2008-03-26 10:20:44',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',65),
  (42,'2008-03-26 10:20:44',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',66),
  (43,'2008-03-26 10:20:44',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',67),
  (44,'2008-03-26 10:20:45',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',68),
  (45,'2008-03-26 10:20:45',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',69),
  (46,'2008-03-26 10:20:45',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',70),
  (47,'2008-03-26 10:20:45',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',71),
  (48,'2008-03-26 10:20:45',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',72),
  (49,'2008-03-26 10:20:45',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',73),
  (50,'2008-03-26 10:20:46',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',74),
  (51,'2008-03-26 10:20:47',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',75),
  (52,'2008-03-26 10:20:47',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',76),
  (53,'2008-03-26 10:20:48',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',77),
  (54,'2008-03-26 10:20:48',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',78),
  (55,'2008-03-26 10:20:49',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',79),
  (56,'2008-03-26 10:20:49',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',80),
  (57,'2008-03-26 10:20:49',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',81),
  (58,'2008-03-26 10:20:49',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',82),
  (59,'2008-03-26 10:20:50',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',83),
  (60,'2008-03-26 10:20:50',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',84),
  (61,'2008-03-26 10:20:50',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',85),
  (62,'2008-03-26 10:20:50',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',86),
  (63,'2008-03-26 10:20:50',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',87),
  (64,'2008-03-26 10:20:50',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',88),
  (65,'2008-03-26 10:20:51',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',89),
  (66,'2008-03-26 10:20:51',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',90),
  (67,'2008-03-26 10:20:51',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',91),
  (68,'2008-03-26 10:20:51',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',92),
  (69,'2008-03-26 10:20:51',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',93),
  (70,'2008-03-26 10:20:51',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',94),
  (71,'2008-03-26 10:20:51',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',95),
  (72,'2008-03-26 10:20:51',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',96),
  (73,'2008-03-26 10:20:52',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',97),
  (74,'2008-03-26 10:20:52',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',98),
  (75,'2008-03-26 10:20:52',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',99),
  (76,'2008-03-26 10:20:52',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',100),
  (77,'2008-03-26 17:10:00',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',2),
  (78,'2008-03-26 17:10:00',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',3),
  (79,'2008-03-26 17:10:00',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',4),
  (80,'2008-03-26 17:10:00',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',5),
  (81,'2008-03-26 17:10:00',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',6),
  (82,'2008-03-26 17:10:00',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',7),
  (83,'2008-03-26 17:10:00',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',8),
  (84,'2008-03-26 17:10:00',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',9),
  (85,'2008-03-26 17:10:00',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',10),
  (86,'2008-03-26 17:10:00',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',11),
  (87,'2008-03-26 17:10:01',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',12),
  (88,'2008-03-26 17:10:01',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',13),
  (89,'2008-03-26 17:10:01',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',14),
  (90,'2008-03-26 17:10:01',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',15),
  (91,'2008-03-26 17:10:01',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',16),
  (92,'2008-03-26 17:10:01',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',17),
  (93,'2008-03-26 17:10:01',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',18),
  (94,'2008-03-26 17:10:01',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',19),
  (95,'2008-03-26 17:10:10',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',20),
  (96,'2008-03-26 17:10:10',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',21),
  (97,'2008-03-26 17:10:10',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',22),
  (98,'2008-03-26 17:10:10',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',23),
  (99,'2008-03-26 17:10:10',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',24),
  (100,'2008-03-26 17:10:10',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',25),
  (101,'2008-03-26 17:10:10',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',26),
  (102,'2008-03-26 17:10:10',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',27),
  (103,'2008-03-26 17:10:10',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',28),
  (104,'2008-03-26 17:10:10',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',29),
  (105,'2008-03-26 17:10:10',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',30),
  (106,'2008-03-26 17:10:10',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',31),
  (107,'2008-03-26 17:10:10',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',32),
  (108,'2008-03-26 17:10:10',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',33),
  (109,'2008-03-26 17:10:10',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',34),
  (110,'2008-03-26 17:10:10',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',35),
  (111,'2008-03-26 17:10:10',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',36),
  (112,'2008-03-26 17:10:10',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',37),
  (113,'2008-03-26 17:10:10',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',38),
  (114,'2008-03-26 17:10:10',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',39),
  (115,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',40),
  (116,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',41),
  (117,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',42),
  (118,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',43),
  (119,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',44),
  (120,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',45),
  (121,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',46),
  (122,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',47),
  (123,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',48),
  (124,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',49),
  (125,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',50),
  (126,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',51),
  (127,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',52),
  (128,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',53),
  (129,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',54),
  (130,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',55),
  (131,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',56),
  (132,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',57),
  (133,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',58),
  (134,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',59),
  (135,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',60),
  (136,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',61),
  (137,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',62),
  (138,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',63),
  (139,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',64),
  (140,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',65),
  (141,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',66),
  (142,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',67),
  (143,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',68),
  (144,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',69),
  (145,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',70),
  (146,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',71),
  (147,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',72),
  (148,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',73),
  (149,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',74),
  (150,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',75),
  (151,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',76),
  (152,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',77),
  (153,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',78),
  (154,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',79),
  (155,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',80),
  (156,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',81),
  (157,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',82),
  (158,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',83),
  (159,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',84),
  (160,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',85),
  (161,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',86),
  (162,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',87),
  (163,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',88),
  (164,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',89),
  (165,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',90),
  (166,'2008-03-26 17:10:12',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',91),
  (167,'2008-03-26 17:10:12',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',92),
  (168,'2008-03-26 17:10:12',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',93),
  (169,'2008-03-26 17:10:12',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',94),
  (170,'2008-03-26 17:10:12',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',95),
  (171,'2008-03-26 17:10:12',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',96),
  (172,'2008-03-26 17:10:12',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',97),
  (173,'2008-03-26 17:10:12',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',98),
  (174,'2008-03-26 17:10:12',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',99),
  (175,'2008-03-26 17:10:12',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',100),
  (176,'2008-03-26 17:42:24',0,'192.168.20.198',2,'{01000000647890BE00142AA7AB810000}',2),
  (177,'2008-03-26 17:55:24',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',2),
  (178,'2008-03-26 17:57:00',0,'192.168.20.198',2,'{0A000000BE768BD300142AA7AB810000}',2),
  (179,'2008-03-26 17:58:32',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',3),
  (180,'2008-03-26 17:58:32',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',4),
  (181,'2008-03-26 17:58:32',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',5),
  (182,'2008-03-26 17:58:32',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',6),
  (183,'2008-03-26 17:58:32',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',7),
  (184,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',8),
  (185,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',9),
  (186,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',10),
  (187,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',11),
  (188,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',12),
  (189,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',13),
  (190,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',14),
  (191,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',15),
  (192,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',16),
  (193,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',17),
  (194,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',18),
  (195,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',19),
  (196,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',20),
  (197,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',21),
  (198,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',22),
  (199,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',23),
  (200,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',24),
  (201,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',25),
  (202,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',26),
  (203,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',27),
  (204,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',28),
  (205,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',29),
  (206,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',30),
  (207,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',31),
  (208,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',32),
  (209,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',33),
  (210,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',34),
  (211,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',35),
  (212,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',36),
  (213,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',37),
  (214,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',38),
  (215,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',39),
  (216,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',40),
  (217,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',41),
  (218,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',42),
  (219,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',43),
  (220,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',44),
  (221,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',45),
  (222,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',46),
  (223,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',47),
  (224,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',48),
  (225,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',49),
  (226,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',50),
  (227,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',51),
  (228,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',52),
  (229,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',53),
  (230,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',54),
  (231,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',55),
  (232,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',56),
  (233,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',57),
  (234,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',58),
  (235,'2008-03-26 17:58:33',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',59),
  (236,'2008-03-26 17:58:34',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',60),
  (237,'2008-03-26 17:58:34',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',61),
  (238,'2008-03-26 17:58:34',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',62),
  (239,'2008-03-26 17:58:34',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',63),
  (240,'2008-03-26 17:58:34',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',64),
  (241,'2008-03-26 17:58:34',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',65),
  (242,'2008-03-26 17:58:34',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',66),
  (243,'2008-03-26 17:58:34',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',67),
  (244,'2008-03-26 17:58:34',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',68),
  (245,'2008-03-26 17:58:34',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',69),
  (246,'2008-03-26 17:58:34',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',70),
  (247,'2008-03-26 17:58:34',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',71),
  (248,'2008-03-26 17:58:34',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',72),
  (249,'2008-03-26 17:58:34',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',73),
  (250,'2008-03-26 17:58:34',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',74),
  (251,'2008-03-26 17:58:34',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',75),
  (252,'2008-03-26 17:58:34',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',76),
  (253,'2008-03-26 17:58:34',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',77),
  (254,'2008-03-26 17:58:34',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',78),
  (255,'2008-03-26 17:58:34',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',79),
  (256,'2008-03-26 17:58:34',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',80),
  (257,'2008-03-26 17:58:34',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',81),
  (258,'2008-03-26 17:58:34',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',82),
  (259,'2008-03-26 17:58:34',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',83),
  (260,'2008-03-26 17:58:34',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',84),
  (261,'2008-03-26 17:58:34',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',85),
  (262,'2008-03-26 17:58:34',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',86),
  (263,'2008-03-26 17:58:34',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',87),
  (264,'2008-03-26 17:58:34',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',88),
  (265,'2008-03-26 17:58:34',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',89),
  (266,'2008-03-26 17:58:34',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',90),
  (267,'2008-03-26 17:58:34',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',91),
  (268,'2008-03-26 17:58:34',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',92),
  (269,'2008-03-26 17:58:34',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',93),
  (270,'2008-03-26 17:58:34',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',94),
  (271,'2008-03-26 17:58:34',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',95),
  (272,'2008-03-26 17:58:34',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',96),
  (273,'2008-03-26 17:58:34',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',97),
  (274,'2008-03-26 17:58:34',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',98),
  (275,'2008-03-26 17:58:34',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',99),
  (276,'2008-03-26 17:58:34',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',100),
  (277,'2008-03-26 18:06:09',0,'192.168.20.198',2,'{02000000BAB0CE65000AEBF4783E0000}',2),
  (278,'2008-03-26 18:06:09',0,'192.168.20.198',2,'{02000000BAB0CE65000AEBF4783E0000}',3),
  (279,'2008-03-26 18:06:09',0,'192.168.20.198',2,'{02000000BAB0CE65000AEBF4783E0000}',4),
  (280,'2008-03-26 18:06:09',0,'192.168.20.198',2,'{02000000BAB0CE65000AEBF4783E0000}',5),
  (281,'2008-03-26 18:06:09',0,'192.168.20.198',2,'{02000000BAB0CE65000AEBF4783E0000}',6),
  (282,'2008-03-26 18:06:09',0,'192.168.20.198',2,'{02000000BAB0CE65000AEBF4783E0000}',7),
  (283,'2008-03-26 18:06:09',0,'192.168.20.198',2,'{02000000BAB0CE65000AEBF4783E0000}',8),
  (284,'2008-03-26 18:06:09',0,'192.168.20.198',2,'{02000000BAB0CE65000AEBF4783E0000}',9),
  (285,'2008-03-26 18:06:09',0,'192.168.20.198',2,'{02000000BAB0CE65000AEBF4783E0000}',10),
  (286,'2008-03-26 18:06:09',0,'192.168.20.198',2,'{02000000BAB0CE65000AEBF4783E0000}',11),
  (287,'2008-03-26 18:06:09',0,'192.168.20.198',2,'{02000000BAB0CE65000AEBF4783E0000}',12),
  (288,'2008-03-26 18:06:09',0,'192.168.20.198',2,'{02000000BAB0CE65000AEBF4783E0000}',13),
  (289,'2008-03-26 18:06:09',0,'192.168.20.198',2,'{02000000BAB0CE65000AEBF4783E0000}',14),
  (290,'2008-03-26 18:06:09',0,'192.168.20.198',2,'{02000000BAB0CE65000AEBF4783E0000}',15),
  (291,'2008-03-26 18:06:09',0,'192.168.20.198',2,'{02000000BAB0CE65000AEBF4783E0000}',16),
  (292,'2008-03-26 18:06:09',0,'192.168.20.198',2,'{02000000BAB0CE65000AEBF4783E0000}',17),
  (293,'2008-03-26 18:06:09',0,'192.168.20.198',2,'{02000000BAB0CE65000AEBF4783E0000}',18),
  (294,'2008-03-26 18:06:09',0,'192.168.20.198',2,'{02000000BAB0CE65000AEBF4783E0000}',19),
  (295,'2008-03-26 18:06:09',0,'192.168.20.198',2,'{02000000BAB0CE65000AEBF4783E0000}',20),
  (296,'2008-03-26 18:06:09',0,'192.168.20.198',2,'{02000000BAB0CE65000AEBF4783E0000}',21),
  (297,'2008-03-26 18:06:09',0,'192.168.20.198',2,'{02000000BAB0CE65000AEBF4783E0000}',22),
  (298,'2008-03-26 18:06:10',0,'192.168.20.198',2,'{02000000BAB0CE65000AEBF4783E0000}',23),
  (299,'2008-03-26 18:06:10',0,'192.168.20.198',2,'{02000000BAB0CE65000AEBF4783E0000}',24),
  (300,'2008-03-26 18:06:10',0,'192.168.20.198',2,'{02000000BAB0CE65000AEBF4783E0000}',25),
  (301,'2008-03-26 18:06:10',0,'192.168.20.198',2,'{02000000BAB0CE65000AEBF4783E0000}',26),
  (302,'2008-03-26 18:06:10',0,'192.168.20.198',2,'{02000000BAB0CE65000AEBF4783E0000}',27),
  (303,'2008-03-26 18:06:10',0,'192.168.20.198',2,'{02000000BAB0CE65000AEBF4783E0000}',28),
  (304,'2008-03-26 18:06:10',0,'192.168.20.198',2,'{02000000BAB0CE65000AEBF4783E0000}',29),
  (305,'2008-03-26 18:06:10',0,'192.168.20.198',2,'{02000000BAB0CE65000AEBF4783E0000}',30),
  (306,'2008-03-26 19:48:52',0,'192.168.20.198',2,'{010000003DA5D05C00142AA7AB810000}',2),
  (307,'2008-03-26 19:52:36',0,'192.168.20.198',2,'{010000003DA5D05C00142AA7AB810000}',3),
  (308,'2008-03-26 20:06:23',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',2),
  (309,'2008-03-26 20:06:23',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',3),
  (310,'2008-03-26 20:06:23',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',4),
  (311,'2008-03-26 20:06:23',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',5),
  (312,'2008-03-26 20:06:23',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',6),
  (313,'2008-03-26 20:06:23',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',7),
  (314,'2008-03-26 20:06:23',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',8),
  (315,'2008-03-26 20:06:23',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',9),
  (316,'2008-03-26 20:06:23',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',10),
  (317,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',11),
  (318,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',12),
  (319,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',13),
  (320,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',14),
  (321,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',15),
  (322,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',16),
  (323,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',17),
  (324,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',18),
  (325,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',19),
  (326,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',20),
  (327,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',21),
  (328,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',22),
  (329,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',23),
  (330,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',24),
  (331,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',25),
  (332,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',26),
  (333,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',27),
  (334,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',28),
  (335,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',29),
  (336,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',30),
  (337,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',31),
  (338,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',32),
  (339,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',33),
  (340,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',34),
  (341,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',35),
  (342,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',36),
  (343,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',37),
  (344,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',38),
  (345,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',39),
  (346,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',40),
  (347,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',41),
  (348,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',42),
  (349,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',43),
  (350,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',44),
  (351,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',45),
  (352,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',46),
  (353,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',47),
  (354,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',48),
  (355,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',49),
  (356,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',50),
  (357,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',51),
  (358,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',52),
  (359,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',53),
  (360,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',54),
  (361,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',55),
  (362,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',56),
  (363,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',57),
  (364,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',58),
  (365,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',59),
  (366,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',60),
  (367,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',61),
  (368,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',62),
  (369,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',63),
  (370,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',64),
  (371,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',65),
  (372,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',66),
  (373,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',67),
  (374,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',68),
  (375,'2008-03-26 20:06:25',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',69),
  (376,'2008-03-26 20:06:25',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',70),
  (377,'2008-03-26 20:06:25',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',71),
  (378,'2008-03-26 20:06:25',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',72),
  (379,'2008-03-26 20:06:25',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',73),
  (380,'2008-03-26 20:06:25',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',74),
  (381,'2008-03-26 20:06:25',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',75),
  (382,'2008-03-26 20:06:25',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',76),
  (383,'2008-03-26 20:06:25',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',77),
  (384,'2008-03-26 20:06:25',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',78),
  (385,'2008-03-26 20:06:25',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',79),
  (386,'2008-03-26 20:06:25',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',80),
  (387,'2008-03-26 20:06:25',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',81),
  (388,'2008-03-26 20:06:25',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',82),
  (389,'2008-03-26 20:06:25',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',83),
  (390,'2008-03-26 20:06:25',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',84),
  (391,'2008-03-26 20:06:25',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',85),
  (392,'2008-03-26 20:06:25',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',86),
  (393,'2008-03-26 20:06:25',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',87),
  (394,'2008-03-26 20:06:25',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',88),
  (395,'2008-03-26 20:06:25',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',89),
  (396,'2008-03-26 20:06:25',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',90),
  (397,'2008-03-26 20:06:25',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',91),
  (398,'2008-03-26 20:06:25',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',92),
  (399,'2008-03-26 20:06:25',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',93),
  (400,'2008-03-26 20:06:25',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',94),
  (401,'2008-03-26 20:06:25',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',95),
  (402,'2008-03-26 20:06:25',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',96),
  (403,'2008-03-26 20:06:25',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',97),
  (404,'2008-03-26 20:06:25',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',98),
  (405,'2008-03-26 20:06:25',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',99),
  (406,'2008-03-26 20:06:25',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',100),
  (407,'2008-03-26 20:21:27',0,'192.168.20.198',2,'{010000003DA5D05C00142AA7AB810000}',4),
  (408,'2008-03-26 20:21:27',0,'192.168.20.198',2,'{010000003DA5D05C00142AA7AB810000}',5),
  (409,'2008-03-26 20:21:27',0,'192.168.20.198',2,'{010000003DA5D05C00142AA7AB810000}',6),
  (410,'2008-03-26 20:21:27',0,'192.168.20.198',2,'{010000003DA5D05C00142AA7AB810000}',7),
  (411,'2008-03-26 20:21:27',0,'192.168.20.198',2,'{010000003DA5D05C00142AA7AB810000}',8),
  (412,'2008-03-26 20:21:27',0,'192.168.20.198',2,'{010000003DA5D05C00142AA7AB810000}',9),
  (413,'2008-03-26 20:21:27',0,'192.168.20.198',2,'{010000003DA5D05C00142AA7AB810000}',10),
  (414,'2008-03-26 20:21:27',0,'192.168.20.198',2,'{010000003DA5D05C00142AA7AB810000}',11),
  (415,'2008-03-26 20:21:27',0,'192.168.20.198',2,'{010000003DA5D05C00142AA7AB810000}',12),
  (416,'2008-03-26 20:21:27',0,'192.168.20.198',2,'{010000003DA5D05C00142AA7AB810000}',13),
  (417,'2008-03-26 20:21:27',0,'192.168.20.198',2,'{010000003DA5D05C00142AA7AB810000}',14),
  (418,'2008-03-26 20:21:27',0,'192.168.20.198',2,'{010000003DA5D05C00142AA7AB810000}',15),
  (419,'2008-03-26 20:21:28',0,'192.168.20.198',2,'{010000003DA5D05C00142AA7AB810000}',16),
  (420,'2008-03-26 20:21:28',0,'192.168.20.198',2,'{010000003DA5D05C00142AA7AB810000}',17),
  (421,'2008-03-26 20:21:28',0,'192.168.20.198',2,'{010000003DA5D05C00142AA7AB810000}',18),
  (422,'2008-03-26 20:21:28',0,'192.168.20.198',2,'{010000003DA5D05C00142AA7AB810000}',19),
  (423,'2008-03-26 20:21:28',0,'192.168.20.198',2,'{010000003DA5D05C00142AA7AB810000}',20),
  (424,'2008-03-26 20:21:28',0,'192.168.20.198',2,'{010000003DA5D05C00142AA7AB810000}',21),
  (425,'2008-03-26 20:21:28',0,'192.168.20.198',2,'{010000003DA5D05C00142AA7AB810000}',22),
  (426,'2008-03-26 20:21:28',0,'192.168.20.198',2,'{010000003DA5D05C00142AA7AB810000}',23),
  (427,'2008-03-26 20:21:28',0,'192.168.20.198',2,'{010000003DA5D05C00142AA7AB810000}',24),
  (428,'2008-03-26 20:21:28',0,'192.168.20.198',2,'{010000003DA5D05C00142AA7AB810000}',25),
  (429,'2008-03-26 20:21:28',0,'192.168.20.198',2,'{010000003DA5D05C00142AA7AB810000}',26),
  (430,'2008-03-26 20:21:28',0,'192.168.20.198',2,'{010000003DA5D05C00142AA7AB810000}',27),
  (431,'2008-03-26 20:21:28',0,'192.168.20.198',2,'{010000003DA5D05C00142AA7AB810000}',28),
  (432,'2008-03-26 20:21:28',0,'192.168.20.198',2,'{010000003DA5D05C00142AA7AB810000}',29),
  (433,'2008-03-26 20:21:28',0,'192.168.20.198',2,'{010000003DA5D05C00142AA7AB810000}',30),
  (434,'2008-03-26 21:17:07',0,'192.168.20.198',2,'{020000003393F566000AEB2351BE0000}',71),
  (435,'2008-03-26 21:17:07',0,'192.168.20.198',2,'{020000003393F566000AEB2351BE0000}',72),
  (436,'2008-03-26 21:17:07',0,'192.168.20.198',2,'{020000003393F566000AEB2351BE0000}',73),
  (437,'2008-03-26 21:17:07',0,'192.168.20.198',2,'{020000003393F566000AEB2351BE0000}',74),
  (438,'2008-03-26 21:17:07',0,'192.168.20.198',2,'{020000003393F566000AEB2351BE0000}',75),
  (439,'2008-03-26 21:17:07',0,'192.168.20.198',2,'{020000003393F566000AEB2351BE0000}',76),
  (440,'2008-03-26 21:17:07',0,'192.168.20.198',2,'{020000003393F566000AEB2351BE0000}',77),
  (441,'2008-03-26 21:17:07',0,'192.168.20.198',2,'{020000003393F566000AEB2351BE0000}',78),
  (442,'2008-03-26 21:17:08',0,'192.168.20.198',2,'{020000003393F566000AEB2351BE0000}',79),
  (443,'2008-03-26 21:17:08',0,'192.168.20.198',2,'{020000003393F566000AEB2351BE0000}',80),
  (444,'2008-03-26 21:17:08',0,'192.168.20.198',2,'{020000003393F566000AEB2351BE0000}',81),
  (445,'2008-03-26 21:17:08',0,'192.168.20.198',2,'{020000003393F566000AEB2351BE0000}',82),
  (446,'2008-03-26 21:17:08',0,'192.168.20.198',2,'{020000003393F566000AEB2351BE0000}',83),
  (447,'2008-03-26 21:17:08',0,'192.168.20.198',2,'{020000003393F566000AEB2351BE0000}',84),
  (448,'2008-03-26 21:17:08',0,'192.168.20.198',2,'{020000003393F566000AEB2351BE0000}',85),
  (449,'2008-03-26 21:17:08',0,'192.168.20.198',2,'{020000003393F566000AEB2351BE0000}',86),
  (450,'2008-03-26 21:17:08',0,'192.168.20.198',2,'{020000003393F566000AEB2351BE0000}',87),
  (451,'2008-03-26 21:17:08',0,'192.168.20.198',2,'{020000003393F566000AEB2351BE0000}',88),
  (452,'2008-03-26 21:17:08',0,'192.168.20.198',2,'{020000003393F566000AEB2351BE0000}',89),
  (453,'2008-03-26 21:17:08',0,'192.168.20.198',2,'{020000003393F566000AEB2351BE0000}',90),
  (454,'2008-03-26 21:17:08',0,'192.168.20.198',2,'{020000003393F566000AEB2351BE0000}',91),
  (455,'2008-03-26 21:17:08',0,'192.168.20.198',2,'{020000003393F566000AEB2351BE0000}',92),
  (456,'2008-03-26 21:17:08',0,'192.168.20.198',2,'{020000003393F566000AEB2351BE0000}',93),
  (457,'2008-03-26 21:17:08',0,'192.168.20.198',2,'{020000003393F566000AEB2351BE0000}',94),
  (458,'2008-03-26 21:17:08',0,'192.168.20.198',2,'{020000003393F566000AEB2351BE0000}',95),
  (459,'2008-03-26 21:17:08',0,'192.168.20.198',2,'{020000003393F566000AEB2351BE0000}',96),
  (460,'2008-03-26 21:17:08',0,'192.168.20.198',2,'{020000003393F566000AEB2351BE0000}',97),
  (461,'2008-03-26 21:17:08',0,'192.168.20.198',2,'{020000003393F566000AEB2351BE0000}',98),
  (462,'2008-03-26 21:17:08',0,'192.168.20.198',2,'{020000003393F566000AEB2351BE0000}',99),
  (463,'2008-03-26 21:17:08',0,'192.168.20.198',2,'{020000003393F566000AEB2351BE0000}',100),
  (464,'2008-03-26 21:18:14',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',2),
  (465,'2008-03-26 21:18:14',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',3),
  (466,'2008-03-26 21:18:14',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',4),
  (467,'2008-03-26 21:18:14',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',5),
  (468,'2008-03-26 21:18:14',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',6),
  (469,'2008-03-26 21:18:14',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',7),
  (470,'2008-03-26 21:18:14',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',8),
  (471,'2008-03-26 21:18:14',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',9),
  (472,'2008-03-26 21:18:14',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',10),
  (473,'2008-03-26 21:18:14',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',11),
  (474,'2008-03-26 21:18:14',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',12),
  (475,'2008-03-26 21:18:14',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',13),
  (476,'2008-03-26 21:18:14',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',14),
  (477,'2008-03-26 21:18:14',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',15),
  (478,'2008-03-26 21:18:14',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',16),
  (479,'2008-03-26 21:18:14',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',17),
  (480,'2008-03-26 21:18:14',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',18),
  (481,'2008-03-26 21:18:14',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',19),
  (482,'2008-03-26 21:18:14',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',20),
  (483,'2008-03-26 21:18:14',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',21),
  (484,'2008-03-26 21:18:14',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',22),
  (485,'2008-03-26 21:18:14',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',23),
  (486,'2008-03-26 21:18:14',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',24),
  (487,'2008-03-26 21:18:14',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',25),
  (488,'2008-03-26 21:18:14',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',26),
  (489,'2008-03-26 21:18:14',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',27),
  (490,'2008-03-26 21:18:14',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',28),
  (491,'2008-03-26 21:18:14',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',29),
  (492,'2008-03-26 21:18:14',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',30),
  (493,'2008-03-26 21:18:14',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',31),
  (494,'2008-03-26 21:18:14',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',32),
  (495,'2008-03-26 21:18:14',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',33),
  (496,'2008-03-26 21:18:14',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',34),
  (497,'2008-03-26 21:18:14',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',35),
  (498,'2008-03-26 21:18:14',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',36),
  (499,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',37),
  (500,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',38);

COMMIT;

#
# Data for the `log_3_1` table  (LIMIT 500,500)
#

INSERT INTO `log_3_1` (`ID`, `LogTime`, `Source`, `Ip`, `Level`, `Field1`, `Field2`) VALUES 
  (501,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',39),
  (502,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',40),
  (503,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',41),
  (504,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',42),
  (505,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',43),
  (506,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',44),
  (507,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',45),
  (508,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',46),
  (509,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',47),
  (510,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',48),
  (511,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',49),
  (512,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',50),
  (513,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',51),
  (514,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',52),
  (515,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',53),
  (516,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',54),
  (517,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',55),
  (518,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',56),
  (519,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',57),
  (520,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',58),
  (521,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',59),
  (522,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',60),
  (523,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',61),
  (524,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',62),
  (525,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',63),
  (526,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',64),
  (527,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',65),
  (528,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',66),
  (529,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',67),
  (530,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',68),
  (531,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',69),
  (532,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',70),
  (533,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',71),
  (534,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',72),
  (535,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',73),
  (536,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',74),
  (537,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',75),
  (538,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',76),
  (539,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',77),
  (540,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',78),
  (541,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',79),
  (542,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',80),
  (543,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',81),
  (544,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',82),
  (545,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',83),
  (546,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',84),
  (547,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',85),
  (548,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',86),
  (549,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',87),
  (550,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',88),
  (551,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',89),
  (552,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',90),
  (553,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',91),
  (554,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',92),
  (555,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',93),
  (556,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',94),
  (557,'2008-03-26 21:18:16',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',95),
  (558,'2008-03-26 21:18:16',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',96),
  (559,'2008-03-26 21:18:16',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',97),
  (560,'2008-03-26 21:18:16',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',98),
  (561,'2008-03-26 21:18:16',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',99),
  (562,'2008-03-26 21:18:16',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',100),
  (563,'2008-03-26 21:57:54',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',2),
  (564,'2008-03-26 22:00:14',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',3),
  (565,'2008-03-26 22:00:14',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',4),
  (566,'2008-03-26 22:00:14',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',5),
  (567,'2008-03-26 22:00:14',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',6),
  (568,'2008-03-26 22:00:14',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',7),
  (569,'2008-03-26 22:00:14',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',8),
  (570,'2008-03-26 22:00:14',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',9),
  (571,'2008-03-26 22:00:14',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',10),
  (572,'2008-03-26 22:00:14',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',11),
  (573,'2008-03-26 22:00:14',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',12),
  (574,'2008-03-26 22:00:24',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',13),
  (575,'2008-03-26 22:00:25',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',14),
  (576,'2008-03-26 22:00:25',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',15),
  (577,'2008-03-26 22:00:25',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',16),
  (578,'2008-03-26 22:00:25',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',17),
  (579,'2008-03-26 22:00:25',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',18),
  (580,'2008-03-26 22:00:25',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',19),
  (581,'2008-03-26 22:00:25',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',20),
  (582,'2008-03-26 22:12:29',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',2),
  (583,'2008-03-26 22:12:29',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',3),
  (584,'2008-03-26 22:12:29',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',4),
  (585,'2008-03-26 22:12:29',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',5),
  (586,'2008-03-26 22:12:29',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',6),
  (587,'2008-03-26 22:12:29',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',7),
  (588,'2008-03-26 22:12:29',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',8),
  (589,'2008-03-26 22:12:29',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',9),
  (590,'2008-03-26 22:12:29',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',10),
  (591,'2008-03-26 22:12:29',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',11),
  (592,'2008-03-26 22:12:29',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',12),
  (593,'2008-03-26 22:12:29',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',13),
  (594,'2008-03-26 22:12:29',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',14),
  (595,'2008-03-26 22:12:29',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',15),
  (596,'2008-03-26 22:12:29',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',16),
  (597,'2008-03-26 22:12:29',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',17),
  (598,'2008-03-26 22:12:29',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',18),
  (599,'2008-03-26 22:12:29',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',19),
  (600,'2008-03-26 22:12:29',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',20),
  (601,'2008-03-26 22:12:29',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',21),
  (602,'2008-03-26 22:12:29',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',22),
  (603,'2008-03-26 22:12:30',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',23),
  (604,'2008-03-26 22:12:30',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',24),
  (605,'2008-03-26 22:12:30',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',25),
  (606,'2008-03-26 22:12:30',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',26),
  (607,'2008-03-26 22:12:30',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',27),
  (608,'2008-03-26 22:12:30',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',28),
  (609,'2008-03-26 22:12:30',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',29),
  (610,'2008-03-26 22:12:30',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',30),
  (611,'2008-03-26 22:12:30',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',31),
  (612,'2008-03-26 22:12:30',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',32),
  (613,'2008-03-26 22:12:30',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',33),
  (614,'2008-03-26 22:12:30',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',34),
  (615,'2008-03-26 22:12:30',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',35),
  (616,'2008-03-26 22:12:30',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',36),
  (617,'2008-03-26 22:12:30',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',37),
  (618,'2008-03-26 22:12:30',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',38),
  (619,'2008-03-26 22:12:30',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',39),
  (620,'2008-03-26 22:12:30',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',40),
  (621,'2008-03-26 22:12:30',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',41),
  (622,'2008-03-26 22:12:30',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',42),
  (623,'2008-03-26 22:12:30',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',43),
  (624,'2008-03-26 22:12:30',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',44),
  (625,'2008-03-26 22:12:30',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',45),
  (626,'2008-03-26 22:12:31',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',46),
  (627,'2008-03-26 22:12:31',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',47),
  (628,'2008-03-26 22:12:31',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',48),
  (629,'2008-03-26 22:12:31',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',49),
  (630,'2008-03-26 22:12:31',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',50),
  (631,'2008-03-26 22:12:31',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',51),
  (632,'2008-03-26 22:12:31',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',52),
  (633,'2008-03-26 22:12:31',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',53),
  (634,'2008-03-26 22:12:31',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',54),
  (635,'2008-03-26 22:12:31',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',55),
  (636,'2008-03-26 22:12:31',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',56),
  (637,'2008-03-26 22:12:31',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',57),
  (638,'2008-03-26 22:12:31',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',58),
  (639,'2008-03-26 22:12:31',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',59),
  (640,'2008-03-26 22:12:31',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',60),
  (641,'2008-03-26 22:12:31',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',61),
  (642,'2008-03-26 22:12:31',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',62),
  (643,'2008-03-26 22:12:31',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',63),
  (644,'2008-03-26 22:12:31',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',64),
  (645,'2008-03-26 22:12:31',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',65),
  (646,'2008-03-26 22:12:31',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',66),
  (647,'2008-03-26 22:12:31',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',67),
  (648,'2008-03-26 22:12:31',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',68),
  (649,'2008-03-26 22:12:31',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',69),
  (650,'2008-03-26 22:12:31',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',70),
  (651,'2008-03-26 22:12:32',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',71),
  (652,'2008-03-26 22:12:32',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',72),
  (653,'2008-03-26 22:12:32',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',73),
  (654,'2008-03-26 22:12:32',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',74),
  (655,'2008-03-26 22:12:32',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',75),
  (656,'2008-03-26 22:12:32',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',76),
  (657,'2008-03-26 22:12:32',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',77),
  (658,'2008-03-26 22:12:32',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',78),
  (659,'2008-03-26 22:12:32',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',79),
  (660,'2008-03-26 22:12:32',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',80),
  (661,'2008-03-26 22:12:32',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',81),
  (662,'2008-03-26 22:12:32',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',82),
  (663,'2008-03-26 22:12:32',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',83),
  (664,'2008-03-26 22:12:32',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',84),
  (665,'2008-03-26 22:12:32',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',85),
  (666,'2008-03-26 22:12:32',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',86),
  (667,'2008-03-26 22:12:32',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',87),
  (668,'2008-03-26 22:12:32',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',88),
  (669,'2008-03-26 22:12:32',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',89),
  (670,'2008-03-26 22:12:32',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',90),
  (671,'2008-03-26 22:12:32',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',91),
  (672,'2008-03-26 22:12:32',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',92),
  (673,'2008-03-26 22:12:32',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',93),
  (674,'2008-03-26 22:12:32',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',94),
  (675,'2008-03-26 22:12:32',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',95),
  (676,'2008-03-26 22:12:33',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',96),
  (677,'2008-03-26 22:12:33',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',97),
  (678,'2008-03-26 22:12:33',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',98),
  (679,'2008-03-26 22:12:33',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',99),
  (680,'2008-03-26 22:12:33',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',100),
  (681,'2008-03-26 22:23:59',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',21),
  (682,'2008-03-26 22:23:59',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',22),
  (683,'2008-03-26 22:23:59',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',23),
  (684,'2008-03-26 22:23:59',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',24),
  (685,'2008-03-26 22:24:00',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',25),
  (686,'2008-03-26 22:24:00',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',26),
  (687,'2008-03-26 22:24:00',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',27),
  (688,'2008-03-26 22:24:00',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',28),
  (689,'2008-03-26 22:24:00',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',29),
  (690,'2008-03-26 22:24:00',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',30),
  (691,'2008-03-26 22:27:12',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',31),
  (692,'2008-03-26 22:27:12',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',32),
  (693,'2008-03-26 22:27:12',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',33),
  (694,'2008-03-26 22:27:12',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',34),
  (695,'2008-03-26 22:27:12',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',35),
  (696,'2008-03-26 22:27:12',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',36),
  (697,'2008-03-26 22:27:12',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',37),
  (698,'2008-03-26 22:27:12',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',38),
  (699,'2008-03-26 22:27:12',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',39),
  (700,'2008-03-26 22:27:12',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',40),
  (701,'2008-03-26 22:27:12',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',41),
  (702,'2008-03-26 22:27:12',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',42),
  (703,'2008-03-26 22:27:12',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',43),
  (704,'2008-03-26 22:27:12',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',44),
  (705,'2008-03-26 22:27:12',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',45),
  (706,'2008-03-26 22:27:12',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',46),
  (707,'2008-03-26 22:27:12',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',47),
  (708,'2008-03-26 22:27:12',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',48),
  (709,'2008-03-26 22:27:12',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',49),
  (710,'2008-03-26 22:27:12',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',50),
  (711,'2008-03-26 22:27:12',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',51),
  (712,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',52),
  (713,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',53),
  (714,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',54),
  (715,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',55),
  (716,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',56),
  (717,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',57),
  (718,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',58),
  (719,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',59),
  (720,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',60),
  (721,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',61),
  (722,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',62),
  (723,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',63),
  (724,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',64),
  (725,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',65),
  (726,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',66),
  (727,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',67),
  (728,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',68),
  (729,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',69),
  (730,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',70),
  (731,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',71),
  (732,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',72),
  (733,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',73),
  (734,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',74),
  (735,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',75),
  (736,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',76),
  (737,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',77),
  (738,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',78),
  (739,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',79),
  (740,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',80),
  (741,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',81),
  (742,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',82),
  (743,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',83),
  (744,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',84),
  (745,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',85),
  (746,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',86),
  (747,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',87),
  (748,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',88),
  (749,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',89),
  (750,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',90),
  (751,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',91),
  (752,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',92),
  (753,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',93),
  (754,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',94),
  (755,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',95),
  (756,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',96),
  (757,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',97),
  (758,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',98),
  (759,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',99),
  (760,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',100),
  (761,'2008-03-27 14:39:25',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',2),
  (762,'2008-03-27 14:55:53',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',3),
  (763,'2008-03-27 14:57:17',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',4),
  (764,'2008-03-27 14:57:17',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',5),
  (765,'2008-03-27 14:57:17',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',6),
  (766,'2008-03-27 14:57:17',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',7),
  (767,'2008-03-27 14:57:17',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',8),
  (768,'2008-03-27 14:57:17',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',9),
  (769,'2008-03-27 14:57:17',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',10),
  (770,'2008-03-27 14:57:17',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',11),
  (771,'2008-03-27 14:57:17',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',12),
  (772,'2008-03-27 14:57:17',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',13),
  (773,'2008-03-27 14:57:17',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',14),
  (774,'2008-03-27 14:57:17',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',15),
  (775,'2008-03-27 14:57:17',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',16),
  (776,'2008-03-27 14:57:17',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',17),
  (777,'2008-03-27 14:57:17',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',18),
  (778,'2008-03-27 14:57:17',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',19),
  (779,'2008-03-27 14:57:17',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',20),
  (780,'2008-03-27 14:57:17',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',21),
  (781,'2008-03-27 14:57:17',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',22),
  (782,'2008-03-27 14:57:17',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',23),
  (783,'2008-03-27 14:57:17',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',24),
  (784,'2008-03-27 14:57:17',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',25),
  (785,'2008-03-27 14:57:17',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',26),
  (786,'2008-03-27 14:57:17',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',27),
  (787,'2008-03-27 14:57:17',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',28),
  (788,'2008-03-27 14:57:17',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',29),
  (789,'2008-03-27 14:57:17',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',30),
  (790,'2008-03-27 14:57:18',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',31),
  (791,'2008-03-27 14:57:18',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',32),
  (792,'2008-03-27 14:57:18',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',33),
  (793,'2008-03-27 14:57:18',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',34),
  (794,'2008-03-27 14:57:18',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',35),
  (795,'2008-03-27 14:57:18',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',36),
  (796,'2008-03-27 14:57:18',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',37),
  (797,'2008-03-27 14:57:18',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',38),
  (798,'2008-03-27 14:57:18',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',39),
  (799,'2008-03-27 14:57:18',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',40),
  (800,'2008-03-27 14:57:18',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',41),
  (801,'2008-03-27 14:57:18',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',42),
  (802,'2008-03-27 14:57:18',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',43),
  (803,'2008-03-27 14:57:18',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',44),
  (804,'2008-03-27 14:57:18',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',45),
  (805,'2008-03-27 14:57:18',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',46),
  (806,'2008-03-27 14:57:18',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',47),
  (807,'2008-03-27 14:57:18',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',48),
  (808,'2008-03-27 14:57:18',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',49),
  (809,'2008-03-27 14:57:18',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',50),
  (810,'2008-03-27 14:57:18',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',51),
  (811,'2008-03-27 14:57:18',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',52),
  (812,'2008-03-27 14:57:18',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',53),
  (813,'2008-03-27 14:57:18',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',54),
  (814,'2008-03-27 14:57:18',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',55),
  (815,'2008-03-27 14:57:18',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',56),
  (816,'2008-03-27 14:57:18',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',57),
  (817,'2008-03-27 14:57:18',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',58),
  (818,'2008-03-27 14:57:18',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',59),
  (819,'2008-03-27 14:57:18',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',60),
  (820,'2008-03-27 14:57:18',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',61),
  (821,'2008-03-27 14:57:18',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',62),
  (822,'2008-03-27 14:57:18',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',63),
  (823,'2008-03-27 14:57:18',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',64),
  (824,'2008-03-27 14:57:18',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',65),
  (825,'2008-03-27 14:57:18',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',66),
  (826,'2008-03-27 14:57:18',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',67),
  (827,'2008-03-27 14:57:18',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',68),
  (828,'2008-03-27 14:57:18',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',69),
  (829,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',2),
  (830,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',3),
  (831,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',4),
  (832,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',5),
  (833,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',6),
  (834,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',7),
  (835,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',8),
  (836,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',9),
  (837,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',10),
  (838,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',11),
  (839,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',12),
  (840,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',13),
  (841,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',14),
  (842,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',15),
  (843,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',16),
  (844,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',17),
  (845,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',18),
  (846,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',19),
  (847,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',20),
  (848,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',21),
  (849,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',22),
  (850,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',23),
  (851,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',24),
  (852,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',25),
  (853,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',26),
  (854,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',27),
  (855,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',28),
  (856,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',29),
  (857,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',30),
  (858,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',31),
  (859,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',32),
  (860,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',33),
  (861,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',34),
  (862,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',35),
  (863,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',36),
  (864,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',37),
  (865,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',38),
  (866,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',39),
  (867,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',40),
  (868,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',41),
  (869,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',42),
  (870,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',43),
  (871,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',44),
  (872,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',45),
  (873,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',46),
  (874,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',47),
  (875,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',48),
  (876,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',49),
  (877,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',50),
  (878,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',51),
  (879,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',52),
  (880,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',53),
  (881,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',54),
  (882,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',55),
  (883,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',56),
  (884,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',57),
  (885,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',58),
  (886,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',59),
  (887,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',60),
  (888,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',61),
  (889,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',62),
  (890,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',63),
  (891,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',64),
  (892,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',65),
  (893,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',66),
  (894,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',67),
  (895,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',68),
  (896,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',69),
  (897,'2008-03-27 16:05:12',0,'192.168.20.198',2,'{05000000037A941600142AA7AB810000}',2),
  (898,'2008-03-27 16:05:12',0,'192.168.20.198',2,'{05000000037A941600142AA7AB810000}',3),
  (899,'2008-03-27 16:05:12',0,'192.168.20.198',2,'{05000000037A941600142AA7AB810000}',4),
  (900,'2008-03-27 16:05:12',0,'192.168.20.198',2,'{05000000037A941600142AA7AB810000}',5),
  (901,'2008-03-27 16:05:12',0,'192.168.20.198',2,'{05000000037A941600142AA7AB810000}',6),
  (902,'2008-03-27 16:05:12',0,'192.168.20.198',2,'{05000000037A941600142AA7AB810000}',7),
  (903,'2008-03-27 16:05:12',0,'192.168.20.198',2,'{05000000037A941600142AA7AB810000}',8),
  (904,'2008-03-27 16:05:12',0,'192.168.20.198',2,'{05000000037A941600142AA7AB810000}',9),
  (905,'2008-03-27 16:05:12',0,'192.168.20.198',2,'{05000000037A941600142AA7AB810000}',10),
  (906,'2008-03-27 16:05:12',0,'192.168.20.198',2,'{05000000037A941600142AA7AB810000}',11),
  (907,'2008-03-27 16:06:08',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',2),
  (908,'2008-03-27 16:08:14',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',3),
  (909,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',4),
  (910,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',5),
  (911,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',6),
  (912,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',7),
  (913,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',8),
  (914,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',9),
  (915,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',10),
  (916,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',11),
  (917,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',12),
  (918,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',13),
  (919,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',14),
  (920,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',15),
  (921,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',16),
  (922,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',17),
  (923,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',18),
  (924,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',19),
  (925,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',20),
  (926,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',21),
  (927,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',22),
  (928,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',23),
  (929,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',24),
  (930,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',25),
  (931,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',26),
  (932,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',27),
  (933,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',28),
  (934,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',29),
  (935,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',30),
  (936,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',31),
  (937,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',32),
  (938,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',33),
  (939,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',34),
  (940,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',35),
  (941,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',36),
  (942,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',37),
  (943,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',38),
  (944,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',39),
  (945,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',40),
  (946,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',41),
  (947,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',42),
  (948,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',43),
  (949,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',44),
  (950,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',45),
  (951,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',46),
  (952,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',47),
  (953,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',48),
  (954,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',49),
  (955,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',50),
  (956,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',51),
  (957,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',52),
  (958,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',53),
  (959,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',54),
  (960,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',55),
  (961,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',56),
  (962,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',57),
  (963,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',58),
  (964,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',59),
  (965,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',60),
  (966,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',61),
  (967,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',62),
  (968,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',63),
  (969,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',64),
  (970,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',65),
  (971,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',66),
  (972,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',67),
  (973,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',68),
  (974,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',69);

COMMIT;

#
# Data for the `log_3_2` table  (LIMIT 0,500)
#

INSERT INTO `log_3_2` (`ID`, `LogTime`, `Source`, `Ip`, `Level`, `Field1`, `Field2`, `Field3`) VALUES 
  (1,'2008-03-26 17:14:44',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',100,2),
  (2,'2008-03-26 18:00:17',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',100,1),
  (3,'2008-03-26 22:00:32',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',20,3),
  (4,'2008-03-26 22:12:39',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',100,3),
  (5,'2008-03-27 16:50:57',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',69,2);

COMMIT;

#
# Data for the `log_3_3` table  (LIMIT 0,500)
#

INSERT INTO `log_3_3` (`ID`, `LogTime`, `Source`, `Ip`, `Level`, `Field1`, `Field2`, `Field3`, `Field4`) VALUES 
  (1,'2008-03-26 10:20:43',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',60,0,NULL),
  (2,'2008-03-26 17:10:11',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',60,0,NULL),
  (3,'2008-03-26 17:58:34',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',60,0,NULL),
  (4,'2008-03-26 20:06:24',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',60,0,NULL),
  (5,'2008-03-26 21:18:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',60,0,NULL),
  (6,'2008-03-26 22:12:31',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',60,0,NULL),
  (7,'2008-03-26 22:27:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',60,3,NULL),
  (8,'2008-03-27 14:57:18',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',60,0,NULL),
  (9,'2008-03-27 15:24:21',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',60,0,NULL),
  (10,'2008-03-27 16:44:43',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',60,0,NULL);

COMMIT;

#
# Data for the `log_3_4` table  (LIMIT 0,500)
#

INSERT INTO `log_3_4` (`ID`, `LogTime`, `Source`, `Ip`, `Level`, `Field1`, `Field2`, `Field3`) VALUES 
  (1,'2008-03-26 22:35:55',3,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',100,3717858995);

COMMIT;

#
# Data for the `log_3_6` table  (LIMIT 0,500)
#

INSERT INTO `log_3_6` (`ID`, `LogTime`, `Source`, `Ip`, `Level`, `Field1`, `Field2`, `Field3`, `Field4`, `Field5`) VALUES 
  (1,'2008-03-26 19:52:36',0,'192.168.20.198',2,'{010000003DA5D05C00142AA7AB810000}',157,306,'【身世之迷】','雏凤清鸣'),
  (2,'2008-03-26 22:40:29',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',157,306,'【身世之迷】','雏凤清鸣'),
  (3,'2008-03-27 16:08:14',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',157,306,'【身世之迷】','雏凤清鸣');

COMMIT;

#
# Data for the `log_3_7` table  (LIMIT 0,500)
#

INSERT INTO `log_3_7` (`ID`, `LogTime`, `Source`, `Ip`, `Level`, `Field1`, `Field2`, `Field3`) VALUES 
  (1,'2008-03-26 10:20:29',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',1,'192.168.20.206:2567'),
  (2,'2008-03-26 10:20:52',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',0,NULL),
  (3,'2008-03-26 10:20:52',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',1,'192.168.20.206:3079'),
  (4,'2008-03-26 10:22:51',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',0,NULL),
  (5,'2008-03-26 10:34:31',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',1,'192.168.20.206:19719'),
  (6,'2008-03-26 10:47:06',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',0,NULL),
  (7,'2008-03-26 10:52:25',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',1,'192.168.20.206:47367'),
  (8,'2008-03-26 11:12:05',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',1,'192.168.20.206:28680'),
  (9,'2008-03-26 11:19:28',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',1,'192.168.20.206:41480'),
  (10,'2008-03-26 11:24:14',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',1,'192.168.20.206:47112'),
  (11,'2008-03-26 11:29:56',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',1,'192.168.20.206:63496'),
  (12,'2008-03-26 11:33:38',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',1,'192.168.20.206:64520'),
  (13,'2008-03-26 11:40:20',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',1,'192.168.20.206:5897'),
  (14,'2008-03-26 11:58:45',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',0,NULL),
  (15,'2008-03-26 12:39:07',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',1,'192.168.20.206:41226'),
  (16,'2008-03-26 12:57:07',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',0,NULL),
  (17,'2008-03-26 14:20:11',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',1,'192.168.20.206:18701'),
  (18,'2008-03-26 14:31:50',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',0,NULL),
  (19,'2008-03-26 15:20:32',0,'192.168.20.198',2,'{010000002F2EA97400142AA7AB810000}',1,'192.168.20.146:30278'),
  (20,'2008-03-26 15:44:05',0,'192.168.20.198',2,'{02000000ED9748EA00142AA7AB810000}',1,'192.168.20.169:3344'),
  (21,'2008-03-26 15:47:59',0,'192.168.20.198',2,'{02000000ED9748EA00142AA7AB810000}',0,NULL),
  (22,'2008-03-26 15:48:11',0,'192.168.20.198',2,'{02000000ED9748EA00142AA7AB810000}',1,'192.168.20.169:13072'),
  (23,'2008-03-26 15:48:49',0,'192.168.20.198',2,'{02000000ED9748EA00142AA7AB810000}',0,NULL),
  (24,'2008-03-26 15:49:48',0,'192.168.20.198',2,'{02000000ED9748EA00142AA7AB810000}',1,'192.168.20.169:15376'),
  (25,'2008-03-26 15:56:50',0,'192.168.20.198',2,'{02000000ED9748EA00142AA7AB810000}',0,NULL),
  (26,'2008-03-26 16:00:40',0,'192.168.20.198',2,'{03000000FC816496000AEBF4783E0000}',1,'192.168.20.169:27920'),
  (27,'2008-03-26 16:13:49',0,'192.168.20.198',2,'{03000000FC816496000AEBF4783E0000}',0,NULL),
  (28,'2008-03-26 16:20:18',0,'192.168.20.198',2,'{03000000FC816496000AEBF4783E0000}',1,'192.168.20.169:4369'),
  (29,'2008-03-26 16:31:55',0,'192.168.20.198',2,'{010000002F2EA97400142AA7AB810000}',0,NULL),
  (30,'2008-03-26 16:53:24',0,'192.168.20.198',2,'{03000000FC816496000AEBF4783E0000}',0,NULL),
  (31,'2008-03-26 16:54:17',0,'192.168.20.198',2,'{01000000647890BE00142AA7AB810000}',1,'192.168.20.169:57106'),
  (32,'2008-03-26 16:59:39',0,'192.168.20.198',2,'{01000000647890BE00142AA7AB810000}',0,NULL),
  (33,'2008-03-26 17:00:25',0,'192.168.20.198',2,'{05000000A60A41D1000AEB2351BE0000}',1,'192.168.20.146:25161'),
  (34,'2008-03-26 17:01:42',0,'192.168.20.198',2,'{01000000647890BE00142AA7AB810000}',1,'192.168.20.169:6675'),
  (35,'2008-03-26 17:08:40',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',1,'192.168.20.182:15438'),
  (36,'2008-03-26 17:08:41',0,'192.168.20.198',2,'{05000000A60A41D1000AEB2351BE0000}',0,NULL),
  (37,'2008-03-26 17:11:14',0,'192.168.20.198',2,'{01000000647890BE00142AA7AB810000}',0,NULL),
  (38,'2008-03-26 17:18:54',0,'192.168.20.198',2,'{0500000020A1FAB1000AEBF4783E0000}',0,NULL),
  (39,'2008-03-26 17:40:42',0,'192.168.20.198',2,'{01000000647890BE00142AA7AB810000}',1,'192.168.20.169:40196'),
  (40,'2008-03-26 17:50:05',0,'192.168.20.198',2,'{01000000647890BE00142AA7AB810000}',0,NULL),
  (41,'2008-03-26 17:50:10',0,'192.168.20.198',2,'{030000008147D23800142AA7AB810000}',1,'192.168.20.74:29707'),
  (42,'2008-03-26 17:53:42',0,'192.168.20.198',2,'{0A000000BE768BD300142AA7AB810000}',1,'192.168.20.206:4883'),
  (43,'2008-03-26 17:53:55',0,'192.168.20.198',2,'{0A000000BE768BD300142AA7AB810000}',0,NULL),
  (44,'2008-03-26 17:54:37',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',1,'192.168.20.169:57860'),
  (45,'2008-03-26 17:54:42',0,'192.168.20.198',2,'{030000008147D23800142AA7AB810000}',0,NULL),
  (46,'2008-03-26 17:54:47',0,'192.168.20.198',2,'{0A000000BE768BD300142AA7AB810000}',1,'192.168.20.206:6419'),
  (47,'2008-03-26 17:54:51',0,'192.168.20.198',2,'{0A000000BE768BD300142AA7AB810000}',0,NULL),
  (48,'2008-03-26 17:55:36',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',0,NULL),
  (49,'2008-03-26 17:56:07',0,'192.168.20.198',2,'{0A000000BE768BD300142AA7AB810000}',1,'192.168.20.206:6931'),
  (50,'2008-03-26 17:56:11',0,'192.168.20.198',2,'{0A000000BE768BD300142AA7AB810000}',0,NULL),
  (51,'2008-03-26 17:56:22',0,'192.168.20.198',2,'{0A000000BE768BD300142AA7AB810000}',1,'192.168.20.206:7443'),
  (52,'2008-03-26 17:57:02',0,'192.168.20.198',2,'{0A000000BE768BD300142AA7AB810000}',0,NULL),
  (53,'2008-03-26 17:57:11',0,'192.168.20.198',2,'{0A000000BE768BD300142AA7AB810000}',1,'192.168.20.206:7955'),
  (54,'2008-03-26 17:58:26',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',1,'192.168.20.169:65028'),
  (55,'2008-03-26 17:58:40',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',0,NULL),
  (56,'2008-03-26 17:59:59',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',1,'192.168.20.169:2821'),
  (57,'2008-03-26 18:00:18',0,'192.168.20.198',2,'{04000000400C310700142AA7AB810000}',1,'192.168.20.149:34067'),
  (58,'2008-03-26 18:00:20',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',0,NULL),
  (59,'2008-03-26 18:00:22',0,'192.168.20.198',2,'{0A000000BE768BD300142AA7AB810000}',0,NULL),
  (60,'2008-03-26 18:01:09',0,'192.168.20.198',2,'{04000000400C310700142AA7AB810000}',0,NULL),
  (61,'2008-03-26 18:02:20',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',1,'192.168.20.169:6917'),
  (62,'2008-03-26 18:05:14',0,'192.168.20.198',2,'{02000000BAB0CE65000AEBF4783E0000}',1,'192.168.20.169:9733'),
  (63,'2008-03-26 18:08:12',0,'192.168.20.198',2,'{02000000BAB0CE65000AEBF4783E0000}',0,NULL),
  (64,'2008-03-26 18:08:15',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',0,NULL),
  (65,'2008-03-26 18:10:23',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',1,'192.168.20.169:18181'),
  (66,'2008-03-26 18:12:02',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',0,NULL),
  (67,'2008-03-26 18:17:51',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',1,'192.168.20.169:25349'),
  (68,'2008-03-26 18:19:35',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',0,NULL),
  (69,'2008-03-26 18:20:09',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',1,'192.168.20.169:27397'),
  (70,'2008-03-26 18:21:55',0,'192.168.20.198',2,'{0A000000BE768BD300142AA7AB810000}',1,'192.168.20.206:29203'),
  (71,'2008-03-26 18:22:23',0,'192.168.20.198',2,'{0A000000BE768BD300142AA7AB810000}',0,NULL),
  (72,'2008-03-26 18:24:05',0,'192.168.20.198',2,'{0A000000BE768BD300142AA7AB810000}',1,'192.168.20.206:1284'),
  (73,'2008-03-26 18:24:41',0,'192.168.20.198',2,'{0A000000BE768BD300142AA7AB810000}',0,NULL),
  (74,'2008-03-26 18:25:28',0,'192.168.20.198',2,'{0A000000BE768BD300142AA7AB810000}',1,'192.168.20.206:4612'),
  (75,'2008-03-26 18:25:36',0,'192.168.20.198',2,'{0A000000BE768BD300142AA7AB810000}',0,NULL),
  (76,'2008-03-26 18:26:04',0,'192.168.20.198',2,'{0A000000BE768BD300142AA7AB810000}',1,'192.168.20.206:5124'),
  (77,'2008-03-26 18:29:15',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',1,'192.168.20.169:44549'),
  (78,'2008-03-26 18:29:27',0,'192.168.20.198',2,'{0B000000E9EF985C00142AA7AB810000}',0,NULL),
  (79,'2008-03-26 18:29:27',0,'192.168.20.198',2,'{0A000000BE768BD300142AA7AB810000}',1,'192.168.20.206:9476'),
  (80,'2008-03-26 18:30:26',0,'192.168.20.198',2,'{0A000000BE768BD300142AA7AB810000}',0,NULL),
  (81,'2008-03-26 18:31:06',0,'192.168.20.198',2,'{0A000000BE768BD300142AA7AB810000}',1,'192.168.20.206:15876'),
  (82,'2008-03-26 18:31:18',0,'192.168.20.198',2,'{0A000000BE768BD300142AA7AB810000}',0,NULL),
  (83,'2008-03-26 18:31:38',0,'192.168.20.198',2,'{02000000BAB0CE65000AEBF4783E0000}',1,'192.168.20.169:45061'),
  (84,'2008-03-26 18:31:46',0,'192.168.20.198',2,'{0A000000BE768BD300142AA7AB810000}',1,'192.168.20.206:16388'),
  (85,'2008-03-26 18:32:38',0,'192.168.20.198',2,'{0A000000BE768BD300142AA7AB810000}',0,NULL),
  (86,'2008-03-26 18:35:11',0,'192.168.20.198',2,'{0A000000BE768BD300142AA7AB810000}',1,'192.168.20.206:16900'),
  (87,'2008-03-26 18:35:18',0,'192.168.20.198',2,'{0A000000BE768BD300142AA7AB810000}',0,NULL),
  (88,'2008-03-26 18:35:52',0,'192.168.20.198',2,'{0A000000BE768BD300142AA7AB810000}',1,'192.168.20.206:17412'),
  (89,'2008-03-26 18:37:22',0,'192.168.20.198',2,'{0A000000BE768BD300142AA7AB810000}',0,NULL),
  (90,'2008-03-26 18:38:42',0,'192.168.20.198',2,'{02000000BAB0CE65000AEBF4783E0000}',0,NULL),
  (91,'2008-03-26 18:39:16',0,'192.168.20.198',2,'{0E000000337EDB6400142AA7AB810000}',1,'192.168.20.169:51461'),
  (92,'2008-03-26 18:39:42',0,'192.168.20.198',2,'{0A000000BE768BD300142AA7AB810000}',1,'192.168.20.206:23812'),
  (93,'2008-03-26 18:39:46',0,'192.168.20.198',2,'{0A000000BE768BD300142AA7AB810000}',0,NULL),
  (94,'2008-03-26 18:49:39',0,'192.168.20.198',2,'{0A000000BE768BD300142AA7AB810000}',1,'192.168.20.206:35588'),
  (95,'2008-03-26 18:49:43',0,'192.168.20.198',2,'{0A000000BE768BD300142AA7AB810000}',0,NULL),
  (96,'2008-03-26 19:00:59',0,'192.168.20.198',2,'{0A000000BE768BD300142AA7AB810000}',1,'192.168.20.206:15365'),
  (97,'2008-03-26 19:01:02',0,'192.168.20.198',2,'{0A000000BE768BD300142AA7AB810000}',0,NULL),
  (98,'2008-03-26 19:04:15',0,'192.168.20.198',2,'{0A000000BE768BD300142AA7AB810000}',1,'192.168.20.206:17925'),
  (99,'2008-03-26 19:04:19',0,'192.168.20.198',2,'{0A000000BE768BD300142AA7AB810000}',0,NULL),
  (100,'2008-03-26 19:06:13',0,'192.168.20.198',2,'{0A000000BE768BD300142AA7AB810000}',1,'192.168.20.206:26885'),
  (101,'2008-03-26 19:06:19',0,'192.168.20.198',2,'{0A000000BE768BD300142AA7AB810000}',0,NULL),
  (102,'2008-03-26 19:07:10',0,'192.168.20.198',2,'{0E000000337EDB6400142AA7AB810000}',0,NULL),
  (103,'2008-03-26 19:07:13',0,'192.168.20.198',2,'{0A000000BE768BD300142AA7AB810000}',1,'192.168.20.206:27397'),
  (104,'2008-03-26 19:08:18',0,'192.168.20.198',2,'{0A000000BE768BD300142AA7AB810000}',0,NULL),
  (105,'2008-03-26 19:13:14',0,'192.168.20.198',2,'{0A000000BE768BD300142AA7AB810000}',1,'192.168.20.206:33797'),
  (106,'2008-03-26 19:13:18',0,'192.168.20.198',2,'{0A000000BE768BD300142AA7AB810000}',0,NULL),
  (107,'2008-03-26 19:14:18',0,'192.168.20.198',2,'{0A000000BE768BD300142AA7AB810000}',1,'192.168.20.206:34309'),
  (108,'2008-03-26 19:14:22',0,'192.168.20.198',2,'{0A000000BE768BD300142AA7AB810000}',0,NULL),
  (109,'2008-03-26 19:48:24',0,'192.168.20.198',2,'{010000003DA5D05C00142AA7AB810000}',1,'192.168.20.169:62726'),
  (110,'2008-03-26 19:59:15',0,'192.168.20.198',2,'{010000003DA5D05C00142AA7AB810000}',0,NULL),
  (111,'2008-03-26 19:59:21',0,'192.168.20.198',2,'{0A000000BE768BD300142AA7AB810000}',1,'192.168.20.206:52742'),
  (112,'2008-03-26 19:59:45',0,'192.168.20.198',2,'{0A000000BE768BD300142AA7AB810000}',0,NULL),
  (113,'2008-03-26 20:00:29',0,'192.168.20.198',2,'{030000009F25AA1100142AA7AB810000}',1,'192.168.20.74:18958'),
  (114,'2008-03-26 20:00:50',0,'192.168.20.198',2,'{030000009F25AA1100142AA7AB810000}',0,NULL),
  (115,'2008-03-26 20:00:51',0,'192.168.20.198',2,'{04000000E669036E00142AA7AB810000}',1,'192.168.20.149:40457'),
  (116,'2008-03-26 20:02:05',0,'192.168.20.198',2,'{04000000E669036E00142AA7AB810000}',0,NULL),
  (117,'2008-03-26 20:05:11',0,'192.168.20.198',2,'{010000003DA5D05C00142AA7AB810000}',1,'192.168.20.169:29959'),
  (118,'2008-03-26 20:05:52',0,'192.168.20.198',2,'{010000003DA5D05C00142AA7AB810000}',0,NULL),
  (119,'2008-03-26 20:06:13',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',1,'192.168.20.146:29004'),
  (120,'2008-03-26 20:06:16',0,'192.168.20.198',2,'{04000000E669036E00142AA7AB810000}',1,'192.168.20.149:48393'),
  (121,'2008-03-26 20:06:27',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',0,NULL),
  (122,'2008-03-26 20:06:54',0,'192.168.20.198',2,'{04000000E669036E00142AA7AB810000}',0,NULL),
  (123,'2008-03-26 20:07:32',0,'192.168.20.198',2,'{0C0000006E8E2AEF00142AA7AB810000}',1,'192.168.20.149:49929'),
  (124,'2008-03-26 20:07:51',0,'192.168.20.198',2,'{0C0000006E8E2AEF00142AA7AB810000}',0,NULL),
  (125,'2008-03-26 20:10:57',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',1,'192.168.20.146:32588'),
  (126,'2008-03-26 20:19:16',0,'192.168.20.198',2,'{0B000000C74EBF4400142AA7AB810000}',0,NULL),
  (127,'2008-03-26 20:19:59',0,'192.168.20.198',2,'{010000003DA5D05C00142AA7AB810000}',1,'192.168.20.169:44551'),
  (128,'2008-03-26 20:31:53',0,'192.168.20.198',2,'{0D000000792BC62B00142AA7AB810000}',1,'192.168.20.74:34574'),
  (129,'2008-03-26 20:32:21',0,'192.168.20.198',2,'{0D000000792BC62B00142AA7AB810000}',0,NULL),
  (130,'2008-03-26 21:17:07',0,'192.168.20.198',2,'{020000003393F566000AEB2351BE0000}',1,'192.168.20.146:22349'),
  (131,'2008-03-26 21:17:47',0,'192.168.20.198',2,'{020000003393F566000AEB2351BE0000}',0,NULL),
  (132,'2008-03-26 21:18:04',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',1,'192.168.20.146:23629'),
  (133,'2008-03-26 21:18:18',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',0,NULL),
  (134,'2008-03-26 21:19:15',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',1,'192.168.20.146:25933'),
  (135,'2008-03-26 21:21:21',0,'192.168.20.198',2,'{020000004F4D764500142AA7AB810000}',1,'192.168.20.149:51469'),
  (136,'2008-03-26 21:22:02',0,'192.168.20.198',2,'{020000004F4D764500142AA7AB810000}',0,NULL),
  (137,'2008-03-26 21:22:35',0,'192.168.20.198',2,'{03000000D6B2398400142AA7AB810000}',1,'192.168.20.149:53005'),
  (138,'2008-03-26 21:23:09',0,'192.168.20.198',2,'{010000007CC0C17B00142AA7AB810000}',0,NULL),
  (139,'2008-03-26 21:31:59',0,'192.168.20.198',2,'{04000000415E7CDF00142AA7AB810000}',1,'192.168.20.74:6419'),
  (140,'2008-03-26 21:39:21',0,'192.168.20.198',2,'{04000000415E7CDF00142AA7AB810000}',0,NULL),
  (141,'2008-03-26 21:52:23',0,'192.168.20.198',2,'{010000003DA5D05C00142AA7AB810000}',1,'192.168.20.169:35081'),
  (142,'2008-03-26 21:56:21',0,'192.168.20.198',2,'{010000003DA5D05C00142AA7AB810000}',0,NULL),
  (143,'2008-03-26 21:56:40',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',1,'192.168.20.169:45065'),
  (144,'2008-03-26 22:00:35',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',0,NULL),
  (145,'2008-03-26 22:10:09',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',1,'192.168.20.169:14858'),
  (146,'2008-03-26 22:12:23',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',1,'192.168.20.169:15882'),
  (147,'2008-03-26 22:15:25',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',0,NULL),
  (148,'2008-03-26 22:20:09',0,'192.168.20.198',2,'{010000003DA5D05C00142AA7AB810000}',1,'192.168.20.169:25610'),
  (149,'2008-03-26 22:22:31',0,'192.168.20.198',2,'{010000003DA5D05C00142AA7AB810000}',0,NULL),
  (150,'2008-03-26 22:22:37',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',0,NULL),
  (151,'2008-03-26 22:22:51',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',1,'192.168.20.169:30218'),
  (152,'2008-03-26 22:24:11',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',0,NULL),
  (153,'2008-03-26 22:25:27',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',1,'192.168.20.169:34570'),
  (154,'2008-03-26 22:27:44',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',0,NULL),
  (155,'2008-03-26 22:27:52',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',1,'192.168.20.169:36106'),
  (156,'2008-03-26 22:48:59',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',0,NULL),
  (157,'2008-03-26 22:59:33',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',1,'192.168.20.169:8715'),
  (158,'2008-03-26 23:05:58',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',0,NULL),
  (159,'2008-03-26 23:15:13',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',1,'192.168.20.169:28939'),
  (160,'2008-03-26 23:15:19',0,'192.168.20.198',2,'{0500000000C87D8500142AA7AB810000}',0,NULL),
  (161,'2008-03-26 23:15:29',0,'192.168.20.198',2,'{010000003DA5D05C00142AA7AB810000}',1,'192.168.20.169:29451'),
  (162,'2008-03-26 23:20:49',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',1,'192.168.20.169:34059'),
  (163,'2008-03-26 23:23:01',0,'192.168.20.198',2,'{07000000E91355A3000AEBF4783E0000}',0,NULL),
  (164,'2008-03-26 23:23:04',0,'192.168.20.198',2,'{010000003DA5D05C00142AA7AB810000}',0,NULL),
  (165,'2008-03-27 10:59:34',0,'192.168.20.198',2,'{06000000C346CD3000142AA7AB810000}',1,'192.168.20.182:33626'),
  (166,'2008-03-27 11:01:06',0,'192.168.20.198',2,'{06000000C346CD3000142AA7AB810000}',0,NULL),
  (167,'2008-03-27 11:15:08',0,'192.168.20.198',2,'{0A000000BE768BD300142AA7AB810000}',1,'192.168.20.206:3595'),
  (168,'2008-03-27 11:16:12',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',1,'192.168.20.206:9227'),
  (169,'2008-03-27 11:17:03',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',0,NULL),
  (170,'2008-03-27 11:17:11',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',1,'192.168.20.206:10251'),
  (171,'2008-03-27 11:24:50',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',0,NULL),
  (172,'2008-03-27 11:59:59',0,'192.168.20.198',2,'{04000000415E7CDF00142AA7AB810000}',1,'192.168.20.74:6161'),
  (173,'2008-03-27 12:25:12',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',1,'192.168.20.206:18444'),
  (174,'2008-03-27 12:31:01',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',0,NULL),
  (175,'2008-03-27 12:31:10',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',1,'192.168.20.206:54796'),
  (176,'2008-03-27 12:31:39',0,'192.168.20.198',2,'{070000007A96D3C800142AA7AB810000}',1,'192.168.20.74:18962'),
  (177,'2008-03-27 12:33:01',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',0,NULL),
  (178,'2008-03-27 12:33:10',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',1,'192.168.20.206:65036'),
  (179,'2008-03-27 12:33:29',0,'192.168.20.198',2,'{06000000C346CD3000142AA7AB810000}',1,'192.168.20.182:56411'),
  (180,'2008-03-27 12:36:33',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',0,NULL),
  (181,'2008-03-27 12:38:38',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',1,'192.168.20.206:10765'),
  (182,'2008-03-27 12:38:55',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',0,NULL),
  (183,'2008-03-27 12:39:07',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',1,'192.168.20.206:11277'),
  (184,'2008-03-27 12:59:21',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',0,NULL),
  (185,'2008-03-27 13:44:27',0,'192.168.20.198',2,'{02000000166525F700142AA7AB810000}',1,'192.168.20.149:56082'),
  (186,'2008-03-27 13:45:01',0,'192.168.20.198',2,'{02000000166525F700142AA7AB810000}',0,NULL),
  (187,'2008-03-27 14:05:43',0,'192.168.20.198',2,'{080000005543129D00142AA7AB810000}',1,'192.168.20.74:17414'),
  (188,'2008-03-27 14:09:03',0,'192.168.20.198',2,'{03000000D6B2398400142AA7AB810000}',1,'192.168.20.149:58117'),
  (189,'2008-03-27 14:10:19',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',1,'192.168.20.206:37648'),
  (190,'2008-03-27 14:10:29',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',1,'192.168.20.149:61445'),
  (191,'2008-03-27 14:10:29',0,'192.168.20.198',2,'{0A0000007774D27300142AA7AB810000}',1,'192.168.20.74:25862'),
  (192,'2008-03-27 14:11:25',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',0,NULL),
  (193,'2008-03-27 14:12:23',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',1,'192.168.20.149:6406'),
  (194,'2008-03-27 14:14:45',0,'192.168.20.198',2,'{0A0000007774D27300142AA7AB810000}',1,'192.168.20.74:34054'),
  (195,'2008-03-27 14:17:10',0,'192.168.20.198',2,'{0A0000007774D27300142AA7AB810000}',0,NULL),
  (196,'2008-03-27 14:20:03',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',1,'192.168.20.206:55056'),
  (197,'2008-03-27 14:23:09',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',0,NULL),
  (198,'2008-03-27 14:23:23',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',1,'192.168.20.206:59920'),
  (199,'2008-03-27 14:24:52',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',1,'192.168.20.149:45318'),
  (200,'2008-03-27 14:39:15',0,'192.168.20.198',2,'{01000000C42D4EFC000AEBF4783E0000}',1,'192.168.20.206:41489'),
  (201,'2008-03-27 14:39:56',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',0,NULL),
  (202,'2008-03-27 14:40:05',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',1,'192.168.20.206:42001'),
  (203,'2008-03-27 14:51:26',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',0,NULL),
  (204,'2008-03-27 14:51:54',0,'192.168.20.198',2,'{01000000C42D4EFC000AEBF4783E0000}',0,NULL),
  (205,'2008-03-27 14:51:59',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',0,NULL),
  (206,'2008-03-27 14:55:19',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',1,'192.168.20.149:51465'),
  (207,'2008-03-27 15:10:46',0,'192.168.20.198',2,'{09000000C468D1CC00142AA7AB810000}',0,NULL),
  (208,'2008-03-27 15:23:42',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',1,'192.168.20.182:7520'),
  (209,'2008-03-27 15:25:50',0,'192.168.20.198',2,'{0B0000005EDD03E900142AA7AB810000}',0,NULL),
  (210,'2008-03-27 15:35:26',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',1,'192.168.20.206:58386'),
  (211,'2008-03-27 15:56:49',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',1,'192.168.20.206:21267'),
  (212,'2008-03-27 15:58:39',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',1,'192.168.20.149:22542'),
  (213,'2008-03-27 16:03:54',0,'192.168.20.198',2,'{020000007F8B517A00142AA7AB810000}',1,'192.168.20.206:29971'),
  (214,'2008-03-27 16:04:41',0,'192.168.20.198',2,'{020000007F8B517A00142AA7AB810000}',0,NULL),
  (215,'2008-03-27 16:04:49',0,'192.168.20.198',2,'{05000000037A941600142AA7AB810000}',1,'192.168.20.206:30483'),
  (216,'2008-03-27 16:05:25',0,'192.168.20.198',2,'{05000000037A941600142AA7AB810000}',0,NULL),
  (217,'2008-03-27 16:05:33',0,'192.168.20.198',2,'{030000008147D23800142AA7AB810000}',1,'192.168.20.206:32787'),
  (218,'2008-03-27 16:10:21',0,'192.168.20.198',2,'{030000008147D23800142AA7AB810000}',0,NULL),
  (219,'2008-03-27 16:11:04',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',0,NULL),
  (220,'2008-03-27 16:11:11',0,'192.168.20.198',2,'{030000008147D23800142AA7AB810000}',1,'192.168.20.206:11012'),
  (221,'2008-03-27 16:14:36',0,'192.168.20.198',2,'{030000008147D23800142AA7AB810000}',0,NULL),
  (222,'2008-03-27 16:14:42',0,'192.168.20.198',2,'{0700000035A1D54100142AA7AB810000}',0,NULL),
  (223,'2008-03-27 16:29:17',0,'192.168.20.198',2,'{05000000037A941600142AA7AB810000}',1,'192.168.20.206:4869'),
  (224,'2008-03-27 16:29:21',0,'192.168.20.198',2,'{05000000037A941600142AA7AB810000}',0,NULL),
  (225,'2008-03-27 16:29:52',0,'192.168.20.198',2,'{030000008147D23800142AA7AB810000}',1,'192.168.20.206:5381'),
  (226,'2008-03-27 16:42:05',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',1,'192.168.20.149:1295'),
  (227,'2008-03-27 16:56:40',0,'192.168.20.198',2,'{030000008147D23800142AA7AB810000}',0,NULL),
  (228,'2008-03-27 17:04:00',0,'192.168.20.198',2,'{0C000000A9FE01E400142AA7AB810000}',0,NULL);

COMMIT;

#
# Data for the `log_4_1` table  (LIMIT 0,500)
#

INSERT INTO `log_4_1` (`ID`, `LogTime`, `Source`, `Ip`, `Level`, `Field1`, `Field2`, `Field3`, `Field4`, `Field5`, `Field6`, `Field7`, `Field8`, `Field9`, `Field10`, `Field11`, `Field12`, `Field13`, `Field14`, `Field15`, `Field16`, `Field17`, `Field18`, `Field19`, `Field20`, `Field21`, `Field22`, `Field23`, `Field24`, `Field25`, `Field26`, `Field27`, `Field28`, `Field29`, `Field30`, `Field31`) VALUES 
  (1,'2008-03-26 10:53:03',0,'192.168.20.198',2,'{01000000EF0085AA00142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',8,17,1,1,1,0,'2008-03-26 10:53:03',968280391,1,1000,1000,4294967295,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'金创药（小）','金创药（小）'),
  (2,'2008-03-26 10:53:03',0,'192.168.20.198',2,'{02000000F617A45700142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',8,17,1,1,1,0,'2008-03-26 10:53:03',221688776,1,1000,1000,4294967295,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'金创药（小）','金创药（小）'),
  (3,'2008-03-26 10:53:03',0,'192.168.20.198',2,'{03000000E1AE79D700142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',8,17,1,1,1,0,'2008-03-26 10:53:03',493954925,1,1000,1000,4294967295,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'金创药（小）','金创药（小）'),
  (4,'2008-03-26 10:53:03',0,'192.168.20.198',2,'{040000002056D0D800142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',8,17,1,1,1,0,'2008-03-26 10:53:03',3802827078,1,1000,1000,4294967295,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'金创药（小）','金创药（小）'),
  (5,'2008-03-26 10:53:03',0,'192.168.20.198',2,'{05000000631CF6CE00142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',8,17,1,1,1,0,'2008-03-26 10:53:03',3232851107,1,1000,1000,4294967295,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'金创药（小）','金创药（小）'),
  (6,'2008-03-26 10:53:36',0,'192.168.20.198',2,'{060000009A95F74700142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',8,20,1,406,1,0,'2008-03-26 10:53:36',3569718847,1,1000,1000,0,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'不动先生的烟火','不动先生的烟火'),
  (7,'2008-03-26 10:53:36',0,'192.168.20.198',2,'{07000000F5C5CAAC00142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',8,18,1,3,1,0,'2008-03-26 10:53:36',1415371680,1,1000,1000,4294967295,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'传送符（新手村）','传送符（新手村）'),
  (8,'2008-03-26 10:53:36',0,'192.168.20.198',2,'{08000000E4603A2800142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',8,18,1,3,1,0,'2008-03-26 10:53:36',2722795941,1,1000,1000,4294967295,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'传送符（新手村）','传送符（新手村）'),
  (9,'2008-03-26 10:54:52',0,'192.168.20.198',2,'{09000000170CE02B00142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',8,20,1,129,1,0,'2008-03-26 10:54:52',935809144,1,1000,1000,0,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'银票','银票'),
  (10,'2008-03-26 10:55:11',0,'192.168.20.198',2,'{0A0000007E96012400142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',8,1,10,1,1,3,'2008-03-26 10:55:10',1751746707,1,1000,1000,4294967295,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'铜护腕','铜护腕'),
  (11,'2008-03-26 10:55:11',0,'192.168.20.198',2,'{0B000000495395C300142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',8,1,6,1,1,1,'2008-03-26 10:55:10',4125369790,1,1000,1000,4294967295,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'护身符','护身符'),
  (12,'2008-03-26 11:20:38',0,'192.168.20.198',2,'{01000000338C602400142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',3,17,1,2,3,0,'2008-03-26 11:20:38',2755093491,2,1000,1000,0,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'馒头','馒头'),
  (13,'2008-03-26 11:21:12',0,'192.168.20.198',2,'{020000002A86DBDC00142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',3,17,1,2,1,0,'2008-03-26 11:21:12',3585820610,2,1000,1000,0,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'窝头','窝头'),
  (14,'2008-03-26 11:21:43',0,'192.168.20.198',2,'{030000004534B54B00142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',8,1,7,1,1,3,'2008-03-26 11:21:43',3795485294,1,1000,1000,4294967295,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'长靴','长靴'),
  (15,'2008-03-26 11:21:43',0,'192.168.20.198',2,'{04000000F455B8DB00142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',8,20,1,413,1,0,'2008-03-26 11:21:43',4082161848,1,1000,1000,4294967295,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'五行书','五行书'),
  (16,'2008-03-26 11:24:42',0,'192.168.20.198',2,'{01000000AF16814600142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',3,17,1,2,1,0,'2008-03-26 11:24:42',1737819467,2,1000,1000,0,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'窝头','窝头'),
  (17,'2008-03-26 11:25:31',0,'192.168.20.198',2,'{02000000B6D6B6BA00142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',8,1,7,1,1,4,'2008-03-26 11:25:31',765555815,1,1000,1000,4294967295,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'长靴','长靴'),
  (18,'2008-03-26 11:25:31',0,'192.168.20.198',2,'{03000000A1722CE500142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',8,20,1,413,1,0,'2008-03-26 11:25:31',2015658233,1,1000,1000,4294967295,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'五行书','五行书'),
  (19,'2008-03-26 11:30:08',0,'192.168.20.198',2,'{0100000070B9628A00142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',3,17,1,2,1,0,'2008-03-26 11:30:08',3535784874,2,1000,1000,0,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'窝头','窝头'),
  (20,'2008-03-26 11:30:30',0,'192.168.20.198',2,'{02000000733290C600142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',8,1,7,1,1,2,'2008-03-26 11:30:30',3545321430,1,1000,1000,4294967295,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'长靴','长靴'),
  (21,'2008-03-26 11:30:30',0,'192.168.20.198',2,'{030000006A93359100142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',8,20,1,413,1,0,'2008-03-26 11:30:30',603215200,1,1000,1000,4294967295,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'五行书','五行书'),
  (22,'2008-03-26 11:34:07',0,'192.168.20.198',2,'{010000008E1F4F7400142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',3,17,1,2,1,0,'2008-03-26 11:34:07',1228971323,2,1000,1000,0,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'窝头','窝头'),
  (23,'2008-03-26 11:34:27',0,'192.168.20.198',2,'{0200000019F8440600142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',8,1,7,1,1,3,'2008-03-26 11:34:27',2053455191,1,1000,1000,4294967295,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'长靴','长靴'),
  (24,'2008-03-26 11:34:27',0,'192.168.20.198',2,'{03000000785A892700142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',8,20,1,413,1,0,'2008-03-26 11:34:27',3283446121,1,1000,1000,4294967295,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'五行书','五行书'),
  (25,'2008-03-26 11:40:32',0,'192.168.20.198',2,'{01000000EAA196B200142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',3,17,1,2,1,0,'2008-03-26 11:40:32',1086416836,2,1000,1000,0,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'窝头','窝头'),
  (26,'2008-03-26 12:39:40',0,'192.168.20.198',2,'{0100000053EA117F00142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',3,17,1,1,1,0,'2008-03-26 12:39:40',2602916013,1,1000,1000,0,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'金创药（小）','金创药（小）'),
  (27,'2008-03-26 12:39:40',0,'192.168.20.198',2,'{02000000CAEB270400142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',3,17,1,1,1,0,'2008-03-26 12:39:40',2627233670,1,1000,1000,0,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'金创药（小）','金创药（小）'),
  (28,'2008-03-26 12:39:40',0,'192.168.20.198',2,'{030000006583AE5100142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',3,17,1,1,1,0,'2008-03-26 12:39:40',2417509347,1,1000,1000,0,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'金创药（小）','金创药（小）'),
  (29,'2008-03-26 14:22:55',0,'192.168.20.198',2,'{01000000CBAC1B3800142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',3,17,1,1,2,0,'2008-03-26 14:22:55',1248219724,1,1000,1000,0,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'金创药（中）','金创药（中）'),
  (30,'2008-03-26 14:22:55',0,'192.168.20.198',2,'{0200000062841AB800142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',3,17,1,1,2,0,'2008-03-26 14:22:55',3214724225,1,1000,1000,0,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'金创药（中）','金创药（中）'),
  (31,'2008-03-26 14:22:55',0,'192.168.20.198',2,'{030000009D19277000142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',3,17,1,1,2,0,'2008-03-26 14:22:55',3785724202,1,1000,1000,0,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'金创药（中）','金创药（中）'),
  (32,'2008-03-26 17:11:32',0,'192.168.20.198',2,'{0100000054B4777700142AA7AB810000}','{0500000020A1FAB1000AEBF4783E0000}',8,1,12,1,1,0,'2008-03-26 17:11:32',409092492,1,1000,1000,0,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'马牌（枣红马）','马牌（枣红马）'),
  (33,'2008-03-26 17:13:00',0,'192.168.20.198',2,'{02000000C7D1A18200142AA7AB810000}','{0500000020A1FAB1000AEBF4783E0000}',8,2,1,397,6,1,'2008-03-26 17:12:59',2655217385,1,1000,1000,0,4,'1970-01-01 08:00:00',0,119,5,141,4,85,4,114,4,516,3,0,0,'狼牙锤·破军','狼牙锤·破军'),
  (34,'2008-03-26 17:13:00',0,'192.168.20.198',2,'{030000006E054FE900142AA7AB810000}','{0500000020A1FAB1000AEBF4783E0000}',8,2,3,408,6,5,'2008-03-26 17:13:00',96819356,1,1000,1000,0,4,'1970-01-01 08:00:00',0,85,5,90,4,111,4,772,4,524,3,0,0,'鸟锤甲·延阳','鸟锤甲·延阳'),
  (35,'2008-03-26 17:13:00',0,'192.168.20.198',2,'{0400000079C6F45100142AA7AB810000}','{0500000020A1FAB1000AEBF4783E0000}',8,2,9,406,6,1,'2008-03-26 17:13:00',1506334963,1,1000,1000,0,4,'1970-01-01 08:00:00',0,85,5,90,4,95,4,771,4,544,3,0,0,'雷池冠·延阳','雷池冠·延阳'),
  (36,'2008-03-26 17:13:00',0,'192.168.20.198',2,'{05000000586F6B6200142AA7AB810000}','{0500000020A1FAB1000AEBF4783E0000}',8,2,8,206,6,4,'2008-03-26 17:13:00',32375454,1,1000,1000,0,4,'1970-01-01 08:00:00',0,85,5,90,4,95,4,770,3,534,3,0,0,'甲岚束腰·延阳','甲岚束腰·延阳'),
  (37,'2008-03-26 17:13:00',0,'192.168.20.198',2,'{06000000BBB07FB000142AA7AB810000}','{0500000020A1FAB1000AEBF4783E0000}',8,2,7,208,6,3,'2008-03-26 17:13:00',3390796941,1,1000,1000,0,4,'1970-01-01 08:00:00',0,106,5,85,4,95,4,773,5,554,4,123,3,'飞羽靴·越光','飞羽靴·越光'),
  (38,'2008-03-26 17:13:00',0,'192.168.20.198',2,'{070000009238CAEB00142AA7AB810000}','{0500000020A1FAB1000AEBF4783E0000}',8,2,10,208,6,2,'2008-03-26 17:13:00',3807294704,1,1000,1000,0,4,'1970-01-01 08:00:00',0,85,5,90,4,95,4,774,4,514,3,0,0,'金鳞护腕·延阳','金鳞护腕·延阳'),
  (39,'2008-03-26 17:13:00',0,'192.168.20.198',2,'{080000000DBD849D00142AA7AB810000}','{0500000020A1FAB1000AEBF4783E0000}',8,2,5,106,6,5,'2008-03-26 17:13:00',1374648375,1,1000,1000,0,4,'1970-01-01 08:00:00',0,85,5,90,4,98,4,772,4,524,3,0,0,'温润之链·延阳','温润之链·延阳'),
  (40,'2008-03-26 17:13:00',0,'192.168.20.198',2,'{090000009CDBF6A000142AA7AB810000}','{0500000020A1FAB1000AEBF4783E0000}',8,2,4,106,6,4,'2008-03-26 17:13:00',3259202578,1,1000,1000,0,4,'1970-01-01 08:00:00',0,85,5,90,4,98,4,770,3,534,3,0,0,'荒芜之戒·延阳','荒芜之戒·延阳'),
  (41,'2008-03-26 17:13:00',0,'192.168.20.198',2,'{0A000000EF7B6B4200142AA7AB810000}','{0500000020A1FAB1000AEBF4783E0000}',8,2,11,206,6,2,'2008-03-26 17:13:00',1661527665,1,1000,1000,0,4,'1970-01-01 08:00:00',0,85,5,90,4,95,4,774,4,514,3,0,0,'雁行佩·延阳','雁行佩·延阳'),
  (42,'2008-03-26 17:13:00',0,'192.168.20.198',2,'{0B000000F6A69C3900142AA7AB810000}','{0500000020A1FAB1000AEBF4783E0000}',8,2,6,107,6,3,'2008-03-26 17:13:00',251156612,1,1000,1000,0,4,'1970-01-01 08:00:00',0,85,5,90,4,98,4,773,4,134,3,769,3,'天罡符·延阳','天罡符·延阳'),
  (43,'2008-03-26 17:13:00',0,'192.168.20.198',2,'{0C000000E1018CE400142AA7AB810000}','{0500000020A1FAB1000AEBF4783E0000}',8,1,14,4,1,0,'2008-03-26 17:13:00',883041723,1,1000,1000,0,0,'1970-01-01 08:00:00',0,1,0,0,0,0,0,0,0,0,0,0,0,'锤天王初级秘籍','锤天王初级秘籍'),
  (44,'2008-03-26 18:11:56',0,'192.168.20.198',2,'{0D000000205DA58800142AA7AB810000}','{0B000000E9EF985C00142AA7AB810000}',8,2,1,377,6,1,'2008-03-26 18:11:56',1254089799,1,1000,1000,0,4,'1970-01-01 08:00:00',0,119,5,108,4,141,4,114,4,516,3,0,0,'虎尾棍·破军','虎尾棍·破军'),
  (45,'2008-03-26 18:19:29',0,'192.168.20.198',2,'{0E0000006307536A00142AA7AB810000}','{0B000000E9EF985C00142AA7AB810000}',3,1,1,1,5,1,'2008-03-26 18:19:29',2339716111,1,1000,1000,0,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'穿云缠手','穿云缠手'),
  (46,'2008-03-26 18:22:17',0,'192.168.20.198',2,'{0F0000009AD44CE500142AA7AB810000}','{0A000000BE768BD300142AA7AB810000}',3,17,3,1,2,0,'2008-03-26 18:22:17',3314521905,1,1000,1000,0,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'承仙蜜（中）','承仙蜜（中）'),
  (47,'2008-03-26 18:22:19',0,'192.168.20.198',2,'{10000000F508873600142AA7AB810000}','{0A000000BE768BD300142AA7AB810000}',3,17,4,1,2,0,'2008-03-26 18:22:19',4154272922,1,1000,1000,0,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'益阳散','益阳散'),
  (48,'2008-03-26 18:22:21',0,'192.168.20.198',2,'{11000000E497D15500142AA7AB810000}','{0A000000BE768BD300142AA7AB810000}',3,17,4,1,4,0,'2008-03-26 18:22:21',4283755463,1,1000,1000,0,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'补天散','补天散'),
  (49,'2008-03-26 18:31:15',0,'192.168.20.198',2,'{01000000D780A3C800142AA7AB810000}','{0A000000BE768BD300142AA7AB810000}',3,17,3,1,5,0,'2008-03-26 18:31:15',1690655504,1,1000,1000,0,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'五花玉露丸','五花玉露丸'),
  (50,'2008-03-26 19:52:36',0,'192.168.20.198',2,'{010000001AD1B5FB00142AA7AB810000}','{010000003DA5D05C00142AA7AB810000}',8,2,1,1,1,2,'2008-03-26 19:52:36',2477002373,1,1000,1000,4294967295,0,'1970-01-01 08:00:00',0,119,1,141,1,85,1,0,0,0,0,0,0,'龙五太爷用过的剑·延阳','龙五太爷用过的剑·延阳'),
  (51,'2008-03-26 20:05:45',0,'192.168.20.198',2,'{0100000071FC1E4F00142AA7AB810000}','{010000003DA5D05C00142AA7AB810000}',3,1,3,2,1,3,'2008-03-26 20:05:45',277933812,1,1000,1000,0,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'轻甲','轻甲'),
  (52,'2008-03-26 21:19:56',0,'192.168.20.198',2,'{010000002C485E2A00142AA7AB810000}','{010000007CC0C17B00142AA7AB810000}',8,18,1,24,1,0,'2008-03-26 21:19:56',105596214,1,1000,1000,0,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'九转续命丸','九转续命丸'),
  (53,'2008-03-26 21:19:56',0,'192.168.20.198',2,'{020000003F76BB6400142AA7AB810000}','{010000007CC0C17B00142AA7AB810000}',8,18,1,24,1,0,'2008-03-26 21:19:56',1374658131,1,1000,1000,0,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'九转续命丸','九转续命丸'),
  (54,'2008-03-26 21:19:56',0,'192.168.20.198',2,'{0300000006F8190000142AA7AB810000}','{010000007CC0C17B00142AA7AB810000}',8,18,1,24,1,0,'2008-03-26 21:19:56',3790156420,1,1000,1000,0,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'九转续命丸','九转续命丸'),
  (55,'2008-03-26 21:19:56',0,'192.168.20.198',2,'{04000000B14ECCCD00142AA7AB810000}','{010000007CC0C17B00142AA7AB810000}',8,18,1,24,1,0,'2008-03-26 21:19:56',1353350297,1,1000,1000,0,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'九转续命丸','九转续命丸'),
  (56,'2008-03-26 21:19:56',0,'192.168.20.198',2,'{05000000B0B6BBC700142AA7AB810000}','{010000007CC0C17B00142AA7AB810000}',8,18,1,24,1,0,'2008-03-26 21:19:56',2784062626,1,1000,1000,0,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'九转续命丸','九转续命丸'),
  (57,'2008-03-26 21:19:56',0,'192.168.20.198',2,'{06000000B33A8AA900142AA7AB810000}','{010000007CC0C17B00142AA7AB810000}',8,18,1,24,1,0,'2008-03-26 21:19:56',558015727,1,1000,1000,0,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'九转续命丸','九转续命丸'),
  (58,'2008-03-26 21:19:56',0,'192.168.20.198',2,'{07000000AA7A386B00142AA7AB810000}','{010000007CC0C17B00142AA7AB810000}',8,18,1,24,1,0,'2008-03-26 21:19:56',3058453264,1,1000,1000,0,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'九转续命丸','九转续命丸'),
  (59,'2008-03-26 21:19:56',0,'192.168.20.198',2,'{08000000C556102F00142AA7AB810000}','{010000007CC0C17B00142AA7AB810000}',8,18,1,24,1,0,'2008-03-26 21:19:56',3513597141,1,1000,1000,0,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'九转续命丸','九转续命丸'),
  (60,'2008-03-26 21:19:57',0,'192.168.20.198',2,'{0900000074EE50CE00142AA7AB810000}','{010000007CC0C17B00142AA7AB810000}',8,18,1,24,1,0,'2008-03-26 21:19:56',2874849614,1,1000,1000,0,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'九转续命丸','九转续命丸'),
  (61,'2008-03-26 21:19:57',0,'192.168.20.198',2,'{0A00000067A3E6DF00142AA7AB810000}','{010000007CC0C17B00142AA7AB810000}',8,18,1,24,1,0,'2008-03-26 21:19:56',351849931,1,1000,1000,0,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'九转续命丸','九转续命丸'),
  (62,'2008-03-26 21:20:00',0,'192.168.20.198',2,'{0B0000008E100F8E00142AA7AB810000}','{010000007CC0C17B00142AA7AB810000}',8,1,1,2,1,1,'2008-03-26 21:20:00',2617579228,1,1000,1000,255,0,'1970-01-01 08:00:00',0,120,3,0,0,0,0,0,0,0,0,0,0,'铜剑·碎尘','铜剑·碎尘'),
  (63,'2008-03-26 21:20:45',0,'192.168.20.198',2,'{0C0000001925098800142AA7AB810000}','{010000007CC0C17B00142AA7AB810000}',3,17,1,1,3,0,'2008-03-26 21:20:45',1633442511,1,1000,1000,0,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'金创药（大）','金创药（大）'),
  (64,'2008-03-26 21:20:45',0,'192.168.20.198',2,'{0D00000078D3304500142AA7AB810000}','{010000007CC0C17B00142AA7AB810000}',3,17,1,1,3,0,'2008-03-26 21:20:45',2074850416,1,1000,1000,0,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'金创药（大）','金创药（大）'),
  (65,'2008-03-26 21:20:45',0,'192.168.20.198',2,'{0E0000005B0428A300142AA7AB810000}','{010000007CC0C17B00142AA7AB810000}',3,17,1,1,3,0,'2008-03-26 21:20:45',4016314293,1,1000,1000,0,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'金创药（大）','金创药（大）'),
  (66,'2008-03-26 21:20:45',0,'192.168.20.198',2,'{0F000000B2BDE7C400142AA7AB810000}','{010000007CC0C17B00142AA7AB810000}',3,17,1,1,3,0,'2008-03-26 21:20:45',1994095534,1,1000,1000,0,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'金创药（大）','金创药（大）'),
  (67,'2008-03-26 21:20:45',0,'192.168.20.198',2,'{10000000ADADE87800142AA7AB810000}','{010000007CC0C17B00142AA7AB810000}',3,17,1,1,3,0,'2008-03-26 21:20:45',167282091,1,1000,1000,0,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'金创药（大）','金创药（大）'),
  (68,'2008-03-26 21:20:45',0,'192.168.20.198',2,'{11000000BC89956C00142AA7AB810000}','{010000007CC0C17B00142AA7AB810000}',3,17,1,1,3,0,'2008-03-26 21:20:45',12634684,1,1000,1000,0,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'金创药（大）','金创药（大）'),
  (69,'2008-03-26 21:20:45',0,'192.168.20.198',2,'{120000008FF12F1000142AA7AB810000}','{010000007CC0C17B00142AA7AB810000}',3,17,1,1,3,0,'2008-03-26 21:20:45',1740059185,1,1000,1000,0,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'金创药（大）','金创药（大）'),
  (70,'2008-03-26 21:20:45',0,'192.168.20.198',2,'{1300000016C66D6000142AA7AB810000}','{010000007CC0C17B00142AA7AB810000}',3,17,1,1,3,0,'2008-03-26 21:20:45',3110835098,1,1000,1000,0,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'金创药（大）','金创药（大）'),
  (71,'2008-03-26 21:20:45',0,'192.168.20.198',2,'{140000008124BD5900142AA7AB810000}','{010000007CC0C17B00142AA7AB810000}',3,17,1,1,3,0,'2008-03-26 21:20:45',439537351,1,1000,1000,0,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'金创药（大）','金创药（大）'),
  (72,'2008-03-26 22:40:29',0,'192.168.20.198',2,'{01000000521665E800142AA7AB810000}','{0500000000C87D8500142AA7AB810000}',8,2,1,1,1,2,'2008-03-26 22:40:29',3875451853,1,1000,1000,4294967295,0,'1970-01-01 08:00:00',0,119,1,141,1,85,1,0,0,0,0,0,0,'龙五太爷用过的剑·延阳','龙五太爷用过的剑·延阳'),
  (73,'2008-03-26 23:05:47',0,'192.168.20.198',2,'{02000000CD23DB6300142AA7AB810000}','{0500000000C87D8500142AA7AB810000}',3,1,1,1,5,5,'2008-03-26 23:05:47',720883736,1,1000,1000,0,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'穿云缠手','穿云缠手'),
  (74,'2008-03-26 23:05:49',0,'192.168.20.198',2,'{030000005CA77E7500142AA7AB810000}','{0500000000C87D8500142AA7AB810000}',3,1,1,1,3,5,'2008-03-26 23:05:49',844983474,1,1000,1000,0,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'拳宗缠手','拳宗缠手'),
  (75,'2008-03-27 10:59:53',0,'192.168.20.198',2,'{04000000AF18333A00142AA7AB810000}','{06000000C346CD3000142AA7AB810000}',8,1,12,5,3,0,'2008-03-27 10:59:52',2185660161,1,1000,1000,0,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'马牌（乌云踏雪）','马牌（乌云踏雪）'),
  (76,'2008-03-27 11:00:00',0,'192.168.20.198',2,'{05000000B6D0282E00142AA7AB810000}','{06000000C346CD3000142AA7AB810000}',8,1,1,2,1,1,'2008-03-27 11:00:00',3575372131,1,1000,1000,255,0,'1970-01-01 08:00:00',0,141,3,108,3,0,0,0,0,0,0,0,0,'铜剑·飞燕','铜剑·飞燕'),
  (77,'2008-03-27 15:35:45',0,'192.168.20.198',2,'{0100000051F4BE5200142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',3,1,3,3,4,1,'2008-03-27 15:35:45',2873824930,1,1000,1000,0,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'???','???'),
  (78,'2008-03-27 15:56:58',0,'192.168.20.198',2,'{0100000091B2C1BC00142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',3,1,10,1,4,5,'2008-03-27 15:56:58',2363907269,1,1000,1000,0,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'苍狼护腕','苍狼护腕'),
  (79,'2008-03-27 16:00:09',0,'192.168.20.198',2,'{02000000100B035500142AA7AB810000}','{0C000000A9FE01E400142AA7AB810000}',8,1,12,10,4,0,'2008-03-27 16:00:09',306358087,1,1000,1000,0,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'马牌（赤兔）','马牌（赤兔）'),
  (80,'2008-03-27 16:08:14',0,'192.168.20.198',2,'{0300000093BD3DF100142AA7AB810000}','{0C000000A9FE01E400142AA7AB810000}',8,2,1,1,1,4,'2008-03-27 16:08:14',1238340352,1,1000,1000,4294967295,0,'1970-01-01 08:00:00',0,119,1,141,1,85,1,0,0,0,0,0,0,'龙五太爷用过的剑·延阳','龙五太爷用过的剑·延阳'),
  (81,'2008-03-27 16:08:25',0,'192.168.20.198',2,'{040000000A7281CB00142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',3,1,3,3,3,1,'2008-03-27 16:08:25',3160228653,1,1000,1000,0,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'渔忍装','渔忍装'),
  (82,'2008-03-27 16:40:23',0,'192.168.20.198',2,'{05000000A5F00A5600142AA7AB810000}','{030000008147D23800142AA7AB810000}',3,1,9,3,1,2,'2008-03-27 16:40:23',3089344992,1,1000,1000,0,0,'1970-01-01 08:00:00',0,0,0,0,0,0,0,0,0,0,0,0,0,'弱水笠','弱水笠'),
  (83,'2008-03-27 16:55:29',0,'192.168.20.198',2,'{06000000D4A0E0B700142AA7AB810000}','{0C000000A9FE01E400142AA7AB810000}',8,1,1,5,1,1,'2008-03-27 16:55:29',4214311078,1,1000,1000,255,0,'1970-01-01 08:00:00',0,119,2,0,0,0,0,0,0,0,0,0,0,'长枪·破军','长枪·破军'),
  (84,'2008-03-27 16:55:45',0,'192.168.20.198',2,'{07000000470CB96500142AA7AB810000}','{0C000000A9FE01E400142AA7AB810000}',8,1,1,5,1,1,'2008-03-27 16:55:45',3466495211,1,1000,1000,255,0,'1970-01-01 08:00:00',0,141,3,1037,1,0,0,0,0,0,0,0,0,'长枪·心眼','长枪·心眼'),
  (85,'2008-03-27 16:56:32',0,'192.168.20.198',2,'{08000000EE55131B00142AA7AB810000}','{0C000000A9FE01E400142AA7AB810000}',8,1,1,5,1,1,'2008-03-27 16:56:32',624405585,1,1000,1000,255,0,'1970-01-01 08:00:00',0,1037,3,119,1,0,0,0,0,0,0,0,0,'长枪·北冥','长枪·北冥');

COMMIT;

#
# Data for the `log_4_2` table  (LIMIT 0,500)
#

INSERT INTO `log_4_2` (`ID`, `LogTime`, `Source`, `Ip`, `Level`, `Field1`, `Field2`, `Field3`, `Field4`, `Field5`, `Field6`) VALUES 
  (1,'2008-03-26 10:54:25',0,'192.168.20.198',2,'{060000009A95F74700142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',7,1,44992,99072),
  (2,'2008-03-26 10:55:08',0,'192.168.20.198',2,'{09000000170CE02B00142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',7,1,45174,98677),
  (3,'2008-03-26 11:02:55',0,'192.168.20.198',2,'{01000000EF0085AA00142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',4,1,45174,98677),
  (4,'2008-03-26 21:20:49',0,'192.168.20.198',2,'{0D00000078D3304500142AA7AB810000}','{010000007CC0C17B00142AA7AB810000}',4,1,44777,98781),
  (5,'2008-03-26 21:20:50',0,'192.168.20.198',2,'{0C0000001925098800142AA7AB810000}','{010000007CC0C17B00142AA7AB810000}',4,1,44777,98781),
  (6,'2008-03-26 21:20:58',0,'192.168.20.198',2,'{0B0000008E100F8E00142AA7AB810000}','{010000007CC0C17B00142AA7AB810000}',6,1,44793,98492),
  (7,'2008-03-27 16:11:36',0,'192.168.20.198',2,'{040000000A7281CB00142AA7AB810000}','{030000008147D23800142AA7AB810000}',6,1,43943,98271),
  (8,'2008-03-27 17:00:07',0,'192.168.20.198',2,'{08000000EE55131B00142AA7AB810000}','SYSTEM',2,0,0,0);

COMMIT;

#
# Data for the `log_4_3` table  (LIMIT 0,500)
#

INSERT INTO `log_4_3` (`ID`, `LogTime`, `Source`, `Ip`, `Level`, `Field1`, `Field2`, `Field3`, `Field4`, `Field5`, `Field6`) VALUES 
  (1,'2008-03-26 11:41:06',0,'192.168.20.198',2,'{02000000F617A45700142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',1,1,44883,98645),
  (2,'2008-03-26 11:41:21',0,'192.168.20.198',2,'{02000000F617A45700142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',1,1,44917,98610),
  (3,'2008-03-26 11:48:05',0,'192.168.20.198',2,'{03000000E1AE79D700142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',1,1,44877,98626),
  (4,'2008-03-26 11:48:12',0,'192.168.20.198',2,'{03000000E1AE79D700142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',1,1,44910,98589),
  (5,'2008-03-26 11:49:53',0,'192.168.20.198',2,'{03000000E1AE79D700142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',1,1,44902,98633),
  (6,'2008-03-26 11:53:20',0,'192.168.20.198',2,'{05000000631CF6CE00142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',1,1,44823,98825),
  (7,'2008-03-26 12:39:46',0,'192.168.20.198',2,'{0100000053EA117F00142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',1,1,44794,98492),
  (8,'2008-03-26 12:44:26',0,'192.168.20.198',2,'{02000000CAEB270400142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',1,1,44709,98658),
  (9,'2008-03-26 12:49:22',0,'192.168.20.198',2,'{040000002056D0D800142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',1,1,44640,98555),
  (10,'2008-03-26 14:23:23',0,'192.168.20.198',2,'{01000000CBAC1B3800142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',1,1,44785,98612),
  (11,'2008-03-26 21:20:05',0,'192.168.20.198',2,'{0B0000008E100F8E00142AA7AB810000}','{010000007CC0C17B00142AA7AB810000}',1,1,44437,98731),
  (12,'2008-03-26 21:20:52',0,'192.168.20.198',2,'{11000000BC89956C00142AA7AB810000}','{010000007CC0C17B00142AA7AB810000}',1,1,44777,98781),
  (13,'2008-03-26 23:05:56',0,'192.168.20.198',2,'{030000005CA77E7500142AA7AB810000}','{0500000000C87D8500142AA7AB810000}',1,1,42936,99038),
  (14,'2008-03-27 14:24:38',0,'192.168.20.198',2,'{01000000EAA196B200142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',1,1,43735,98546),
  (15,'2008-03-27 14:24:49',0,'192.168.20.198',2,'{08000000E4603A2800142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',1,1,43771,98524),
  (16,'2008-03-27 14:24:52',0,'192.168.20.198',2,'{010000006087B44D00142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',1,1,43802,98496),
  (17,'2008-03-27 14:40:36',0,'192.168.20.198',2,'{010000006087B44D00142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',1,1,43829,98474),
  (18,'2008-03-27 16:01:42',0,'192.168.20.198',2,'{02000000100B035500142AA7AB810000}','{0C000000A9FE01E400142AA7AB810000}',1,3,51248,102094),
  (19,'2008-03-27 16:06:36',0,'192.168.20.198',2,'{0100000051F4BE5200142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',1,1,43887,98146),
  (20,'2008-03-27 16:08:29',0,'192.168.20.198',2,'{040000000A7281CB00142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',1,1,43887,98146),
  (21,'2008-03-27 16:57:53',0,'192.168.20.198',2,'{08000000EE55131B00142AA7AB810000}','{0C000000A9FE01E400142AA7AB810000}',1,3,51364,101838);

COMMIT;

#
# Data for the `log_4_4` table  (LIMIT 0,500)
#

INSERT INTO `log_4_4` (`ID`, `LogTime`, `Source`, `Ip`, `Level`, `Field1`, `Field2`, `Field3`, `Field4`, `Field5`, `Field6`) VALUES 
  (1,'2008-03-26 11:41:20',0,'192.168.20.198',2,'{02000000F617A45700142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',1,1,44883,98645),
  (2,'2008-03-26 11:48:10',0,'192.168.20.198',2,'{03000000E1AE79D700142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',1,1,44875,98539),
  (3,'2008-03-26 11:48:45',0,'192.168.20.198',2,'{03000000E1AE79D700142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',1,1,44910,98589),
  (4,'2008-03-26 14:24:45',0,'192.168.20.198',2,'{01000000CBAC1B3800142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',1,1,44820,98581),
  (5,'2008-03-26 21:20:12',0,'192.168.20.198',2,'{0B0000008E100F8E00142AA7AB810000}','{010000007CC0C17B00142AA7AB810000}',1,1,44475,98676),
  (6,'2008-03-26 21:20:53',0,'192.168.20.198',2,'{11000000BC89956C00142AA7AB810000}','{010000007CC0C17B00142AA7AB810000}',1,1,44777,98781),
  (7,'2008-03-26 23:15:36',0,'192.168.20.198',2,'{030000005CA77E7500142AA7AB810000}','{010000003DA5D05C00142AA7AB810000}',1,1,42905,99013),
  (8,'2008-03-27 14:24:40',0,'192.168.20.198',2,'{01000000EAA196B200142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',1,1,43735,98546),
  (9,'2008-03-27 14:24:50',0,'192.168.20.198',2,'{08000000E4603A2800142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',1,1,43771,98524),
  (10,'2008-03-27 14:24:53',0,'192.168.20.198',2,'{010000006087B44D00142AA7AB810000}','{0700000035A1D54100142AA7AB810000}',1,1,43802,98496),
  (11,'2008-03-27 14:40:41',0,'192.168.20.198',2,'{010000006087B44D00142AA7AB810000}','{01000000C42D4EFC000AEBF4783E0000}',1,1,43875,98429),
  (12,'2008-03-27 16:02:52',0,'192.168.20.198',2,'{02000000100B035500142AA7AB810000}','{0C000000A9FE01E400142AA7AB810000}',1,3,51292,102099),
  (13,'2008-03-27 16:06:41',0,'192.168.20.198',2,'{0100000051F4BE5200142AA7AB810000}','{030000008147D23800142AA7AB810000}',1,1,43931,98094),
  (14,'2008-03-27 16:08:32',0,'192.168.20.198',2,'{040000000A7281CB00142AA7AB810000}','{030000008147D23800142AA7AB810000}',1,1,43931,98094);

COMMIT;

#
# Data for the `log_4_5` table  (LIMIT 0,500)
#

INSERT INTO `log_4_5` (`ID`, `LogTime`, `Source`, `Ip`, `Level`, `Field1`, `Field2`, `Field3`, `Field4`, `Field5`, `Field6`, `Field7`, `Field8`, `Field9`, `Field10`) VALUES 
  (1,'2008-03-26 23:22:57',0,'192.168.20.198',2,'{030000005CA77E7500142AA7AB810000}','{07000000E91355A3000AEBF4783E0000}','{010000003DA5D05C00142AA7AB810000}',1,1,42897,99046,1,43015,99052);

COMMIT;

#
# Data for the `log_7_1` table  (LIMIT 0,500)
#

INSERT INTO `log_7_1` (`ID`, `LogTime`, `Source`, `Ip`, `Level`, `Field1`, `Field2`) VALUES 
  (1,'2008-03-26 15:47:59',0,'192.168.20.198',2,'{02000000ED9748EA00142AA7AB810000}','me.KickOut()'),
  (2,'2008-03-26 15:53:57',0,'192.168.20.198',2,'{02000000ED9748EA00142AA7AB810000}','me.NewWorld(1,1390,3089)'),
  (3,'2008-03-26 16:21:30',0,'192.168.20.198',2,'{03000000FC816496000AEBF4783E0000}','me.NewWorld(1,1394,3059)');

COMMIT;

#
# Data for the `log_7_2` table  (LIMIT 0,500)
#

INSERT INTO `log_7_2` (`ID`, `LogTime`, `Source`, `Ip`, `Level`, `Field1`) VALUES 
  (1,'2008-03-26 15:27:53',3,'192.168.20.198',2,'GlobalExcute(\"KDialog.Msg2SubWorld(\'系统公告:dddddddddddddd\')\")'),
  (2,'2008-03-26 15:33:31',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:ddddddddddddddd\"})'),
  (3,'2008-03-26 15:33:52',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", \"0\", \"0\", \"系统公告:dddddddddddddddddd\"})'),
  (4,'2008-03-26 15:34:18',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:ddddddddddddddddddd\"})'),
  (5,'2008-03-26 15:34:36',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", \"0\", \"0\", \"系统公告:aaaaaaaaaaaaaa\"})'),
  (6,'2008-03-26 15:36:22',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:aaaaaaaaaaaaaaaaaa\"})'),
  (7,'2008-03-26 15:37:17',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (8,'2008-03-26 15:38:17',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (9,'2008-03-26 15:41:06',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (10,'2008-03-26 15:42:06',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (11,'2008-03-26 15:42:06',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (12,'2008-03-26 15:43:07',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (13,'2008-03-26 15:43:07',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (14,'2008-03-26 15:44:08',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (15,'2008-03-26 15:44:08',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (16,'2008-03-26 15:44:54',3,'192.168.20.198',2,'SendMailGC(\"aaaaaa2\",\"GMMSG\",\"您的问题已收到,游戏技巧请访问jxsj.kingsoft.com,GM频道只受理卡号、无法登录、功能异常、程序BUG等反馈及违规行为举报,祝游戏愉快!\")'),
  (17,'2008-03-26 15:45:09',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (18,'2008-03-26 15:45:09',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (19,'2008-03-26 15:45:14',3,'192.168.20.198',2,'SendMailGC(\"aaaaaa2\",\"GMMSG\",\"sdfgsdfgsdfgdfgsdfgdfgsdfgdsfgsfgsfg?\")'),
  (20,'2008-03-26 15:45:44',3,'192.168.20.198',2,'SendMailGC(\"aaaaaa2\",\"GMMSG\",\"您的问题已收到,游戏技巧请访问jxsj.kingsoft.com,GM频道只受理卡号、无法登录、功能异常、程序BUG等反馈及违规行为举报,祝游戏愉快!\")'),
  (21,'2008-03-26 15:46:09',3,'192.168.20.198',2,'SendMailGC(\"aaaaaa2\",\"GMMSG\",\"???????????????????????????????????????????????????????????%$\")'),
  (22,'2008-03-26 15:46:09',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (23,'2008-03-26 15:46:09',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (24,'2008-03-26 15:47:01',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:哈哈哈哈，有个猪头坐我旁边。\"})'),
  (25,'2008-03-26 15:47:10',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (26,'2008-03-26 15:47:10',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (27,'2008-03-26 15:47:32',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:有个猪头坐我旁边。\"})'),
  (28,'2008-03-26 15:47:59',3,'192.168.20.198',2,'SendMailGC(\"aaaaaa2\",\"GMMSG\",\"您好，由于刷GM频道，您被踢人。\")'),
  (29,'2008-03-26 15:48:11',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (30,'2008-03-26 15:48:11',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (31,'2008-03-26 15:49:11',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (32,'2008-03-26 15:49:11',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (33,'2008-03-26 15:50:12',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (34,'2008-03-26 15:50:12',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (35,'2008-03-26 15:50:18',3,'192.168.20.198',2,'SendMailGC(\"aaaaaa2\",\"GMMSG\",\"你信不信。\")'),
  (36,'2008-03-26 15:51:13',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (37,'2008-03-26 15:51:13',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (38,'2008-03-26 15:52:14',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (39,'2008-03-26 15:52:14',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (40,'2008-03-26 15:53:15',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (41,'2008-03-26 15:53:15',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (42,'2008-03-26 15:54:16',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (43,'2008-03-26 15:54:16',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (44,'2008-03-26 15:55:17',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (45,'2008-03-26 15:55:17',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (46,'2008-03-26 15:56:18',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (47,'2008-03-26 15:56:18',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (48,'2008-03-26 15:57:18',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (49,'2008-03-26 15:57:18',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (50,'2008-03-26 15:58:19',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (51,'2008-03-26 15:58:19',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (52,'2008-03-26 15:59:20',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (53,'2008-03-26 15:59:20',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (54,'2008-03-26 16:00:20',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (55,'2008-03-26 16:00:20',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (56,'2008-03-26 16:01:22',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (57,'2008-03-26 16:01:22',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (58,'2008-03-26 16:02:22',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (59,'2008-03-26 16:02:22',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (60,'2008-03-26 16:03:12',3,'192.168.20.198',2,'SendMailGC(\"新建角色名\",\"GMMSG\",\"您的问题已收到,游戏技巧请访问jxsj.kingsoft.com,GM频道只受理卡号、无法登录、功能异常、程序BUG等反馈及违规行为举报,祝游戏愉快!\")'),
  (61,'2008-03-26 16:03:23',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (62,'2008-03-26 16:03:23',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (63,'2008-03-26 16:03:34',3,'192.168.20.198',2,'SendMailGC(\"新建角色名\",\"GMMSG\",\"adsfasdfasdfasdfasdfasfdasdfasdfasdfasdfasdfasdfasdfasdfasdfasdf?\")'),
  (64,'2008-03-26 16:03:56',3,'192.168.20.198',2,'SendMailGC(\"新建角色名\",\"GMMSG\",\"您的问题已收到,游戏技巧请访问jxsj.kingsoft.com,GM频道只受理卡号、无法登录、功能异常、程序BUG等反馈及违规行为举报,祝游戏愉快!\")'),
  (65,'2008-03-26 16:04:25',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (66,'2008-03-26 16:04:25',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (67,'2008-03-26 16:05:26',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (68,'2008-03-26 16:05:26',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (69,'2008-03-26 16:06:27',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (70,'2008-03-26 16:06:27',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (71,'2008-03-26 16:07:27',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (72,'2008-03-26 16:07:27',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (73,'2008-03-26 16:08:28',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (74,'2008-03-26 16:08:28',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (75,'2008-03-26 16:09:30',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (76,'2008-03-26 16:09:30',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (77,'2008-03-26 16:10:31',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (78,'2008-03-26 16:10:31',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (79,'2008-03-26 16:11:32',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (80,'2008-03-26 16:11:32',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (81,'2008-03-26 16:12:32',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (82,'2008-03-26 16:12:32',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (83,'2008-03-26 16:13:12',3,'192.168.20.198',2,'SendMailGC(\"新建角色名\",\"GMMSG\",\"砍死你，行不，就砍死你\")'),
  (84,'2008-03-26 16:13:33',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (85,'2008-03-26 16:13:33',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (86,'2008-03-26 16:14:34',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (87,'2008-03-26 16:14:34',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (88,'2008-03-26 16:15:34',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (89,'2008-03-26 16:15:34',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (90,'2008-03-26 16:16:35',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (91,'2008-03-26 16:16:35',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (92,'2008-03-26 16:17:35',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (93,'2008-03-26 16:17:36',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (94,'2008-03-26 16:18:36',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (95,'2008-03-26 16:18:36',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (96,'2008-03-26 16:19:38',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (97,'2008-03-26 16:19:38',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (98,'2008-03-26 16:20:38',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (99,'2008-03-26 16:20:38',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (100,'2008-03-26 16:21:39',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (101,'2008-03-26 16:21:39',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (102,'2008-03-26 16:22:19',3,'192.168.20.198',2,'SendMailGC(\"新建角色名\",\"GMMSG\",\"我快要疯掉了。你们呢？\")'),
  (103,'2008-03-26 16:22:40',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (104,'2008-03-26 16:22:40',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (105,'2008-03-26 16:23:40',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (106,'2008-03-26 16:23:40',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (107,'2008-03-26 16:24:41',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (108,'2008-03-26 16:24:41',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (109,'2008-03-26 16:25:42',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (110,'2008-03-26 16:25:42',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (111,'2008-03-26 16:26:42',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (112,'2008-03-26 16:26:42',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (113,'2008-03-26 16:27:44',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (114,'2008-03-26 16:27:44',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (115,'2008-03-26 16:28:45',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (116,'2008-03-26 16:28:45',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (117,'2008-03-26 16:29:45',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (118,'2008-03-26 16:29:45',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (119,'2008-03-26 16:30:45',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (120,'2008-03-26 16:30:46',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (121,'2008-03-26 16:31:46',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (122,'2008-03-26 16:31:47',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (123,'2008-03-26 16:32:47',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (124,'2008-03-26 16:32:47',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (125,'2008-03-26 16:33:48',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (126,'2008-03-26 16:33:48',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (127,'2008-03-26 16:34:49',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (128,'2008-03-26 16:34:49',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (129,'2008-03-26 16:35:50',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (130,'2008-03-26 16:35:50',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (131,'2008-03-26 16:36:51',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (132,'2008-03-26 16:36:51',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (133,'2008-03-26 16:37:52',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (134,'2008-03-26 16:37:52',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (135,'2008-03-26 16:38:52',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (136,'2008-03-26 16:38:52',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (137,'2008-03-26 16:39:53',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (138,'2008-03-26 16:39:53',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (139,'2008-03-26 16:40:54',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (140,'2008-03-26 16:40:54',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (141,'2008-03-26 16:41:06',3,'192.168.20.198',2,'SendMailGC(\"新建角色名\",\"GMMSG\",\"您的问题已收到,游戏技巧请访问jxsj.kingsoft.com,GM频道只受理卡号、无法登录、功能异常、程序BUG等反馈及违规行为举报,祝游戏愉快!\")'),
  (142,'2008-03-26 16:41:55',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (143,'2008-03-26 16:41:55',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (144,'2008-03-26 16:42:44',3,'192.168.20.198',2,'SendMailGC(\"新建角色名\",\"GMMSG\",\"天天<corld=red>天天好心情。\")'),
  (145,'2008-03-26 16:42:56',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (146,'2008-03-26 16:42:57',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (147,'2008-03-26 16:43:57',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (148,'2008-03-26 16:43:57',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (149,'2008-03-26 16:44:58',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (150,'2008-03-26 16:44:58',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (151,'2008-03-26 16:45:58',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (152,'2008-03-26 16:45:58',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (153,'2008-03-26 16:47:00',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (154,'2008-03-26 16:47:00',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (155,'2008-03-26 16:48:00',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (156,'2008-03-26 16:48:00',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})'),
  (157,'2008-03-26 16:49:05',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.NewsMsg\", 0, 0, \"系统公告:猪头宣布猪头宣布，大家听到了吧！！！！\"})'),
  (158,'2008-03-26 16:49:10',3,'192.168.20.198',2,'GlobalExcute({\"KDialog.Msg2SubWorld\", \"系统公告:猪头宣布猪头宣布\"})');

COMMIT;

#
# Data for the `log_8_1` table  (LIMIT 0,500)
#

INSERT INTO `log_8_1` (`ID`, `LogTime`, `Source`, `Ip`, `Level`, `Field1`, `Field2`, `Field3`, `Field4`, `Field5`, `Field6`, `Field7`, `Field8`, `Field9`) VALUES 
  (1,'2008-03-26 23:22:57',0,'192.168.20.198',2,1,'{07000000E91355A3000AEBF4783E0000}',1,42897,99046,'{010000003DA5D05C00142AA7AB810000}',1,43015,99052);

COMMIT;

#
# Data for the `log_8_2` table  (LIMIT 0,500)
#

INSERT INTO `log_8_2` (`ID`, `LogTime`, `Source`, `Ip`, `Level`, `Field1`, `Field2`, `Field3`, `Field4`, `Field5`, `Field6`) VALUES 
  (1,'2008-03-26 23:22:57',0,'192.168.20.198',2,1,'{010000003DA5D05C00142AA7AB810000}','{07000000E91355A3000AEBF4783E0000}','{030000005CA77E7500142AA7AB810000}','拳宗缠手',1);

COMMIT;

#
# Data for the `map` table  (LIMIT 0,500)
#

INSERT INTO `map` (`MapID`, `MapName`) VALUES 
  (1,'云中镇\r'),
  (2,'龙门镇\r'),
  (3,'永乐镇\r'),
  (4,'稻香村\r'),
  (5,'江津村\r'),
  (6,'石鼓镇\r'),
  (7,'龙泉村\r'),
  (8,'巴陵县\r'),
  (9,'少林派\r'),
  (10,'天忍教\r'),
  (11,'萨满教\r'),
  (12,'昆仑派\r'),
  (13,'西夏一品堂\r'),
  (14,'武当派\r'),
  (15,'丐帮\r'),
  (16,'峨嵋派\r'),
  (17,'翠烟门\r'),
  (18,'唐门\r'),
  (19,'大理段氏\r'),
  (20,'五毒教\r'),
  (21,'长歌门\r'),
  (22,'天王帮\r'),
  (23,'汴京府\r'),
  (24,'凤翔府\r'),
  (25,'襄阳府\r'),
  (26,'扬州府\r'),
  (27,'成都府\r'),
  (28,'大理府\r'),
  (29,'临安府\r'),
  (30,'古战场\r'),
  (31,'龙门客栈\r'),
  (32,'军马场\r'),
  (33,'梅花岭\r'),
  (34,'长江河谷\r'),
  (35,'白族市集\r'),
  (36,'雁荡龙湫\r'),
  (37,'洞庭湖畔\r'),
  (38,'镇东墓园\r'),
  (39,'祁连山\r'),
  (40,'潼关\r'),
  (41,'淮水沙洲\r'),
  (42,'蜀南竹海\r'),
  (43,'茶马古道\r'),
  (44,'铸剑坊\r'),
  (45,'岳阳楼\r'),
  (46,'后山禁地\r'),
  (47,'天忍教禁地\r'),
  (48,'见性峰\r'),
  (49,'天柱峰\r'),
  (50,'姑苏水榭\r'),
  (51,'九老峰\r'),
  (52,'百花谷\r'),
  (53,'翡翠湖\r'),
  (54,'南夷部落\r'),
  (55,'青螺岛\r'),
  (56,'塔林东部\r'),
  (57,'金国皇陵一层\r'),
  (58,'冰穴迷宫一层\r'),
  (59,'龙虎幻境东部\r'),
  (60,'燕子坞外围\r'),
  (61,'九老洞一层\r'),
  (62,'百花阵外阵\r'),
  (63,'湖畔竹林东部\r'),
  (64,'原始森林东部\r'),
  (65,'芦苇荡东南部\r'),
  (66,'塔林西部\r'),
  (67,'金国皇陵二层\r'),
  (68,'冰穴迷宫二层\r'),
  (69,'龙虎幻境西部\r'),
  (70,'燕子坞中段\r'),
  (71,'九老洞二层\r'),
  (72,'百花阵内阵\r'),
  (73,'湖畔竹林西部\r'),
  (74,'原始森林西部\r'),
  (75,'芦苇荡西北部\r'),
  (76,'塔林中部\r'),
  (77,'金国皇陵三层\r'),
  (78,'冰穴迷宫三层\r'),
  (79,'龙虎幻境中部\r'),
  (80,'燕子坞内部\r'),
  (81,'九老洞三层\r'),
  (82,'百花阵阵眼\r'),
  (83,'湖畔竹林中部\r'),
  (84,'原始森林中部\r'),
  (85,'芦苇荡中部\r'),
  (86,'太行古径\r'),
  (87,'大散关\r'),
  (88,'汉水古渡\r'),
  (89,'寒山古刹\r'),
  (90,'浣花溪\r'),
  (91,'洱海摩岩\r'),
  (92,'采石矶\r'),
  (93,'药王洞\r'),
  (94,'居延泽\r'),
  (95,'伏牛山\r'),
  (96,'虎丘剑池\r'),
  (97,'响水洞\r'),
  (98,'点苍山\r'),
  (99,'彭蠡古泽\r'),
  (100,'风陵渡\r'),
  (101,'沙漠迷宫\r'),
  (102,'鸡冠洞\r'),
  (103,'蜀冈山\r'),
  (104,'剑阁蜀道\r'),
  (105,'段氏皇陵\r'),
  (106,'九嶷溪\r'),
  (107,'龙门石窟\r'),
  (108,'西夏皇陵\r'),
  (109,'黄鹤楼\r'),
  (110,'荐菊洞\r'),
  (111,'剑门关\r'),
  (112,'天龙寺\r'),
  (113,'帮源秘洞\r'),
  (114,'敕勒川\r'),
  (115,'嘉峪关\r'),
  (116,'华山\r'),
  (117,'蜀冈秘境\r'),
  (118,'丰都鬼城\r'),
  (119,'苗岭\r'),
  (120,'武夷山\r'),
  (121,'武陵山\r'),
  (122,'漠北草原\r'),
  (123,'敦煌古城\r'),
  (124,'活死人墓\r'),
  (125,'大禹台\r'),
  (126,'三峡栈道\r'),
  (127,'蚩尤洞\r'),
  (128,'锁云渊\r'),
  (129,'伏流洞\r'),
  (130,'蒙古王庭\r'),
  (131,'月牙泉\r'),
  (132,'阿房宫废墟\r'),
  (133,'梁山泊\r'),
  (134,'神女峰\r'),
  (135,'夜郎旧墟\r'),
  (136,'鼓浪屿\r'),
  (137,'桃花源\r'),
  (138,'云中镇_室内\r'),
  (139,'龙门镇_室内\r'),
  (140,'永乐镇_室内\r'),
  (141,'稻香村_室内\r'),
  (142,'江津村_室内\r'),
  (143,'江津村_室内\r'),
  (144,'龙泉村_室内\r'),
  (145,'巴陵县_室内\r'),
  (146,'少林派_室内\r'),
  (147,'天忍教_室内\r'),
  (148,'萨满教_室内\r'),
  (149,'昆仑派_室内\r'),
  (150,'西夏一品堂_室内\r'),
  (151,'武当派_室内\r'),
  (152,'丐帮_室内\r'),
  (153,'峨嵋派_室内\r'),
  (154,'翠烟门_室内\r'),
  (155,'唐门_室内\r'),
  (156,'大理段氏_室内\r'),
  (157,'五毒教_室内\r'),
  (158,'长歌门_室内\r'),
  (159,'天王帮_室内\r'),
  (160,'汴京府_室内\r'),
  (161,'凤翔府_室内\r'),
  (162,'襄阳府_室内\r'),
  (163,'扬州府_室内\r'),
  (164,'成都府_室内\r'),
  (165,'大理府_室内\r'),
  (166,'临安府_室内\r'),
  (167,'武林擂台(汴京)\r'),
  (168,'武林擂台(凤翔)\r'),
  (169,'武林擂台(襄阳)\r'),
  (170,'武林擂台(扬州)\r'),
  (171,'武林擂台(成都)\r'),
  (172,'武林擂台(大理)\r'),
  (173,'武林擂台(临安)\r'),
  (174,'铁索栈桥(汴京)\r'),
  (175,'铁索栈桥(凤翔)\r'),
  (176,'铁索栈桥(襄阳)\r'),
  (177,'铁索栈桥(扬州)\r'),
  (178,'铁索栈桥(成都)\r'),
  (179,'铁索栈桥(大理)\r'),
  (180,'铁索栈桥(临安)\r'),
  (181,'战场报名点_宋(扬州)\r'),
  (182,'战场报名点_宋(凤翔)\r'),
  (183,'战场报名点_宋(襄阳)\r'),
  (184,'战场报名点_金(扬州)\r'),
  (185,'战场报名点_金(凤翔)\r'),
  (186,'战场报名点_金(襄阳)\r'),
  (187,'城郊战场(扬州)\r'),
  (188,'城郊战场(凤翔)\r'),
  (189,'城郊战场(襄阳)\r'),
  (190,'森林战场(扬州)\r'),
  (191,'森林战场(凤翔)\r'),
  (192,'森林战场(襄阳)\r'),
  (193,'峡谷战场(扬州)\r'),
  (194,'峡谷战场(凤翔)\r'),
  (195,'峡谷战场(襄阳)\r'),
  (196,'见性峰_室内\r'),
  (197,'九老峰_室内\r'),
  (198,'青螺岛_室内\r'),
  (199,'龙虎幻境东部_室内\r'),
  (200,'燕子坞外围_室内\r'),
  (201,'湖畔竹林东部_室内\r'),
  (202,'塔林西部_室内\r'),
  (203,'金国皇陵二层_室内\r'),
  (204,'百花阵内阵_室内\r'),
  (205,'原始森林西部_室内\r'),
  (206,'大散关_室内\r'),
  (207,'汉水古渡_室内\r'),
  (208,'浣花溪_室内\r'),
  (209,'洱海摩岩_室内\r'),
  (210,'居延泽_室内\r'),
  (211,'伏牛山_室内\r'),
  (212,'点苍山_室内\r'),
  (213,'彭蠡古泽_室内\r'),
  (214,'沙漠迷宫_室内\r'),
  (215,'鸡冠洞_室内\r'),
  (216,'剑阁蜀道_室内\r'),
  (217,'九嶷溪_室内\r'),
  (218,'龙门石窟_室内\r'),
  (219,'西夏皇陵_室内\r'),
  (220,'天龙寺_室内\r'),
  (221,'帮源秘洞_室内\r'),
  (222,'汴京府大牢\r'),
  (223,'临安府大牢\r'),
  (224,'明教\r'),
  (225,'白虎堂大殿(初级)\r'),
  (226,'白虎堂一层_东(初级)\r'),
  (227,'白虎堂一层_南(初级)\r'),
  (228,'白虎堂一层_西(初级)\r'),
  (229,'白虎堂一层_北(初级)\r'),
  (230,'白虎堂二层_阴(初级)\r'),
  (231,'白虎堂二层_阳(初级)\r'),
  (232,'白虎堂三层(初级)\r'),
  (233,'白虎堂大殿(高级)\r'),
  (234,'白虎堂一层_东(高级)\r'),
  (235,'白虎堂一层_南(高级)\r'),
  (236,'白虎堂一层_西(高级)\r'),
  (237,'白虎堂一层_北(高级)\r'),
  (238,'白虎堂二层_阴(高级)\r'),
  (239,'白虎堂二层_阳(高级)\r'),
  (240,'白虎堂三层(高级)\r'),
  (241,'门派竞技场(少林)\r'),
  (242,'门派竞技场(天王)\r'),
  (243,'门派竞技场(唐门)\r'),
  (244,'门派竞技场(五毒)\r'),
  (245,'门派竞技场(峨嵋)\r'),
  (246,'门派竞技场(翠烟)\r'),
  (247,'门派竞技场(丐帮)\r'),
  (248,'门派竞技场(天忍)\r'),
  (249,'门派竞技场(武当)\r'),
  (250,'门派竞技场(昆仑)\r'),
  (251,'门派竞技场(明教)\r'),
  (252,'门派竞技场(段氏)\r'),
  (253,'百年天牢\r'),
  (254,'陶朱公疑冢\r'),
  (255,'洗髓岛\r'),
  (256,'洗髓岛山洞\r');

COMMIT;

#
# Data for the `tong` table  (LIMIT 0,500)
#

INSERT INTO `tong` (`ID`, `LogTime`, `Source`, `IP`, `Level`, `Field1`, `Field2`) VALUES 
  (1,'2008-02-02',0,'127.0.0.1',0,'0','??????');

COMMIT;

#
# Data for the `trolebaseinfo` table  (LIMIT 0,500)
#

INSERT INTO `trolebaseinfo` (`id`, `cRoleId`, `cAccountName`, `cRoleName`, `iSex`, `iSeries`, `iLevel`, `iCamp`, `iPkValue`, `iExp`, `iLife`, `iMana`, `iStamina`, `iGatherPoint`, `iMakePoint`, `iRemainPotential`, `iStrength`, `iDexterity`, `iVitality`, `iEnergy`, `iSkillPoint`, `dCreateTime`, `dLastLoginTime`, `iGameTime`, `cLastLoginIP`, `iFaction`, `iFactionRoute`, `iMoney`, `iTongID`, `SERVER_TYPE`, `ConfigFileName`) VALUES 
  (149,'1','1','1',1,1,20,100,20,500,500,500,500,1,1,1,1,1,1,1,1,'2008-01-01','2008-01-02',200,'0.0.0.0',1,1,100,1,1,'1');

COMMIT;



/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;