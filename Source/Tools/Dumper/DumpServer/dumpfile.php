<?php

/**
 * @author 
 * @copyright 2009
 */
	require_once('lib/config.php');	
			
	//$DumpFileDirectory="C:\\wamp\\www\\jx3\\upload\\";
	$DumpFileDirectory="/opt/data/www/clientreport/dump/";
	if( !isset($_POST['uuid']) || !isset($_POST['key']) ){
		echo "参数错误.";
		exit(-1);		
	}
	
	if($_FILES['File']['name']!=NULL)
	{
	$arrFile = explode( '.', strtolower($_FILES['File']['name']) );
	//$ReceiveFileName=date(YmdHis).$_POST['key'].".".$arrFile[count($arrFile) - 1 ];
	$ReceiveFileName=date("YmdHis")."_".$_POST['filename']."_".$_POST['ver']."_".$_POST['exceptionmodule']."_".$_POST['guid'].".zip";
	}else{
		$ReceiveFileName=NULL;
		echo "没有文件.";
		exit(-1);
	}
	    
	$uuid	=	cleanInfo(strtolower($_POST['uuid']));
	$key	=	cleanInfo($_POST['key']);
	$ExceptionModule	=	cleanInfo($_POST['exceptionmodule']);
	$FileLevel = cleanInfo($_POST['filelevel']);
	$ip		=	cleanInfo($_SERVER['SERVER_ADDR']);
	$guid = cleanInfo($_POST['guid']);
	
	//查询是否有该KEY Dump_State
	$sql = sprintf("select count(*) from `dump_state` where `DumpKey` = '%s' and datediff(`DumpDate`, '%s') = 0", $key, date("Y-m-d"));
	$num = $DatabaseHandler->getOne($sql);
	
	if ($num == 0)
	{		
		//新KEY		
		if(move_uploaded_file($_FILES['File']['tmp_name'], $DumpFileDirectory.$ReceiveFileName))
		{
			$sql = sprintf("INSERT INTO `dump_file`(`DumpKey` , `FileName`, `FileLevel`, `IP`, `GUID`)	VALUES ('%s','%s',%d, '%s', '%s')",
	 					$key,$ReceiveFileName,$FileLevel, $ip, $guid);					
			$DatabaseHandler->Execute($sql);
									
			$sql = sprintf("INSERT INTO `dump_state` ( `DumpKey` , `FileNum` )
	  					VALUES ('%s', %d)",
	 					$key, 1);					
			$DatabaseHandler->Execute($sql);		
		}
	}
	else
	{
		//普通文件
		if ($FileLevel == 0)
		{						
			$sql = sprintf("select sum(FileNum) from `dump_state` where `DumpKey` = '%s'", $key);
			$fileNum = $DatabaseHandler->getOne($sql);			
			
			$sql = sprintf("select MaxFileNum from `dump_state` where `DumpKey` = '%s' and datediff(`DumpDate`, '%s') = 0", $key, date("Y-m-d"));
			$maxFileNum = $DatabaseHandler->getOne($sql);
			
			if ($fileNum < $maxFileNum)
			{
				if(move_uploaded_file($_FILES['File']['tmp_name'], $DumpFileDirectory.$ReceiveFileName))
				{
					$sql = sprintf("INSERT INTO `dump_file` ( `DumpKey` , `FileName`, `FileLevel`, `IP`, `GUID`)
			  					VALUES ('%s','%s',%d,'%s', '%s')",
			 					$key,$ReceiveFileName,$FileLevel,$ip,$guid);					
					$DatabaseHandler->Execute($sql);
					
					$sql = sprintf("update `dump_state` set `FileNum` = `FileNum` + 1 where `DumpKey` = '%s' and datediff(`DumpDate`, '%s') = 0", $key, date("Y-m-d"));
					$DatabaseHandler->Execute($sql);
				}
			}	
		}
		else
		{
			if(move_uploaded_file($_FILES['File']['tmp_name'], $DumpFileDirectory.$ReceiveFileName))
			{
				$sql = sprintf("INSERT INTO `dump_file` ( `DumpKey` , `FileName`, `FileLevel`, `IP`, `GUID`)
		  					VALUES ('%s','%s',%d,'%s','%s')",
		 					$key,$ReceiveFileName,$FileLevel,$ip,$guid);					
				$DatabaseHandler->Execute($sql);
				
				$sql = sprintf("update `dump_state` set `FileNum` = `FileNum` + 1 where `DumpKey` = '%s' and datediff(`DumpDate`, '%s') = 0", $key, date("Y-m-d"));
				$DatabaseHandler->Execute($sql);
			}
		}
	}
	
	$DatabaseHandler->close();
	exit(0);
?>
