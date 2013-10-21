//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-9-14
//      File_base        : IPlugIn
//      File_ext         : cs
//      Author           : 徐晓刚
//      Description      : 插件接口
//
//////////////////////////////////////////////////////////////////////

namespace FSEye
{
	/// <summary>
	/// 插件接口
	/// </summary>
	/// <author>徐晓刚</author>
	public interface IPlugIn
	{
		/// <summary>
		/// 插件名称
		/// </summary>
		string Name	{ get; }

		/// <summary>
		/// 作者
		/// </summary>
		string Author { get; }

		/// <summary>
		/// GUID
		/// </summary>
		string Guid { get; }

        /// <summary>
        /// 参数的描述
        /// </summary>
        string[] ParaDescription { get; }

        /// <summary>
        /// 参数
        /// 注:第一个object返回该PlugIn的参数个数
        /// </summary>
        object[] Parameters { set; get; }

		/// <summary>
		/// 初始化
		/// </summary>
		void Initialize(GameServer server);

		/// <summary>
		/// 销毁
		/// </summary>
		void Dispose();

		/// <summary>
		/// 执行插件行为
		/// </summary>
		/// <param name="actionName">行为键</param>
		/// <param name="args">参数</param>
		bool DoAction(int userId,string actionKey, ref object outArg, params object[] args);
				
		/// <summary>
		/// 得到插件数据
		/// </summary>
		/// <param name="dataKey">数据键</param>
		object GetData(int userId,string dataKey, params object[] args);
	}

	/// <summary>
	/// 插件基类
	/// </summary>
	/// <author>徐晓刚</author>
	public abstract class BasePlugIn : IPlugIn
	{
		protected GameServer _server;

		public abstract string Name { get; }

		public abstract string Author { get; }

		public abstract string Guid { get; }

		public abstract string[] ParaDescription { get; }

		public abstract object[] Parameters { get; set; }

		public virtual void Initialize(GameServer server)
		{
			_server = server;
		}

		public abstract void Dispose();

		public abstract bool DoAction(int userId,string actionKey, ref object outArg, params object[] args);

		public abstract object GetData(int userId,string dataKey, params object[] args);
	}
}
