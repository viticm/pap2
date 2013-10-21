//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-10
//      File_base        : OperationResult
//      File_ext         : cs
//      Author           : 徐晓刚
//      Description      : 操作结果
//
//////////////////////////////////////////////////////////////////////

using System;

/// <summary>
/// 消息类型
/// </summary>
public enum MessageType
{
	Success,
	Failure,
	Attention,
	Normal
}

public class OperationResult
{
	private string _message;

	private MessageType _type;

	/// <summary>
	/// 消息
	/// </summary>
	public string Message
	{
		get { return _message; }
	}

	/// <summary>
	/// 消息类型
	/// </summary>
	public MessageType Type
	{
		get { return _type; }
	}

	public OperationResult(string message, MessageType type)
	{
		_message = message;
		_type = type;
	}
}