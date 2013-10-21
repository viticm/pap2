//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2007-3
//      File_base        : GameServerMonitor
//      File_ext         : cs
//      Author           : 徐晓刚
//      Description      : 游戏服务器监视器
//
//////////////////////////////////////////////////////////////////////

using System;
using System.Collections;
using System.Collections.Generic;
using System.Threading;
using System.Text;

namespace FSEye
{
	/// <summary>
	/// 预警等级
	/// </summary>
	public enum AlertLevel
	{
		Attention,
		Important,
		Serious
	}

	/// <summary>
	/// 预警条件
	/// </summary>
	public abstract class AlertCondition
	{
		AlertLevel _level;

		/// <summary>
		/// 预警等级
		/// </summary>
		public AlertLevel Level
		{
			get { return _level; }
			set { _level = value; }
		}

		public AlertCondition()
		{
			_level = AlertLevel.Attention;
		}

		public AlertCondition(AlertLevel level)
		{
			_level = level;
		}

		/// <summary>
		/// 检查条件是否满足
		/// </summary>
		/// <param name="checkObject">检查对象</param>
		public abstract bool Check(object checkObject);
	}

	/// <summary>
	/// 检查结果
	/// </summary>
	public class CheckResult
	{
		IList _alertConditions;

		/// <summary>
		/// 未能满足的预警条件
		/// </summary>
		public IList AlertConditions
		{
			get { return _alertConditions; }			
		}

		/// <summary>
		/// 没有问题
		/// </summary>
		/// <remarks>判断检查结果是否一切正常，没有任何预警条件不能满足</remarks>
		public bool NoProblem
		{
			get { return (_alertConditions.Count == 0); }
		}

		public CheckResult()
		{
			_alertConditions = new ArrayList();
		}

		public override string ToString()
		{
			StringBuilder str = new StringBuilder();
			foreach(AlertCondition condition in _alertConditions)
			{
				str.Append(condition.ToString());
				str.Append(", ");
			}

			return str.ToString();
		}
	}

	/// <summary>
	/// 游戏服务器监视器
	/// </summary>
	public class GameServerMonitor
	{
		public const long DefaultCheckPeriod = 2000;//默认检测间隔
		public const long DefaultDueTime = 2000;//默认延迟

		IList _alertConditions;

		IList _lastCheckResults;

		Timer _checkGameServerTimer;

		long _checkPeriod = DefaultCheckPeriod;

		long _dueTime = DefaultDueTime;

		public IList AlertConditions
		{
			get { return _alertConditions; }
		}

		public IList LastCheckResults
		{
			get { return _lastCheckResults; }
		}

		public long CheckPeriod
		{
			get { return _checkPeriod; }
			set
			{
				_checkPeriod = value;
				_checkGameServerTimer.Change(_dueTime, _checkPeriod);
			}
		}

		public GameServerMonitor()
		{
            _alertConditions = ArrayList.Synchronized(new ArrayList());
            _lastCheckResults = ArrayList.Synchronized(new ArrayList());
			_checkGameServerTimer = new Timer(new TimerCallback(CheckAllGameServers), null, Timeout.Infinite, Timeout.Infinite);

			//TODO For Debug
			AlertCondition condition = new CpuUsageAlertCondition(50);
			condition.Level = AlertLevel.Attention;
			_alertConditions.Add(condition);
			condition = new DiskUsageAlertCondition(DiskUsageAlertCondition.Mode.FreeDiskSize, 3000);
			_alertConditions.Add(condition);
		}

		public void Start()
		{
			_checkGameServerTimer.Change(_dueTime, _checkPeriod);
		}

		public void Stop()
		{
			_checkGameServerTimer.Change(Timeout.Infinite, Timeout.Infinite);
		}

		public CheckResult Check(GameServer server)
		{
			CheckResult result = new CheckResult();

			foreach(AlertCondition condition in _alertConditions)
			{
				if (!condition.Check(server))
				{
					result.AlertConditions.Add(condition);
				}
			}

			return result;
		}

		private void CheckAllGameServers(object arg)
		{
			_lastCheckResults.Clear();
			foreach (GameServer server in AdminServer.TheInstance.GameServerManager.Servers)
			{
				CheckResult result = Check(server);
				_lastCheckResults.Add(result);
                server.LastCheckResult = result;
			}
		}
	}
}
