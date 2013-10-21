<?php

/**
 * @author 
 * @copyright 2009
 */
date_default_timezone_set('Asia/Chongqing');
require_once('database.class.php');

$db['dbtype']  = "mysql";
$db['dbhost']  ="127.0.0.1";
$db['dbname']  ="dumpdb";
$db['dbuser']  ="root";
$db['dbpwd']   ="mysql";


if(!empty($db))
{		
		// 实例化数据库对象
    $DatabaseHandler  = new DatabaseHandler($db['dbhost'], $db['dbuser'], $db['dbpwd'], $db['dbname']);
        
    unset($db);// 清除此数组，使得以后不可用，避免数据库信息外漏
}
else
{
    die("无数据库设置");
}


function cleanInfo(& $info) 
{
    if (!get_magic_quotes_gpc()) 
    {
        if (is_array($info)) 
        {
            foreach ($info as $key => $value) 
            {
                $info[$key] = cleanInfo($value);
            }
        } 
        else 
        {
            $info = preg_replace("/^[\n\t\r]+/", "", $info);
            $info = preg_replace("/^[ ]{2,}/", "  ", $info);
            return addslashes(rtrim($info));
        }
        return $info;
    } 
    else 
    {
        if (is_array($info)) 
        {
            foreach ($info as $key => $value) 
            {
                $info[$key] = cleanInfo($value);
            }
        }
        else 
        {
            $info = preg_replace("/^[\n\t\r]+/", "", $info);
            $info = preg_replace("/^[ ]{2,}/", "  ", $info);             
            return rtrim($info);
        }        
        return $info;
    } 
}


?>