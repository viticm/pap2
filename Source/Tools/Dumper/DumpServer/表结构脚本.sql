DROP TABLE IF EXISTS `clientinfo`.`dump_active_machine_statistic`;
CREATE TABLE  `clientinfo`.`dump_active_machine_statistic` (
  `date` date NOT NULL,
  `count` int(10) unsigned NOT NULL,
  `versionex` varchar(32) NOT NULL,
  PRIMARY KEY  USING BTREE (`date`,`versionex`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS `clientinfo`.`dump_base`;
CREATE TABLE  `clientinfo`.`dump_base` (
  `DumpTime` timestamp NOT NULL default CURRENT_TIMESTAMP,
  `UUID` varchar(32) character set utf8 NOT NULL,
  `DumpKey` varchar(32) character set utf8 NOT NULL,
  `DumpAppName` varchar(64) character set utf8 default NULL,
  `Version` varchar(32) character set utf8 default NULL,
  `DumpModule` varchar(32) character set utf8 default NULL,
  `VersionEx` varchar(32) character set utf8 default NULL,
  `Account` varchar(32) character set utf8 default NULL,
  `RoleName` varchar(32) character set utf8 default NULL,
  `RegionName` varchar(64) character set utf8 default NULL,
  `ServerName` varchar(64) character set utf8 default NULL,
  `VideoCard` varchar(64) default NULL,
  `Driver` varchar(32) default NULL,
  `DriverVersion` varchar(32) default NULL,
  `TotalPhysMem` int(10) unsigned default NULL,
  `CPUBrand` varchar(64) default NULL,
  `IP` varchar(32) default NULL,
  `GUID` varchar(32) default NULL,
  KEY `DumpAppName` (`DumpAppName`),
  KEY `DumpModule` (`DumpModule`),
  KEY `UUID` (`UUID`),
  KEY `DumpKey` (`DumpKey`),
  KEY `DumpTime` (`DumpTime`),
  KEY `VersionEx` (`VersionEx`),
  KEY `Index_7` (`GUID`)
) ENGINE=InnoDB DEFAULT CHARSET=gbk ROW_FORMAT=COMPACT COMMENT='InnoDB free: 45287424 kB; InnoDB free: 40998912 kB';

DROP TABLE IF EXISTS `clientinfo`.`dump_file`;
CREATE TABLE  `clientinfo`.`dump_file` (
  `DumpKey` varchar(32) NOT NULL,
  `FileName` varchar(512) NOT NULL,
  `FileLevel` tinyint(3) NOT NULL default '0',
  `Remark` varchar(1024) default NULL,
  `IP` varchar(16) NOT NULL,
  `GUID` varchar(32) default NULL,
  `AutoAnalysisFlag` tinyint(4) NOT NULL default '0',
  `DumpTime` timestamp NOT NULL default CURRENT_TIMESTAMP,
  KEY `Index_1` (`AutoAnalysisFlag`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS `clientinfo`.`dump_player_login_base`;
CREATE TABLE  `clientinfo`.`dump_player_login_base` (
  `LoginTime` timestamp NOT NULL default CURRENT_TIMESTAMP,
  `UUID` varchar(32) NOT NULL,
  `Version` varchar(32) default NULL,
  `VersionEx` varchar(32) default NULL,
  `VideoCard` varchar(128) default NULL,
  `Driver` varchar(32) default NULL,
  `DriverVersion` varchar(32) default NULL,
  `TotalPhysMem` int(10) unsigned default NULL,
  `CPUBrand` varchar(64) default NULL,
  `IP` varchar(32) default NULL,
  KEY `Index_1` USING BTREE (`LoginTime`),
  KEY `Index_2` USING BTREE (`UUID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='InnoDB free: 43284480 kB';

DROP TABLE IF EXISTS `clientinfo`.`dump_state`;
CREATE TABLE  `clientinfo`.`dump_state` (
  `DumpKey` varchar(32) NOT NULL,
  `FileNum` int(10) unsigned NOT NULL default '0',
  `MaxFileNum` int(10) unsigned NOT NULL default '10',
  `State` varchar(2) NOT NULL default '0',
  `Remark` varchar(1024) default NULL,
  `DumpTotalNum` int(10) unsigned NOT NULL default '1',
  `VersionEx` varchar(32) default NULL,
  `Version` varchar(32) default NULL,
  `DumpAppName` varchar(45) default NULL,
  `DumpModule` varchar(45) default NULL,
  `DumpDate` timestamp NOT NULL default CURRENT_TIMESTAMP,
  PRIMARY KEY  USING BTREE (`DumpKey`,`DumpDate`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='InnoDB free: 29956096 kB; InnoDB free: 28418048 kB; InnoDB f';

DROP TABLE IF EXISTS `clientinfo`.`dump_statistic`;
CREATE TABLE  `clientinfo`.`dump_statistic` (
  `StatDate` datetime NOT NULL,
  `VersionEx` varchar(32) default NULL,
  `DumpAppName` varchar(64) NOT NULL,
  `DumpModule` varchar(32) NOT NULL,
  `DumpCount` int(10) unsigned NOT NULL,
  `DumpMachineCount` int(10) unsigned NOT NULL,
  `DumpTypeCount` int(10) unsigned NOT NULL,
  KEY `StatDate` (`StatDate`),
  KEY `VersionEx` (`VersionEx`),
  KEY `DumpAppName` (`DumpAppName`),
  KEY `DumpModule` (`DumpModule`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS `clientinfo`.`dump_system_info`;
CREATE TABLE  `clientinfo`.`dump_system_info` (
  `DumpKey` varchar(32) NOT NULL,
  `VideoCard` varchar(64) default NULL,
  `Driver` varchar(32) default NULL,
  `DriverVersion` varchar(32) default NULL,
  `TotalPhysMem` int(10) unsigned default NULL,
  `CPUBrand` varchar(64) default NULL,
  `IP` varchar(32) default NULL,
  `dumptime` timestamp NULL default CURRENT_TIMESTAMP,
  KEY `DumpKey` USING BTREE (`DumpKey`),
  KEY `VideoCard` USING BTREE (`VideoCard`),
  KEY `Driver` USING BTREE (`Driver`),
  KEY `DriverVersion` (`DriverVersion`),
  KEY `TotalPhysMem` USING BTREE (`TotalPhysMem`),
  KEY `IP` USING BTREE (`IP`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;