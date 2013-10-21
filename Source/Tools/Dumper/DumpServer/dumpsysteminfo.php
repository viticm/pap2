<?php

/**
 * @author 
 * @copyright 2009
 */

	require_once('lib/config.php');
					
	if( !isset($_POST['key']) ){
		echo "²ÎÊý´íÎó";
		exit(-1);		
	}
								
	$key	=	cleanInfo($_POST['key']);
	$videocard	=	cleanInfo($_POST['videocard']);
	$driverversion = cleanInfo($_POST['driverversion']);
	$cpubrand	=	cleanInfo($_POST['cpubrand']);
  $driver	=	cleanInfo($_POST['driver']);
	$totalphysmem = cleanInfo($_POST['totalphysmem']);
	$ip = cleanInfo($_SERVER['REMOTE_ADDR']);
	
	$sql = sprintf("INSERT INTO `dump_system_info` ( `DumpKey`, `VideoCard`, `DriverVersion`, `CPUBrand`, `Driver`, `TotalPhysMem`, `IP`)
	  					VALUES ('%s','%s', '%s', '%s', '%s', '%d', '%s')",
	 					$key, $videocard, $driverversion, $cpubrand, $driver, $totalphysmem, $ip);					
	$DatabaseHandler->Execute($sql);
	
	$DatabaseHandler->close();
	
	exit(0);
?>