<?php

/**
 * @author 
 * @copyright 2009
 */

	require_once('lib/config.php');
					
	if( !isset($_POST['uuid']) || !isset($_POST['key'])){
		echo "²ÎÊý´íÎó";
		exit(-1);		
	}
				
	$uuid	=	cleanInfo(strtolower($_POST['uuid']));
	$key	=	cleanInfo($_POST['key']);
	$clientname	=	cleanInfo($_POST['clientname']);
	$version	=	cleanInfo($_POST['ver']);
	$versionex = cleanInfo($_POST['verex']);
	$exceptionmodule	=	cleanInfo($_POST['exceptionmodule']);
	$account = cleanInfo($_POST['account']);
	$rolename = cleanInfo($_POST['rolename']);
	$regionname = cleanInfo($_POST['regionname']);
	$servername = cleanInfo($_POST['servername']);
  $videocard	=	cleanInfo($_POST['videocard']);
	$driverversion = cleanInfo($_POST['driverversion']);
	$cpubrand	=	cleanInfo($_POST['cpubrand']);
  $driver	=	cleanInfo($_POST['driver']);
	$totalphysmem = cleanInfo($_POST['totalphysmem']);
	$ip = cleanInfo($_SERVER['REMOTE_ADDR']);	
	$guid = cleanInfo($_POST['guid']);
	
	$sql = sprintf("set names gbk");					
	$DatabaseHandler->Execute($sql);
	
	$sql = sprintf("INSERT INTO `dump_base` ( 
								`UUID` , `DumpKey`, `DumpAppName`, `Version`, `VersionEx`, `DumpModule`, 
								`Account`, `RoleName`, `RegionName`, `ServerName`,
								`VideoCard`, `Driver`, `DriverVersion`, `TotalPhysMem`, `CPUBrand`, `IP`, `GUID`)
	  					VALUES (
	  						'%s','%s','%s', '%s', '%s', '%s', 
	  						'%s', '%s', '%s', '%s', 
	  						'%s', '%s', '%s', '%d', '%s', '%s', '%s')",
	 							$uuid, $key, $clientname, $version, $versionex, $exceptionmodule, 
	 							$account, $rolename, $regionname, $servername,
	 							$videocard, $driver, $driverversion, $totalphysmem, $cpubrand, $ip, $guid);					
	$DatabaseHandler->Execute($sql);
	
	$DatabaseHandler->close();
	
	exit(0);
?>