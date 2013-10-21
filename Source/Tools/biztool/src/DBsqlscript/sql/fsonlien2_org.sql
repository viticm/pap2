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
# Structure for the `account_info` table :
#

DROP TABLE IF EXISTS `account_info`;

CREATE TABLE `account_info` (
  `Id` int(11) NOT NULL auto_increment,
  `name` varchar(50) default NULL,
  `password` varchar(50) default NULL,
  PRIMARY KEY  (`Id`),
  UNIQUE KEY `name` (`name`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Structure for the `ace` table :
#

DROP TABLE IF EXISTS `ace`;

CREATE TABLE `ace` (
  `ACE_ID` int(11) NOT NULL auto_increment,
  `OBJECT_ID` int(11) default NULL,
  `OPERATOR_ID` int(11) default NULL,
  `OPERATION` int(11) default NULL,
  PRIMARY KEY  (`ACE_ID`)
) ENGINE=MyISAM AUTO_INCREMENT=109 DEFAULT CHARSET=gb2312;

#
# Structure for the `batch_task` table :
#

DROP TABLE IF EXISTS `batch_task`;

CREATE TABLE `batch_task` (
  `Id` int(11) NOT NULL default '0',
  `automation` blob,
  `step` int(6) default NULL,
  `server_ids` blob,
  PRIMARY KEY  (`Id`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Structure for the `fseye_object` table :
#

DROP TABLE IF EXISTS `fseye_object`;

CREATE TABLE `fseye_object` (
  `ID` int(11) NOT NULL auto_increment,
  `OWNER` int(11) default NULL,
  `PARENT_ID` int(11) default NULL,
  `NAME` varchar(50) default NULL,
  `COMMENT` varchar(50) default NULL,
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM AUTO_INCREMENT=109 DEFAULT CHARSET=gb2312;

#
# Structure for the `game_log` table :
#

DROP TABLE IF EXISTS `game_log`;

CREATE TABLE `game_log` (
  `ID` int(11) NOT NULL auto_increment,
  `GAME_SERVER_ID` int(11) default NULL,
  `LOG_DATE` date default NULL,
  `TOTAL_MONEY` bigint(20) default NULL,
  `TOTAL_MONEY_IN_BOX` bigint(20) default NULL,
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Structure for the `game_server` table :
#

DROP TABLE IF EXISTS `game_server`;

CREATE TABLE `game_server` (
  `ID` int(11) NOT NULL default '0',
  `NET_ADDRESS` varchar(20) default NULL,
  `GAME_DIR` varchar(100) default NULL,
  `MAC_ADDRESS` varchar(20) default NULL,
  `LORD_PORT` int(11) default NULL,
  `MAX_PLAYER` int(11) default NULL,
  `INTRANET_MASK` varchar(20) default NULL,
  `INTERNET_MASK` varchar(20) default NULL,
  `PAYSYS_IP` varchar(20) default NULL,
  `PAYSYS_PORT` int(11) default NULL,
  `PAYSYS_LOGIN_NAME` varchar(20) default NULL,
  `PAYSYS_LOGIN_PWD` varchar(100) default NULL,
  `DB_HOST` varchar(20) default NULL,
  `DB_NAME` varchar(20) default NULL,
  `DB_LOGIN_NAME` varchar(20) default NULL,
  `DB_LOGIN_PWD` varchar(100) default NULL,
  `DB_BACKUP_DIR` varchar(100) default NULL,
  `FSEYE_IP` varchar(20) default NULL,
  `FSEYE_PORT` int(11) default NULL,
  `FSEYE_LOGIN_PWD` varchar(100) default NULL,
  `HAS_BEEN_CONFIG_GUARD` int(11) default NULL,
  `SERVER_TYPE` int(11) default NULL,
  `ConfigString` varchar(3500) default NULL,
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Structure for the `gm_op_log` table :
#

DROP TABLE IF EXISTS `gm_op_log`;

CREATE TABLE `gm_op_log` (
  `Id` int(11) NOT NULL auto_increment,
  `user_id` int(11) default '0',
  `operation` int(6) default NULL,
  `target` blob,
  `server_id` int(11) default '0',
  `description` blob,
  `log_time` datetime default NULL,
  `target_type` int(6) default NULL,
  PRIMARY KEY  (`Id`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Structure for the `ibshop` table :
#

DROP TABLE IF EXISTS `ibshop`;

CREATE TABLE `ibshop` (
  `id` int(11) NOT NULL auto_increment,
  `time_stamp` datetime default NULL,
  `script` blob,
  PRIMARY KEY  (`id`),
  UNIQUE KEY `id` (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Structure for the `player_count` table :
#

DROP TABLE IF EXISTS `player_count`;

CREATE TABLE `player_count` (
  `SERVER_ID` int(11) NOT NULL default '0',
  `RECORD_TIME` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  `MAX_PLAYER_COUNT` int(6) NOT NULL default '0',
  `MIN_PLAYER_COUNT` int(6) NOT NULL default '0',
  `AVERAGE_PLAYER_COUNT` int(6) NOT NULL default '0',
  PRIMARY KEY  (`SERVER_ID`,`RECORD_TIME`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Structure for the `scheduled_task` table :
#

DROP TABLE IF EXISTS `scheduled_task`;

CREATE TABLE `scheduled_task` (
  `ID` int(11) NOT NULL default '0',
  `TASKDATA` blob NOT NULL,
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Structure for the `server_group` table :
#

DROP TABLE IF EXISTS `server_group`;

CREATE TABLE `server_group` (
  `ID` int(11) NOT NULL default '0',
  `TYPE` int(11) NOT NULL default '0',
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Structure for the `server_plugin` table :
#

DROP TABLE IF EXISTS `server_plugin`;

CREATE TABLE `server_plugin` (
  `ID` int(11) NOT NULL auto_increment,
  `GAME_SERVER_ID` int(11) NOT NULL default '-1',
  `PLUGIN_GUID` varchar(50) NOT NULL,
  `PARAMETER_1` int(6) default '0',
  `PARAMETER_2` int(6) default '0',
  `PARAMETER_3` varchar(50) default NULL,
  `PlugIn_Name` varchar(50) default NULL,
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Structure for the `strategy` table :
#

DROP TABLE IF EXISTS `strategy`;

CREATE TABLE `strategy` (
  `ID` int(11) NOT NULL default '0',
  `EVENT` int(11) NOT NULL default '0',
  `AUTOMATION` blob NOT NULL,
  `ENABLED` int(6) NOT NULL default '0',
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Structure for the `system_log` table :
#

DROP TABLE IF EXISTS `system_log`;

CREATE TABLE `system_log` (
  `ID` int(10) unsigned NOT NULL auto_increment,
  `OPERATOR` int(10) unsigned NOT NULL,
  `OPERATION` int(10) unsigned NOT NULL,
  `TARGET` int(10) unsigned NOT NULL,
  `DESCRIPTION` blob NOT NULL,
  `LOGTIME` timestamp NOT NULL default CURRENT_TIMESTAMP,
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM AUTO_INCREMENT=4 DEFAULT CHARSET=gb2312;

#
# Structure for the `update_server` table :
#

DROP TABLE IF EXISTS `update_server`;

CREATE TABLE `update_server` (
  `ID` int(11) NOT NULL auto_increment,
  `FTP_ADDRESS` varchar(20) default NULL,
  `PORT` int(6) default '21',
  `USERNAME` varchar(50) default NULL,
  `PASSWORD` varchar(50) default NULL,
  `LITERAL` varchar(50) default NULL,
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Structure for the `user` table :
#

DROP TABLE IF EXISTS `user`;

CREATE TABLE `user` (
  `ID` int(11) NOT NULL default '0',
  `USER_NAME` varchar(20) default NULL,
  `REAL_NAME` varchar(20) default NULL,
  `PASSWORD` varchar(50) default NULL,
  PRIMARY KEY  (`ID`),
  UNIQUE KEY `USER_NAME` (`USER_NAME`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Structure for the `user_group` table :
#

DROP TABLE IF EXISTS `user_group`;

CREATE TABLE `user_group` (
  `ID` int(11) NOT NULL default '0',
  `GROUP_NAME` varchar(20) default NULL,
  PRIMARY KEY  (`ID`),
  UNIQUE KEY `GROUP_NAME` (`GROUP_NAME`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Structure for the `user_in_group` table :
#

DROP TABLE IF EXISTS `user_in_group`;

CREATE TABLE `user_in_group` (
  `USER_ID` int(11) NOT NULL default '0',
  `GROUP_ID` int(11) NOT NULL default '0',
  `ESTABLISH_DATETIME` datetime default NULL,
  PRIMARY KEY  (`USER_ID`,`GROUP_ID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;




/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;