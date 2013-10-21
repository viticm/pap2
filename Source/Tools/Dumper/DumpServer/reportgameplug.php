<?php

/**
 * @author 
 * @copyright 2009
 */

	require_once('lib/config.php');
					
	if( !isset($_POST['account']) || !isset($_POST['role']) || !isset($_POST['region']) || !isset($_POST['server'])){
		echo "²ÎÊý´íÎó";
		exit(-1);		
	}
				
	$account = cleanInfo($_POST['account']);
	$role = cleanInfo($_POST['role']);
	$region	= cleanInfo($_POST['region']);
	$server	= cleanInfo($_POST['server']);
	$robot_path = cleanInfo($_POST['robot_path']);
	$game_path = cleanInfo($_POST['game_path']);
	$robot_id = cleanInfo($_POST['robot_id']);

	$sql = sprintf("set names gbk");					
	$DatabaseHandler->Execute($sql);
	
	$sql = sprintf("INSERT INTO `GamePlug` (`account` , `role`, `region`, `server`, `robot_id`, `game_path`, `robot_path`)
	  					    VALUES ('%s','%s','%s', '%s', %d, '%s', '%s')",
	 							  $account, $role, $region, $server, $robot_id, $game_path, $robot_path);
	$DatabaseHandler->Execute($sql);
	
	$DatabaseHandler->close();
	
	exit(0);
?>