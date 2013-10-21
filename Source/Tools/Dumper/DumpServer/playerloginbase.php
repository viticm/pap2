<?php

/**
 * @author 
 * @copyright 2009
 */

	require_once('lib/config.php');
					
	if( !isset($_POST['uuid']) || !isset($_POST['ver']) || !isset($_POST['verex'])){
		echo "²ÎÊý´íÎó";
		exit(-1);		
	}
				
	$uuid	=	cleanInfo(strtolower($_POST['uuid']));
	$version	=	cleanInfo($_POST['ver']);
	$versionex = cleanInfo($_POST['verex']);
  $videocard	=	cleanInfo($_POST['videocard']);
	$driverversion = cleanInfo($_POST['driverversion']);
	$cpubrand	=	cleanInfo($_POST['cpubrand']);
  $driver	=	cleanInfo($_POST['driver']);
	$totalphysmem = cleanInfo($_POST['totalphysmem']);	
	$today = cleanInfo(date("Y-m-d H:i:s"));	
	$ip = cleanInfo($_SERVER['REMOTE_ADDR']);

	$sql = sprintf("select count(*) from `dump_player_login_base` where `UUID` = '%s' and DATEDIFF(`LoginTime`, '%s') = 0", $uuid, $today);
	$num = $DatabaseHandler->getOne($sql);
	
	if ($num == 0)
	{
		$sql = sprintf("INSERT INTO `dump_player_login_base` ( 
								`UUID` , `Version`, `VersionEx`, 
								`VideoCard`, `Driver`, `DriverVersion`, `TotalPhysMem`, `CPUBrand`, `IP`)
	  					VALUES (
	  						'%s','%s','%s',
	  						'%s', '%s', '%s', '%d', '%s', '%s')",
	 							$uuid,  $version, $versionex, 
	 							$videocard, $driver, $driverversion, $totalphysmem, $cpubrand, $ip);					
		$DatabaseHandler->Execute($sql);
	}
	
	$DatabaseHandler->close();
	
	exit(0);
?>