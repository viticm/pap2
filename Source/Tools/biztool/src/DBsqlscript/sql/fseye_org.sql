# SQL Manager 2005 Lite for MySQL 3.6.5.1
# ---------------------------------------
# Host     : 192.168.20.198
# Port     : 3306
# Database : fseye


SET FOREIGN_KEY_CHECKS=0;

DROP DATABASE IF EXISTS `fseye`;

CREATE DATABASE `fseye`
    CHARACTER SET 'gb2312'
    COLLATE 'gb2312_chinese_ci';

USE `fseye`;

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

#
# Data for the `ace` table  (LIMIT 0,500)
#

INSERT INTO `ace` (`ACE_ID`, `OBJECT_ID`, `OPERATOR_ID`, `OPERATION`) VALUES
  (1,1,6,7),
  (2,2,6,7),
  (3,3,6,7),
  (4,4,6,7),
  (5,5,6,7),
  (6,6,6,7),
  (7,7,6,7),
  (8,8,6,7),
  (9,9,6,7),
  (10,10,6,7),
  (11,11,6,7),
  (12,12,6,7),
  (13,13,6,7),
  (14,14,6,7),
  (15,15,6,7),
  (16,16,6,7),
  (17,17,6,7),
  (18,18,6,7),
  (19,19,6,7),
  (20,20,6,7),
  (21,21,6,7),
  (22,22,6,7),
  (23,23,6,7),
  (24,24,6,7),
  (25,25,6,7),
  (26,26,6,7),
  (27,27,6,7),
  (28,28,6,7),
  (29,29,6,7),
  (30,30,6,7),
  (31,31,6,7),
  (32,32,6,7),
  (33,33,6,7),
  (34,34,6,7),
  (35,35,6,7),
  (36,36,6,7),
  (37,37,6,7),
  (38,38,6,7),
  (39,39,6,7),
  (40,40,6,7),
  (41,41,6,7),
  (42,42,6,7),
  (43,43,6,7),
  (44,44,6,7),
  (45,45,6,7),
  (46,46,6,7),
  (47,47,6,7),
  (48,48,6,7),
  (49,49,6,7),
  (50,50,6,7),
  (51,51,6,7),
  (52,52,6,7),
  (53,53,6,7),
  (54,54,6,7),
  (55,55,6,7),
  (56,56,6,7),
  (57,57,6,7),
  (58,58,6,7),
  (59,59,6,7),
  (60,60,6,7),
  (61,61,6,7),
  (62,62,6,7),
  (63,63,6,7),
  (64,64,6,7),
  (65,65,6,7),
  (66,66,6,7),
  (67,67,6,7),
  (68,68,6,7),
  (69,69,6,7),
  (70,70,6,7),
  (71,71,6,7),
  (72,72,6,7),
  (73,73,6,7),
  (74,74,6,7),
  (75,75,6,7),
  (76,76,6,7),
  (77,77,6,7),
  (78,78,6,7),
  (79,79,6,7),
  (80,80,6,7),
  (81,81,6,7),
  (82,82,6,7),
  (83,83,6,7),
  (84,84,6,7),
  (85,85,6,7),
  (86,86,6,7),
  (87,87,6,7),
  (88,88,6,7),
  (89,89,6,7),
  (90,90,6,7),
  (91,91,6,7),
  (92,92,6,7),
  (93,93,6,7),
  (94,94,6,7),
  (95,95,6,7),
  (96,96,6,7),
  (97,97,6,7),
  (98,98,6,7),
  (99,99,6,7),
  (100,100,6,7),
  (101,101,6,7),
  (102,102,6,7),
  (103,103,6,7),
  (104,104,6,7),
  (105,105,6,7),
  (106,106,6,7),
  (107,107,6,7),
  (108,108,6,7);

COMMIT;

#
# Data for the `fseye_object` table  (LIMIT 0,500)
#

INSERT INTO `fseye_object` (`ID`, `OWNER`, `PARENT_ID`, `NAME`, `COMMENT`) VALUES
  (1,0,0,'FSEye','FSEye'),
  (2,0,1,'System','System'),
  (3,0,2,'SecurityManager','SecurityManager'),
  (4,0,3,'Users','Users'),
  (5,0,3,'Groups','Groups'),
  (6,0,4,'blaze','blaze'),
  (7,0,2,'GameServerManager','GameServerManager'),
  (8,0,7,'GameServerList','GameServerList'),
  (9,0,8,'ServerGroup1','ServerGroup1'),
  (10,0,2,'AutomationManager','AutomationManager'),
  (11,0,2,'StrategyManager','StrategyManager'),
  (12,0,2,'ScheduledTaskManager','ScheduledTaskManager'),
  (13,0,2,'ServiceSwitch','ServiceSwitch'),
  (14,0,3,'Login','Login'),
  (15,0,3,'Logout','Logout'),
  (16,0,1,'Function','Function'),
  (17,0,16,'System','System'),
  (18,0,17,'SystemControl','SystemControl'),
  (19,0,17,'SystemConfig','SystemConfig'),
  (20,0,16,'Security','Security'),
  (21,0,20,'ListUser','ListUser'),
  (22,0,20,'ListGroup','ListGroup'),
  (23,0,20,'EditGroup','EditGroup'),
  (24,0,20,'EditPrivilege','EditPrivilege'),
  (25,0,20,'UserGroup','UserGroup'),
  (26,0,20,'EditUser','EditUser'),
  (27,0,20,'Who','Who'),
  (28,0,20,'AddUser','AddUser'),
  (29,0,20,'Log','Log'),
  (30,0,20,'AddGroup','AddGroup'),
  (31,0,20,'DeleteUser','DeleteUser'),
  (32,0,16,'GameMaster','GameMaster'),
  (33,0,32,'MessageCenter','MessageCenter'),
  (34,0,32,'GameSetting','GameSetting'),
  (35,0,32,'GMCommand','GMCommand'),
  (36,0,32,'PlayerWho','PlayerWho'),
  (37,0,32,'QueryRole','QueryRole'),
  (38,0,32,'RoleDetail','RoleDetail'),
  (39,0,16,'GameInfo','GameInfo'),
  (40,0,39,'Statistic','Statistic'),
  (41,0,40,'EasyStatistic','EasyStatistic'),
  (42,0,40,'RoleCountStatistic','RoleCountStatistic'),
  (43,0,40,'AdvancedStatistic','AdvancedStatistic'),
  (44,0,40,'NpcDeathStatistic','NpcDeathStatistic'),
  (45,0,40,'ExpStatistic','ExpStatistic'),
  (46,0,39,'TopPlayer','TopPlayer'),
  (47,0,16,'UpdateServer','UpdateServer'),
  (48,0,47,'UpdateManagement','UpdateManagement'),
  (49,0,47,'UploadFiles','UploadFiles'),
  (50,0,47,'UpdateServerConfig','UpdateServerConfig'),
  (51,0,47,'AddUpdateServer','AddUpdateServer'),
  (52,0,47,'EditUpdateServer','EditUpdateServer'),
  (53,0,47,'UpdateServerState','UpdateServerState'),
  (54,0,16,'Automation','Automation'),
  (55,0,54,'ScheduledTaskList','ScheduledTaskList'),
  (56,0,54,'StrategyList','StrategyList'),
  (57,0,54,'AddScheduledTask','AddScheduledTask'),
  (58,0,54,'AddStrategy','AddStrategy'),
  (59,0,16,'GameServer','GameServer'),
  (60,0,59,'ServerList','ServerList'),
  (61,0,59,'ServerOverview','ServerOverview'),
  (62,0,59,'ServerControl','ServerControl'),
  (63,0,59,'Console','Console'),
  (64,0,59,'AlertConfig','AlertConfig'),
  (65,0,59,'UploadFile','UploadFile'),
  (66,0,59,'ListFile','ListFile'),
  (67,0,59,'EditServer','EditServer'),
  (68,0,59,'DeleteServer','DeleteServer'),
  (69,0,59,'PlayerCount','PlayerCount'),
  (70,0,59,'ServerGroupManagement','ServerGroupManagement'),
  (71,0,59,'ServerDetail','ServerDetail'),
  (72,0,59,'AdvancedPlayerCount','AdvancedPlayerCount'),
  (73,0,2,'UpdateManagement','UpdateManagement'),
  (74,0,73,'UpdateServer','UpdateServer'),
  (75,0,9,'ServerGroup1_1','ServerGroup1_1'),
  (76,0,59,'AlertCondition','AlertCondition'),
  (77,0,20,'DeleteGroup','DeleteGroup'),
  (78,0,40,'ActivePlayerStatistic','ActivePlayerStatistic'),
  (79,0,40,'AuctionStatistic','AuctionStatistic'),
  (80,0,40,'ItemStatistic','ItemStatistic'),
  (81,0,40,'LevelDistribution','LevelDistribution'),
  (82,0,40,'MoneyStatistic','MoneyStatistic'),
  (83,0,40,'NpcDropItemStatistic','NpcDropItemStatistic'),
  (84,0,40,'ProfessionalSexStatistic','ProfessionalSexStatistic'),
  (85,0,40,'RoleCountStatistic','RoleCountStatistic'),
  (86,0,40,'TaiSuiStatistic','TaiSuiStatistic'),
  (87,0,32,'ChatMonitor','ChatMonitor'),
  (88,0,32,'ItemExchange','ItemExchange'),
  (89,0,32,'ItemFlow','ItemFlow'),
  (90,0,32,'OperationQuery','OperationQuery'),
  (91,0,32,'PlayerMonitor','PlayerMonitor'),
  (92,0,32,'RoleItemInfo','RoleItemInfo'),
  (93,0,32,'RoleSkillInfo','RoleSkillInfo'),
  (94,0,32,'SendMail','SendMail'),
  (95,0,54,'BatchTask','BatchTask'),
  (96,0,54,'BatchTaskDetail','BatchTaskDetail'),
  (97,0,54,'EditBatchTask','EditBatchTask'),
  (98,0,16,'Paysys','Paysys'),
  (99,0,98,'AccountInfo','AccountInfo'),
  (100,0,98,'AccountPassword','AccountPassword'),
  (101,0,98,'AccountState','AccountState'),
  (102,0,98,'GatewayInfo','GatewayInfo'),
  (103,0,98,'PaysysManagement','PaysysManagement'),
  (104,0,40,'MailStatistic','MailStatistic'),
  (105,0,40,'SocialRelationStatistic','SocialRelationStatistic'),
  (106,0,40,'ClanMemberStatistic','ClanMemberStatistic'),
  (107,0,59,'DatabaseInfo','DatabaseInfo'),
  (108,0,2,'BatchTaskManager','BatchTaskManager');

COMMIT;

#
# Data for the `server_group` table  (LIMIT 0,500)
#

INSERT INTO `server_group` (`ID`, `TYPE`) VALUES
  (9,1),
  (75,0);

COMMIT;

#
# Data for the `system_log` table  (LIMIT 0,500)
#

INSERT INTO `system_log` (`ID`, `OPERATOR`, `OPERATION`, `TARGET`, `DESCRIPTION`, `LOGTIME`) VALUES
  (1,6,2,14,'[blaze]从[127.0.0.1]登入系统','2008-03-26 19:37:56'),
  (2,6,2,15,'[blaze]登出系统','2008-03-26 19:38:03'),
  (3,6,2,14,'[blaze]从[127.0.0.1]登入系统','2008-03-26 19:38:18');

COMMIT;

#
# Data for the `user` table  (LIMIT 0,500)
#

INSERT INTO `user` (`ID`, `USER_NAME`, `REAL_NAME`, `PASSWORD`) VALUES
  (6,'blaze','BlazeStudio','2EsWR5BsW9yHtEFKlMrfsw==');

COMMIT;

