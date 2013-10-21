//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-8
//      File_base        : UserInfo
//      File_ext         : cs
//      Author           : 徐晓刚
//      Description      : 用户信息类
//
//////////////////////////////////////////////////////////////////////

using System;

/// <summary>
/// 用户信息类
/// </summary>
/// <author>徐晓刚</author>
public class UserInfo
{
	private int _id;

	private string _userName;

	private string _realName;

	private DateTime _loginTime;

	long _loginId;

	public int Id
	{
		get { return _id; }
		set { _id = value; }
	}

	public string UserName
	{
		get { return _userName; }
		set { _userName = value; }
	}

	public string RealName
	{
		get { return _realName; }
		set { _realName = value; }
	}

	public DateTime LoginTime
	{
		get { return _loginTime; }
		set { _loginTime = value; }
	}

	public long LoginId
	{
		get { return _loginId; }
		set { _loginId = value; }
	}

	public UserInfo()
	{
		_id = 0;
		_userName = string.Empty;
		_realName = string.Empty;
		_loginTime = DateTime.Now;
	}

	public override string ToString()
	{
		return _userName + " [" + _id + ", " + _realName + ", " + _loginTime + "]";
	}
}
