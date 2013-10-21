<?php
/**
 * 数据库操作类
 *
 * Copyright(c) 2005 by 刘晨光. All rights reserved
 *
 * To contact the author write to {@link mailto:chenguangliu@21cn.com}
 *
 * @author 刘晨光
 * @version 1.1
 * @touch   email:chenguangliu@21cn.com msn:liuchenguang_pro@hotmail.com qq:64452627
 * @package Base Class
 */
/*if(!defined('IN_SYS')) {
        exit('Access Denied');
}*/

class DatabaseHandler
{
	/**
     * 数据库连接句柄
     *
     * @var     resource
     * @access  private
     */
	var $conn;

    /**
     * 查询后返回的结果句柄
     *
     * @var resource
     * @access  private
     */
	var $resource;

    /**
     * 当前执行的SQL
     *
     * @var string
     * @access  private
     */
	var $debug = false;

    /**
     * 当前执行的SQL
     *
     * @var string
     * @access  private
     */
	var $sql;

    /**
     * 所有的SQL
     *
     * @var string
     * @access  private
     */
	var $sqls = array();

	/**
     * 初始化数据库连接并选择默认数据库
     *
     * @param   string  $host		//主机
     * @param   string  $user		//用户名
     * @param   string  $password	//密码
     * @param   string  $database	//数据库
	 * @return  boolean
     * @access  public
     */
	function DatabaseHandler($host,$user,$password,$database)
	{		
		$this->conn = @mysql_connect($host,$user,$password);
		if(!$this->conn)
		{						
			$this->exception("数据连接失败!");
			return false;
		}

	  
		$this->select($database,$this->conn);

        $sql = "SET NAMES 'UTF8'";
        $this->execute($sql);
	}

	/**
     * 选择数据库
     *
     * @param   string  $database	//数据库
	 * @return  boolean
     * @access  public
     */
	function select($database)
	{
		if(!mysql_select_db($database))
		{
			$this->exception("数据库打开失败!");
			return false;
		}
	}

    /**
     * 执行sql语句
     *
     * @param   string  $sql	//执行SELECT，SHOW，EXPLAIN 或 DESCRIBE 返回 resource,其他的返回 boolean
	 * @return  boolean
     * @access  public
     */
	function Execute($sql)
	{
		if($sql == "")
		{
			$this->exception("SQL语句不能为空!");
		}

		if(!$this->conn)
		{
			$this->exception("数据库连接失败!");
		}

		$this->sql    = $sql;
		$this->sqls[] = $sql;

		if($this->debug == true)
		{
			print "<hr />$sql<hr />";
		}
		
		$result = mysql_query($sql,$this->conn);
		
		if(is_resource($result))
		{
			$this->resource = $result;
			$result = new RecordSet($result);
			$record = $this->fetch();
			if(is_array($record))
			{
				$result->fields = $record;
			}
			else
			{
			    $result->EOF = true;
			}
		}

		return $result;
	}

    /**
     * 从结果集中取得一行作为关联数组，或数字数组，或二者兼有
     *
     * @param   int     $mode	//数据库查询模式
	 * @return  array
     * @access  public
     */
	function fetch($mode = MYSQL_ASSOC)
	{
		if(!is_resource($this->resource))
			$this->exception("SQL查询语句无效");	
		
		return mysql_fetch_array($this->resource,$mode);
	}

    /**
     * 释放结果内存
     *
	 * @return  void
     * @access  public
     */
	function free()
	{
		mysql_free_result($this->resource);
	}

    /**
     * 关闭数据库连接
     *
	 * @return  void
     * @access  public
     */
	function close()
	{
		if(is_resource($this->resource))
		{
			$this->free();
		}

		mysql_close($this->conn);
	}

    /**
     * 根据执行结果取得影响行数
     *
     * @return int
     * @access  public
     */
	function getAffectedRow()
	{
		return mysql_affected_rows();
	}

    /**
     * 根据查询结果计算结果集条数
     *
     * @return int
     * @access  public
     */
	function getFetchNum()
	{
		return mysql_num_rows($this->resource);
	}

    /**
     * 取得查询结果的第一行第一列
     *
     * @param string $sql
     * @return mixed 一般是数值或者字符串
     * @access  public
     */
	function getOne($sql)
	{
		$rs = $this->Execute($sql);
		
		return is_array($rs->fields) ? array_shift($rs->fields) : false;
	}

    /**
     * 取得查询结果的第一行
     *
     * @param string $sql SQL语句
     * @param const $fetch_mode
     * @return array
     * @access  public
     */
	function getRow($sql)
	{
		$rs = $this->Execute($sql);

		return $rs->fields;
	}

    /**
     * 取得 SQL 结果的所有行
     *
     * @param string $sql
     * @return array
     * @access  public
     */
	function getAll($sql)
	{
		$rs = $this->Execute($sql);
		
		$record = array();

		if(is_array($rs->fields))
		{
			$record[0] = $rs->fields;
			$record[0]['index'] = 1;
		}
		
		$i = 1;// 索引计数
		while($row = $this->fetch())
		{
			$record[$i] = $row;
			$record[$i]['index'] = $i+1;
			$i++;
		}

		return $record;
	}

	/**
	 * 返回最后写入的记录编号
	 *
	 * @param    void
	 * @return   int
	 * @access   public
	 */
	function Insert_ID()
	{
		return mysql_insert_id($this->conn);
	}

    /**
     * 获取插入语句
     *
     * @param    string     $tbl_name   表名
     * @param    array      $info       数据
     */
    function getInsertSql($tbl_name,$info)
    {
        if(is_array($info)&&!empty($info))
        {
            $i = 0;
            foreach($info as $key=>$val)
            {
                $fields[$i] = $key;
                $values[$i] = addslashes($val);
                $i++;
            }
            $s_fields = "(".implode(",",$fields).")";
            $s_values  = "('".implode("','",$values)."')";
            $sql = "INSERT INTO
                        $tbl_name
                        $s_fields
                    VALUES
                        $s_values";
            Return $sql;
        }
        else
        {
            Return false;
        }
    }

    /**
     * 多数据插入
     *
     * @param    string     $tbl_name       表名
     * @param    array      $data           数据，必须是形如 $array[索引][字段名]才能正确执行
     */
    function getMultiInsertSql($tbl_name, $data, $comment='')
    {
        if(is_array($data) && !empty($data))
        {
            $v_str = array();
            $field_list = array();
            foreach($data as $k=>$v)
            {
                if(empty($field_list))
                {
                    foreach($v as $kk=>$kv)
                    {
                        $field_list[] = $kk;
                    }
                }
                
                $v_str[] = "('".implode("','",$v)."')";
            }
            $s_fields = "(".implode(",", $field_list).")";
            $s_values = implode(",", $v_str);

            if(!empty($comment))
            {
                $comment = "/* ".$comment." */";
            }
            $sql = "{$comment}
                    INSERT INTO
                        $tbl_name
                        $s_fields
                    VALUES
                        $s_values";
            Return $sql;
        }
        else
        {
            Return false;
        }
    }
    
    /**
     * 获取替换语句
     *
     * @param    string     $tbl_name   表名
     * @param    array      $info       数据
     */
    function getReplaceSql($tbl_name,$info)
    {
        if(is_array($info)&&!empty($info))
        {
            $i = 0;
            foreach($info as $key=>$val)
            {
                $fields[$i] = $key;
                $values[$i] = addslashes($val);
                $i++;
            }
            $s_fields = "(".implode(",",$fields).")";
            $s_values  = "('".implode("','",$values)."')";
            $sql = "REPLACE INTO
                        $tbl_name
                        $s_fields
                    VALUES
                        $s_values";
            Return $sql;
        }
        else
        {
            Return false;
        }
    }

    /**
     * 更新记录
     *
     * @param    string     $tbl_name   表名
     * @param    array      $info       数据
     * @param    array      $condition  条件
     */
    function getUpdateSql($tbl_name,$info,$condition)
    {
        $i = 0;
        $data = '';
        if(is_array($info)&&!empty($info))
        {
            foreach( $info as $key=>$val )
            {
                if(isset($val))
                {
                    $val = addslashes($val);
                    if($i==0&&$val!==null)
                    {
                        $data = $key."='".$val."'";
                    }
                    else
                    {
                        $data .= ",".$key." = '".$val."'";
                    }
                    $i++;
                }
            }	

            if(!empty($condition))
            {
                $condition = " WHERE ".$condition;
            }
            $sql = "UPDATE ".$tbl_name." SET ".$data.$condition;

            return $sql;
        }
        else
        {
            Return false;
        }
    }

    /**
     * 返回指定列数据信息
     *
     * @param   string  $condition	//条件
	 * @param   string  $fields
	 *
	 * @param   string  $table      //表名
	 * @return  array
     * @access  public
     */
	function getMap($fields,$table,$condition = "")
	{

		$sql = "SELECT ".$fields." FROM ".$table.$condition;

		return $this->getAll($sql);
	}

    /**
     * 数据库异常处理
     *
     * @param   string  $message	//信息
	 * @return  void
     * @access  public
     */
	function exception($message)
	{
		global $config;
		$error = @mysql_error($this->conn);
		$errno = @mysql_errno($this->conn);
		$time  = date("Y年m月d日 H时i分s秒");
		$sql   = $this->sql;

		$content = <<<EOT
<style>
.main{
	font-size:12px;
	width:100%;
}			
</style>
<div class="main">
	<div style="font-weight:bold;">站点错误报告：</div>
	<div width="80%" style="text-align:left">
	<p>
　　当您来到这个页面的时候，代表着这里出现了一个严重的错误。<br />
　　请您尝试 <a href="">点击这里</a> 来刷新页面。或者 <a href="/">点击这里</a>
返回站点首页。如果问题还没有解决，请尝试 <a href="mailto: zengjian@kingsoft.com">联系管理员</a> 来解决此问题。</p>
  <ul>
    <li>错误编号:[$error:$errno]</li>
    <li>出错时间:[$time]</li>
    <li>出错信息:[$message]</li>
    <li>执行操作:[$sql]</li>
  </ul>
	</div>
</div>
EOT;

		exit($content);
	}
}

/**
 * 查询数据集类
 * 
 * @abstract
 * @access public
 */
class RecordSet
{
	/**
	 * 是否到数据集尾
	 * @access public
	 */
	var $EOF = false;
	
	/**
	 * 返回数据记录
	 * @access public
	 */
	var $fields;

	/**
	 * 查询句柄
	 * @access public
	 */
	var $resultId;
	
	/**
	 * 查询方式
	 * @access public
	 */
	var $fetchMode = MYSQL_ASSOC;
	
	/**
	 * 构造函数
	 *
	 * @param    resource    $resultId    查询句柄
	 */
	function RecordSet(&$resultId)
	{
		$this->resultId = $resultId;
	}

	/**
	 * 设置结果集模式
	 *
	 * @param    static    $fetchmode    结果集模式
	 */
	function setFetchMode($fetchmode = MYSQL_ASSOC)
	{
		$this->fetchMode = $fetchmode;
	}

	/**
	 * 返回下一条记录
	 * 
	 * @access public 
	 */
	function MoveNext()
	{
		if ($this->fields = mysql_fetch_array($this->resultId,$this->fetchMode))
		{
			return true;
		}

		if (!$this->EOF)
		{
			$this->EOF = true;
		}

		return false;
	}
}
?>