//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-8
//      File_base        : IMessageHandler
//      File_ext         : cs
//      Author           : 徐晓刚
//      Description      : 消息处理者接口
//
//////////////////////////////////////////////////////////////////////

namespace FSEye
{
	/// <summary>
	/// 消息处理者接口
	/// </summary>
	/// <author>徐晓刚</author>
	public interface IMessageHandler
	{
		/// <summary>
		/// 处理消息
		/// </summary>
		/// <param name="message">消息</param>
		void ProcessMessage(GameServer server, IProtocol message);
	}
}
