//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-9-14
//      File_base        : GameServerDb
//      File_ext         : cs
//      Author           : 徐晓刚
//      Description      : 游戏服务器数据库
//
//////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;

namespace FSEye.PlugIn
{
	/// <summary>
	/// 游戏服务器数据库（操作插件）
	/// </summary>
	public class GameServerDb : BasePlugIn
	{
		public const string PlugInGuid = "{9609275e-205e-47a5-9065-58e7c52e3cbf}";
		public const string ActionKeyQuery = "ActionKeyQuery";
		public const string DataKeyQueryResult = "DataKeyQueryResult";

		object _syncRoot = new object();
		uint _queryCount;
		Hashtable _queryResultCache;

        public override object[] Parameters
        {
            set { }
            get
            {
                return new object[]{
                    0
                };
            }
        }


		public override string[] ParaDescription
        {
            get
            {
                return new string[]{
                    string.Empty
            };
            }
        }

		public override string Name
		{
			get { return "GameServerDb"; }
		}

		public override string Guid
		{
			get { return PlugInGuid; }
		}

		public override string Author
		{
			get { return "xuxiaogang"; }
		}

		public override void Initialize(GameServer server)
		{
			base.Initialize(server);

			_queryCount = 1;
			_queryResultCache = Hashtable.Synchronized(new Hashtable());
			_server.MessageReceived += new MessageReceivedEventHandler(this.MessageReceived);
		}

		public override void Dispose()
		{
			_server.MessageReceived -= new MessageReceivedEventHandler(this.MessageReceived);
		}

		private void MessageReceived(GameServer server, IProtocol message)
		{
			if ((ProtocolDef)message.ProtocolId == ProtocolDef.g2e_exesql_def)
			{
				g2e_exesql protocol = message as g2e_exesql;
				bool success = ((FSEyeResult)protocol.nRetCode == FSEyeResult.fseye_success);
				bool done = ((FSEyeResult)protocol.nRetCode != FSEyeResult.mydb_more_result);
								
				UpdateQueryResultCache(protocol.nSessionID, protocol.szResult, done, success);
			}
		}

        public override bool DoAction(int userId, string actionKey, ref object outArg, params object[] args)
		{
			switch (actionKey)
			{
				case ActionKeyQuery:
					return Query(ref outArg, args);
			}

			throw new Exception(string.Format("ActionKey {0} does not exist.", actionKey));
		}

        public override object GetData(int userId, string dataKey, params object[] args)
		{
			switch (dataKey)
			{
				case DataKeyQueryResult:
					return GetQueryResult(args);
			}

			throw new Exception(string.Format("DataKey {0} does not exist.", dataKey));
		}

		bool Query(ref object outArg, params object[] args)
		{
			if (args == null || args.Length != 2)
				throw new ArgumentException();

			string dbName = args[0] as string;
			SqlCommand cmd = args[1] as SqlCommand;

			uint id = 0;
			lock (_syncRoot)
			{
				id = _queryCount;
				_queryCount++;
			}

			e2g_exesql protocol = new e2g_exesql();
			protocol.nSessionID = id;
			protocol.szDatabase = dbName;
            protocol.wServer = (UInt16)_server.Id;
            protocol.szSql = cmd.SqlBuffer;
            
			if (!_server.SendMessage(protocol))
			{
				throw new Exception("GameServer.QueryGameServerDb: SendMessage failure.");
			}

			outArg = id;
			return true;
		}

		SqlResult GetQueryResult(params object[] args)
		{
			if (args == null || args.Length != 1 || args[0].GetType() != typeof(uint))
				throw new ArgumentException();

			uint sessionId = (uint)args[0];

			lock (_queryResultCache.SyncRoot)
			{
				if (_queryResultCache.Contains(sessionId))
				{
					SqlResult result = _queryResultCache[sessionId] as SqlResult;
					if (result.ResultState == SqlResult.State.Done)
					{
						_queryResultCache.Remove(sessionId);
						return result;
					}
				}
			}

			return null;
		}

		void UpdateQueryResultCache(uint sessionId, byte[] data, bool done, bool success)
		{
			lock (_queryResultCache.SyncRoot)
			{
				SqlResult result = null;
				if (_queryResultCache.Contains(sessionId))
				{
					result = _queryResultCache[sessionId] as SqlResult;
				}
				else
				{
					result = new SqlResult(sessionId);
					_queryResultCache.Add(sessionId, result);
				}

				if (result != null)
				{
					result.AppendData(data);
					if (done)
					{
						result.Done(success);
					}
				}
			}
		}
	}
}
