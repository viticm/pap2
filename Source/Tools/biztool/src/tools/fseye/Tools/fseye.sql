CREATE DATABASE IF NOT EXISTS fseye;

DROP TABLE IF EXISTS `fseye`.`ace`;
CREATE TABLE  `fseye`.`ace` (
  `ACE_ID` int(11) NOT NULL auto_increment,
  `OBJECT_ID` int(11) default NULL,
  `OPERATOR_ID` int(11) default NULL,
  `OPERATION` int(11) default NULL,
  PRIMARY KEY  (`ACE_ID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

DROP TABLE IF EXISTS `fseye`.`account_info`;
CREATE TABLE `account_info` (
  `Id` int(11) NOT NULL auto_increment,
  `name` varchar(50) default NULL,
  `password` varchar(50) default NULL,
  PRIMARY KEY  (`Id`),
  UNIQUE KEY `name` (`name`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

DROP TABLE IF EXISTS `fseye`.`fseye_object`;
CREATE TABLE  `fseye`.`fseye_object` (
  `ID` int(11) NOT NULL auto_increment,
  `OWNER` int(11) default NULL,
  `PARENT_ID` int(11) default NULL,
  `NAME` varchar(50) default NULL,
  `COMMENT` varchar(50) default NULL,
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

DROP TABLE IF EXISTS `fseye`.`game_log`;
CREATE TABLE  `fseye`.`game_log` (
  `ID` int(11) NOT NULL auto_increment,
  `GAME_SERVER_ID` int(11) default NULL,
  `LOG_DATE` date default NULL,
  `TOTAL_MONEY` bigint(20) default NULL,
  `TOTAL_MONEY_IN_BOX` bigint(20) default NULL,
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

DROP TABLE IF EXISTS `fseye`.`game_server`;
CREATE TABLE  `fseye`.`game_server` (
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
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

DROP TABLE IF EXISTS `fseye`.`server_group`;
CREATE TABLE  `fseye`.`server_group` (
  `ID` int(11) NOT NULL default '0',
  `TYPE` int(11) NOT NULL default '0',
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

DROP TABLE IF EXISTS `fseye`.`server_plugin`;
CREATE TABLE  `fseye`.`server_plugin` (
  `ID` int(11) NOT NULL auto_increment,
  `GAME_SERVER_ID` int(11) NOT NULL default '-1',
  `PLUGIN_GUID` varchar(50) NOT NULL,
  `PARAMETER_1` int(6) default '0',
  `PARAMETER_2` int(6) default '0',
  `PARAMETER_3` varchar(50) default NULL,
  `PlugIn_Name` varchar(50) default NULL,
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

DROP TABLE IF EXISTS `fseye`.`strategy`;
CREATE TABLE  `fseye`.`strategy` (
  `ID` int(11) NOT NULL default '0',
  `EVENT` int(11) NOT NULL default '0',
  `AUTOMATION` blob NOT NULL,
  `ENABLED` int(6) NOT NULL default '0',
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

DROP TABLE IF EXISTS `fseye`.`scheduled_task`;
CREATE TABLE  `fseye`.`scheduled_task` (
  `ID` int(11) NOT NULL default '0',
  `TASKDATA` blob NOT NULL,
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

DROP TABLE IF EXISTS `fseye`.`update_server`;
CREATE TABLE  `fseye`.`update_server` (
  `ID` int(11) NOT NULL auto_increment,
  `FTP_ADDRESS` varchar(20) default NULL,
  `PORT` int(6) default '21',
  `USERNAME` varchar(50) default NULL,
  `PASSWORD` varchar(50) default NULL,
  `LITERAL` varchar(50) default NULL,
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

DROP TABLE IF EXISTS `fseye`.`user`;
CREATE TABLE  `fseye`.`user` (
  `ID` int(11) NOT NULL default '0',
  `USER_NAME` varchar(20) default NULL,
  `REAL_NAME` varchar(20) default NULL,
  `PASSWORD` varchar(50) default NULL,
  PRIMARY KEY  (`ID`),
  UNIQUE KEY `USER_NAME` (`USER_NAME`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

DROP TABLE IF EXISTS `fseye`.`user_group`;
CREATE TABLE  `fseye`.`user_group` (
  `ID` int(11) NOT NULL default '0',
  `GROUP_NAME` varchar(20) default NULL,
  PRIMARY KEY  (`ID`),
  UNIQUE KEY `GROUP_NAME` (`GROUP_NAME`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

DROP TABLE IF EXISTS `fseye`.`user_in_group`;
CREATE TABLE  `fseye`.`user_in_group` (
  `USER_ID` int(11) NOT NULL default '0',
  `GROUP_ID` int(11) NOT NULL default '0',
  `ESTABLISH_DATETIME` datetime default NULL,
  PRIMARY KEY  (`USER_ID`,`GROUP_ID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

DROP TABLE IF EXISTS `fseye`.`system_log`;
CREATE TABLE  `fseye`.`system_log` (
  `ID` int(10) unsigned NOT NULL auto_increment,
  `OPERATOR` int(10) unsigned NOT NULL,
  `OPERATION` int(10) unsigned NOT NULL,
  `TARGET` int(10) unsigned NOT NULL,
  `DESCRIPTION` blob NOT NULL,
  `LOGTIME` timestamp NOT NULL default CURRENT_TIMESTAMP,
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

DROP TABLE IF EXISTS `fseye`.`player_count`;
CREATE TABLE  `fseye`.`player_count` (
  `SERVER_ID` int(11) NOT NULL default '0',
  `RECORD_TIME` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  `MAX_PLAYER_COUNT` int(6) NOT NULL default '0',
  `MIN_PLAYER_COUNT` int(6) NOT NULL default '0',
  `AVERAGE_PLAYER_COUNT` int(6) NOT NULL default '0',
  PRIMARY KEY  (`SERVER_ID`,`RECORD_TIME`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

DROP TABLE IF EXISTS `fseye`.`gm_op_log`;
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

DROP TABLE IF EXISTS `fseye`.`batch_task`;
CREATE TABLE `batch_task` (
  `Id` int(11) NOT NULL default '0',
  `automation` blob,
  `step` int(6) default NULL,
  `server_ids` blob,
  PRIMARY KEY  (`Id`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

USE fseye;

INSERT INTO ace(object_id,operator_id,operation) VALUES (1,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (2,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (3,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (4,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (5,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (6,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (7,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (8,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (9,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (10,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (11,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (12,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (13,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (14,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (15,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (16,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (17,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (18,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (19,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (20,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (21,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (22,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (23,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (24,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (25,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (26,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (27,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (28,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (29,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (30,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (31,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (32,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (33,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (34,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (35,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (36,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (37,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (38,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (39,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (40,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (41,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (42,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (43,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (44,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (45,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (46,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (47,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (48,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (49,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (50,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (51,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (52,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (53,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (54,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (55,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (56,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (57,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (58,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (59,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (60,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (61,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (62,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (63,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (64,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (65,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (66,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (67,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (68,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (69,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (70,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (71,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (72,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (73,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (74,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (75,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (76,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (77,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (78,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (79,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (80,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (81,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (82,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (83,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (84,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (85,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (86,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (87,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (88,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (89,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (90,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (91,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (92,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (93,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (94,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (95,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (96,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (97,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (98,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (99,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (100,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (101,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (102,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (103,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (104,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (105,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (106,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (107,6,7);
INSERT INTO ace(object_id,operator_id,operation) VALUES (108,6,7);

INSERT INTO fseye_object VALUES (1,0,0,'FSEye','FSEye');
INSERT INTO fseye_object VALUES (2,0,1,'System','System');
INSERT INTO fseye_object VALUES (3,0,2,'SecurityManager','SecurityManager');
INSERT INTO fseye_object VALUES (4,0,3,'Users','Users');
INSERT INTO fseye_object VALUES (5,0,3,'Groups','Groups');
INSERT INTO fseye_object VALUES (6,0,4,'blaze','blaze');
INSERT INTO fseye_object VALUES (7,0,2,'GameServerManager','GameServerManager');
INSERT INTO fseye_object VALUES (8,0,7,'GameServerList','GameServerList');
INSERT INTO fseye_object VALUES (9,0,8,'ServerGroup1','ServerGroup1');
INSERT INTO fseye_object VALUES (10,0,2,'AutomationManager','AutomationManager');
INSERT INTO fseye_object VALUES (11,0,2,'StrategyManager','StrategyManager');
INSERT INTO fseye_object VALUES (12,0,2,'ScheduledTaskManager','ScheduledTaskManager');
INSERT INTO fseye_object VALUES (13,0,2,'ServiceSwitch','ServiceSwitch');
INSERT INTO fseye_object VALUES (14,0,3,'Login','Login');
INSERT INTO fseye_object VALUES (15,0,3,'Logout','Logout');
INSERT INTO fseye_object VALUES (16,0,1,'Function','Function');
INSERT INTO fseye_object VALUES (17,0,16,'System','System');
INSERT INTO fseye_object VALUES (18,0,17,'SystemControl','SystemControl');
INSERT INTO fseye_object VALUES (19,0,17,'SystemConfig','SystemConfig');
INSERT INTO fseye_object VALUES (20,0,16,'Security','Security');
INSERT INTO fseye_object VALUES (21,0,20,'ListUser','ListUser');
INSERT INTO fseye_object VALUES (22,0,20,'ListGroup','ListGroup');
INSERT INTO fseye_object VALUES (23,0,20,'EditGroup','EditGroup');
INSERT INTO fseye_object VALUES (24,0,20,'EditPrivilege','EditPrivilege');
INSERT INTO fseye_object VALUES (25,0,20,'UserGroup','UserGroup');
INSERT INTO fseye_object VALUES (26,0,20,'EditUser','EditUser');
INSERT INTO fseye_object VALUES (27,0,20,'Who','Who');
INSERT INTO fseye_object VALUES (28,0,20,'AddUser','AddUser');
INSERT INTO fseye_object VALUES (29,0,20,'Log','Log');
INSERT INTO fseye_object VALUES (30,0,20,'AddGroup','AddGroup');
INSERT INTO fseye_object VALUES (31,0,20,'DeleteUser','DeleteUser');
INSERT INTO fseye_object VALUES (32,0,16,'GameMaster','GameMaster');
INSERT INTO fseye_object VALUES (33,0,32,'MessageCenter','MessageCenter');
INSERT INTO fseye_object VALUES (34,0,32,'GameSetting','GameSetting');
INSERT INTO fseye_object VALUES (35,0,32,'GMCommand','GMCommand');
INSERT INTO fseye_object VALUES (36,0,32,'PlayerWho','PlayerWho');
INSERT INTO fseye_object VALUES (37,0,32,'QueryRole','QueryRole');
INSERT INTO fseye_object VALUES (38,0,32,'RoleDetail','RoleDetail');
INSERT INTO fseye_object VALUES (39,0,16,'GameInfo','GameInfo');
INSERT INTO fseye_object VALUES (40,0,39,'Statistic','Statistic');
INSERT INTO fseye_object VALUES (41,0,40,'EasyStatistic','EasyStatistic');
INSERT INTO fseye_object VALUES (42,0,40,'RoleCountStatistic','RoleCountStatistic');
INSERT INTO fseye_object VALUES (43,0,40,'AdvancedStatistic','AdvancedStatistic');
INSERT INTO fseye_object VALUES (44,0,40,'NpcDeathStatistic','NpcDeathStatistic');
INSERT INTO fseye_object VALUES (45,0,40,'ExpStatistic','ExpStatistic');
INSERT INTO fseye_object VALUES (46,0,39,'TopPlayer','TopPlayer');
INSERT INTO fseye_object VALUES (47,0,16,'UpdateServer','UpdateServer');
INSERT INTO fseye_object VALUES (48,0,47,'UpdateManagement','UpdateManagement');
INSERT INTO fseye_object VALUES (49,0,47,'UploadFiles','UploadFiles');
INSERT INTO fseye_object VALUES (50,0,47,'UpdateServerConfig','UpdateServerConfig');
INSERT INTO fseye_object VALUES (51,0,47,'AddUpdateServer','AddUpdateServer');
INSERT INTO fseye_object VALUES (52,0,47,'EditUpdateServer','EditUpdateServer');
INSERT INTO fseye_object VALUES (53,0,47,'UpdateServerState','UpdateServerState');
INSERT INTO fseye_object VALUES (54,0,16,'Automation','Automation');
INSERT INTO fseye_object VALUES (55,0,54,'ScheduledTaskList','ScheduledTaskList');
INSERT INTO fseye_object VALUES (56,0,54,'StrategyList','StrategyList');
INSERT INTO fseye_object VALUES (57,0,54,'AddScheduledTask','AddScheduledTask');
INSERT INTO fseye_object VALUES (58,0,54,'AddStrategy','AddStrategy');
INSERT INTO fseye_object VALUES (59,0,16,'GameServer','GameServer');
INSERT INTO fseye_object VALUES (60,0,59,'ServerList','ServerList');
INSERT INTO fseye_object VALUES (61,0,59,'ServerOverview','ServerOverview');
INSERT INTO fseye_object VALUES (62,0,59,'ServerControl','ServerControl');
INSERT INTO fseye_object VALUES (63,0,59,'Console','Console');
INSERT INTO fseye_object VALUES (64,0,59,'AlertConfig','AlertConfig');
INSERT INTO fseye_object VALUES (65,0,59,'UploadFile','UploadFile');
INSERT INTO fseye_object VALUES (66,0,59,'ListFile','ListFile');
INSERT INTO fseye_object VALUES (67,0,59,'EditServer','EditServer');
INSERT INTO fseye_object VALUES (68,0,59,'DeleteServer','DeleteServer');
INSERT INTO fseye_object VALUES (69,0,59,'PlayerCount','PlayerCount');
INSERT INTO fseye_object VALUES (70,0,59,'ServerGroupManagement','ServerGroupManagement');
INSERT INTO fseye_object VALUES (71,0,59,'ServerDetail','ServerDetail');
INSERT INTO fseye_object VALUES (72,0,59,'AdvancedPlayerCount','AdvancedPlayerCount');
INSERT INTO fseye_object VALUES (73,0,2,'UpdateManagement','UpdateManagement');
INSERT INTO fseye_object VALUES (74,0,73,'UpdateServer','UpdateServer');
INSERT INTO fseye_object VALUES (75,0,9,'ServerGroup1_1','ServerGroup1_1');
INSERT INTO fseye_object VALUES (76,0,59,'AlertCondition','AlertCondition');
INSERT INTO fseye_object VALUES (77,0,20,'DeleteGroup','DeleteGroup');
INSERT INTO fseye_object VALUES (78,0,40,'ActivePlayerStatistic','ActivePlayerStatistic');
INSERT INTO fseye_object VALUES (79,0,40,'AuctionStatistic','AuctionStatistic');
INSERT INTO fseye_object VALUES (80,0,40,'ItemStatistic','ItemStatistic');
INSERT INTO fseye_object VALUES (81,0,40,'LevelDistribution','LevelDistribution');
INSERT INTO fseye_object VALUES (82,0,40,'MoneyStatistic','MoneyStatistic');
INSERT INTO fseye_object VALUES (83,0,40,'NpcDropItemStatistic','NpcDropItemStatistic');
INSERT INTO fseye_object VALUES (84,0,40,'ProfessionalSexStatistic','ProfessionalSexStatistic');
INSERT INTO fseye_object VALUES (85,0,40,'RoleCountStatistic','RoleCountStatistic');
INSERT INTO fseye_object VALUES (86,0,40,'TaiSuiStatistic','TaiSuiStatistic');
INSERT INTO fseye_object VALUES (87,0,32,'ChatMonitor','ChatMonitor');
INSERT INTO fseye_object VALUES (88,0,32,'ItemExchange','ItemExchange');
INSERT INTO fseye_object VALUES (89,0,32,'ItemFlow','ItemFlow');
INSERT INTO fseye_object VALUES (90,0,32,'OperationQuery','OperationQuery');
INSERT INTO fseye_object VALUES (91,0,32,'PlayerMonitor','PlayerMonitor');
INSERT INTO fseye_object VALUES (92,0,32,'RoleItemInfo','RoleItemInfo');
INSERT INTO fseye_object VALUES (93,0,32,'RoleSkillInfo','RoleSkillInfo');
INSERT INTO fseye_object VALUES (94,0,32,'SendMail','SendMail');
INSERT INTO fseye_object VALUES (95,0,54,'BatchTask','BatchTask');
INSERT INTO fseye_object VALUES (96,0,54,'BatchTaskDetail','BatchTaskDetail');
INSERT INTO fseye_object VALUES (97,0,54,'EditBatchTask','EditBatchTask');
INSERT INTO fseye_object VALUES (98,0,16,'Paysys','Paysys');
INSERT INTO fseye_object VALUES (99,0,98,'AccountInfo','AccountInfo');
INSERT INTO fseye_object VALUES (100,0,98,'AccountPassword','AccountPassword');
INSERT INTO fseye_object VALUES (101,0,98,'AccountState','AccountState');
INSERT INTO fseye_object VALUES (102,0,98,'GatewayInfo','GatewayInfo');
INSERT INTO fseye_object VALUES (103,0,98,'PaysysManagement','PaysysManagement');
INSERT INTO fseye_object VALUES (104,0,40,'MailStatistic','MailStatistic');
INSERT INTO fseye_object VALUES (105,0,40,'SocialRelationStatistic','SocialRelationStatistic');
INSERT INTO fseye_object VALUES (106,0,40,'ClanMemberStatistic','ClanMemberStatistic');
INSERT INTO fseye_object VALUES (107,0,59,'DatabaseInfo','DatabaseInfo');
INSERT INTO fseye_object VALUES (108,0,2,'BatchTaskManager','BatchTaskManager');

INSERT INTO server_group VALUES (9,1);
INSERT INTO server_group VALUES (75,0);

INSERT INTO user VALUES (6,'blaze','BlazeStudio','2EsWR5BsW9yHtEFKlMrfsw==');
