//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 2007-5
//      File_base        : ScheduledTask
//      File_ext         : cs
//      Author           : 徐晓刚
//      Description      : 计划任务
//
//////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Collections;
using System.Threading;
using System.Data;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;
using FSEye.Security;
using FSEye.DAL;

namespace FSEye
{
	/// <summary>
	/// 计划任务类型
	/// </summary>
	public enum ScheduledTaskType
	{
		Once,
		Repeat,
		DayOfWeek,
		DayOfMonth
	}

	/// <summary>
	/// 计划任务接口
	/// </summary>
	public interface IScheduledTask : ISecurity, ISerializable 
	{		
		/// <summary>
		/// 计划任务类型
		/// </summary>
		ScheduledTaskType Type { get; }

		/// <summary>
		/// 下一次执行时间
		/// </summary>
		DateTime NextExecuteTime { get; }

		/// <summary>
		/// 自动化对象
		/// </summary>
		IAutomation Automation { get; set; }

		/// <summary>
		/// 是否启用
		/// </summary>
		bool Enabled { get; set; }

		/// <summary>
		/// 执行
		/// </summary>
		void Execute();
	}

	/// <summary>
	/// 计划任务管理单元
	/// </summary>
	public class ScheduledTaskUnit
	{
		public IScheduledTask Task = null;
		public long AvoidCheckTimeout = 0;
	}

	/// <summary>
	/// 计划任务管理器
	/// </summary>
	public class ScheduledTaskManager : ISecurity, IDisposable
	{
        const string SecurityObjectScheduledTaskManager = "/FSEye/System/ScheduledTaskManager";

		const long MaxAvoidCheckTimeout = 120;//最大免检超时

		const long CheckTaskPeriodTimeSpanTicks = 15 * 1000 * 1000 * 10;//检查间隔（tick）

		const int CheckTaskPeriod = 1000 * 15;//检查间隔（毫秒）

		FSEyeObject _securityObject;
		
		Timer _checkTaskTimer;//检查定时器

		IList _taskList;//任务管理单元列表

		bool _on;//是否打开

		bool _disposed = false;

		public FSEyeObject SecurityObject
		{
			get { return _securityObject; }
			set { _securityObject = value; }
		}

		/// <summary>
		/// 是否打开
		/// </summary>
		public bool On
		{
			get { return _on; }
			set { _on = value; }
		}

		/// <summary>
		/// 计划任务管理单元列表
		/// </summary>
		public IList ScheduledTaskList
		{
			get { return _taskList; }
		}
				
		public ScheduledTaskManager()
        {
            _taskList = ArrayList.Synchronized(new ArrayList());
			_on = false;
			_checkTaskTimer = new Timer(new TimerCallback(ProcessCheckTasks), null, 5000, CheckTaskPeriod);
        }

		/// <summary>
		/// 载入计划任务
		/// </summary>
		public void Load()
		{
            this.SecurityObject = AdminServer.TheInstance.SecurityManager.Get(SecurityObjectScheduledTaskManager);
			_taskList.Clear();

			using (IBlazeDatabase db = DbFactory.GetDatabase())
			{
				IBlazeTable scheduledTaskTable = db.GetTable(TableString.ScheduledTaskTableName);
				DataSet ds = new DataSet();
				scheduledTaskTable.Get(ds);
				DataTable table = ds.Tables[TableString.ScheduledTaskTableName];
				foreach (DataRow row in table.Rows)
				{
                    try
                    {
                        int id = (int)row[TableString.ScheduledTaskFieldId];
                        byte[] taskData = (byte[])row[TableString.ScheduledTaskFieldTaskData];

                        MemoryStream taskDataStream = new MemoryStream(taskData);
                        IFormatter formatter = new BinaryFormatter();
                        IScheduledTask task = (IScheduledTask)formatter.Deserialize(taskDataStream);
                        if (task != null)
                        {
                            FSEyeObject secObj = AdminServer.TheInstance.SecurityManager.Get(id);
                            if (secObj != null)
                            {
                                task.SecurityObject = secObj;

                                ScheduledTaskUnit unit = new ScheduledTaskUnit();
                                unit.Task = task;
                                unit.AvoidCheckTimeout = 0;
                                _taskList.Add(unit);
                            }
                        }
                    }
                    catch (Exception e)
                    {
                        Util.DebugLog(e.StackTrace);
                        continue;
                    }
				}
			}
		}

		/// <summary>
		/// 得到计划任务
		/// </summary>
        public ScheduledTaskUnit Get(int taskId)
		{
			for (int i = 0; i < _taskList.Count; i++)
			{
				ScheduledTaskUnit unit = _taskList[i] as ScheduledTaskUnit;
				if (unit.Task.SecurityObject.Id == taskId)
				{
					return unit;
				}
			}

			return null;
		}

		/// <summary>
		/// 启用计划任务
		/// </summary>
		public bool Enable(int taskId, bool enabled)
		{
            ScheduledTaskUnit taskUnit = Get(taskId);
            if (taskUnit != null)
			{
                if (taskUnit.Task.Enabled != enabled)
				{
					taskUnit.Task.Enabled = enabled;
					Update(taskId, taskUnit.Task, taskUnit.Task.SecurityObject.Name, taskUnit.Task.SecurityObject.Comment);
					return true;
				}
			}

			return false;
		}

		/// <summary>
		/// 添加计划任务
		/// </summary>
		public void Add(IScheduledTask task, string name, string comment)
		{
			if (task == null)
				throw new ArgumentNullException("task");
			if (name == null)
				throw new ArgumentNullException("name");
			if (comment == null)
				throw new ArgumentNullException("comment");

			using (IBlazeDatabase db = DbFactory.GetDatabase())
			{
				try
				{
					db.BeginTrans();

					SecurityManager sm = AdminServer.TheInstance.SecurityManager;
					FSEyeObject newObject = sm.Get(SecurityObject.FullPath + SecurityManager.ObjectPathDelimiter + name, db);
					byte[] automationData = AdminServer.TheInstance.AutomationManager.Save(task.Automation);

					MemoryStream taskDataStream = new MemoryStream();
					IFormatter formatter = new BinaryFormatter();
					formatter.Serialize(taskDataStream, task);
					byte[] taskData = taskDataStream.ToArray();
					if (newObject != null && automationData != null && taskData != null)
					{
						newObject.Comment = comment;
						sm.Set(newObject);
						task.SecurityObject = newObject;

						IBlazeTable scheduledTaskTable = db.GetTable(TableString.ScheduledTaskTableName);
						DataSet ds = new DataSet();
						scheduledTaskTable.Get(ds);
						DataTable table = ds.Tables[TableString.ScheduledTaskTableName];
						DataRow row = table.NewRow();
						row[TableString.ScheduledTaskFieldId] = newObject.Id;
						row[TableString.ScheduledTaskFieldTaskData] = taskData;
						table.Rows.Add(row);
						scheduledTaskTable.Set(table);

						ScheduledTaskUnit unit = new ScheduledTaskUnit();
						unit.Task = task;
						unit.AvoidCheckTimeout = 0;
						_taskList.Add(unit);

                        //设置权限
                        AdminServer.TheInstance.SecurityManager.CopyAce(newObject.Parent, newObject, true);

						db.CommitTrans();
					}
				}
				catch (Exception)
				{
					db.RollbackTrans();
				}
			}
		}

		/// <summary>
		/// 删除计划任务
		/// </summary>
		/// <param name="taskId">计划任务ID</param>		
		public bool Delete(int taskId)
		{
			for (int i = 0; i < _taskList.Count; i++)
			{
				ScheduledTaskUnit unit = _taskList[i] as ScheduledTaskUnit;
				if (unit.Task.SecurityObject.Id == taskId)
				{
					using (IBlazeDatabase db = DbFactory.GetDatabase())
					{
						try
						{
							db.BeginTrans();

							AdminServer.TheInstance.SecurityManager.Delete(taskId, db);

							IBlazeTable scheduledTaskTable = db.GetTable(TableString.ScheduledTaskTableName);
							scheduledTaskTable.Delete(FilterFactory.CreateEqualFilter(TableString.ScheduledTaskFieldId, taskId));

                            AdminServer.TheInstance.SecurityManager.Delete(taskId, db);

							_taskList.RemoveAt(i);

							db.CommitTrans();
							return true;
						}
						catch (Exception ex)
						{
							if (db != null)
								db.RollbackTrans();

							throw ex;
						}
					}
				}
			}

			return false;
		}

		/// <summary>
		/// 更新计划任务
		/// </summary>
		public void Update(int taskId, IScheduledTask newTask, string name, string comment)
		{
			using (IBlazeDatabase db = DbFactory.GetDatabase())
			{
				try
				{
					db.BeginTrans();

					SecurityManager sm = AdminServer.TheInstance.SecurityManager;
					FSEyeObject taskObject = sm.Get(taskId);
					taskObject.Name = name;
					taskObject.Comment = comment;
					sm.Set(taskObject, db);
					newTask.SecurityObject = taskObject;

					using (MemoryStream taskDataStream = new MemoryStream())
					{
						IFormatter formatter = new BinaryFormatter();
						formatter.Serialize(taskDataStream, newTask);
						byte[] taskData = taskDataStream.ToArray();
						DataSet ds = new DataSet();
						IBlazeTable scheduledTaskTable = db.GetTable(TableString.ScheduledTaskTableName);
						scheduledTaskTable.Get(ds);
						DataTable table = ds.Tables[TableString.ScheduledTaskTableName];
						DataRow[] rows = table.Select(TableString.ScheduledTaskFieldId + "=" + newTask.SecurityObject.Id);
						if (rows.Length > 0)
						{
							DataRow row = rows[0];
							row[TableString.ScheduledTaskFieldTaskData] = taskData;
							scheduledTaskTable.Set(table);
						}
					}

					foreach (ScheduledTaskUnit unit in _taskList)
					{
						if (unit.Task.SecurityObject.Id == taskId)
						{
							unit.AvoidCheckTimeout = 0;
							unit.Task = newTask;
							break;
						}
					}

					db.CommitTrans();
				}
				catch (Exception ex)
				{
					if (db != null)
						db.RollbackTrans();

					throw ex;
				}
			}
		}

		/// <summary>
		/// 检查计划任务
		/// </summary>
		void ProcessCheckTasks(object state)
		{
			if (!_on)
				return;

			DateTime now = DateTime.Now;
			foreach (ScheduledTaskUnit unit in _taskList)
			{
				if (unit.Task.Enabled)
				{
					unit.AvoidCheckTimeout--;
					if (unit.AvoidCheckTimeout <= 0)
					{
						DateTime nextExeTime = unit.Task.NextExecuteTime;
						if (nextExeTime != DateTime.MinValue)
						{
							long spanTicks = nextExeTime.Subtract(now).Ticks;
							//时间到了，执行任务
							if (spanTicks >= 0)
							{
								if (spanTicks < CheckTaskPeriodTimeSpanTicks)
								{
									unit.Task.Execute();
								}
								else if (spanTicks < (CheckTaskPeriodTimeSpanTicks * 5))
								{
									unit.AvoidCheckTimeout = 0;
								}
								else if (spanTicks < (CheckTaskPeriodTimeSpanTicks * 10))
								{
									unit.AvoidCheckTimeout = 5;
								}
								else
								{
									unit.AvoidCheckTimeout = 10;
								}
							}
						}
						else
						{
							unit.AvoidCheckTimeout = MaxAvoidCheckTimeout;
						}
					}
				}
			}
		}

		public void Dispose()
		{
			if (!_disposed)
			{
				if (_checkTaskTimer != null)
					_checkTaskTimer.Dispose();
				if (_taskList != null)
					_taskList.Clear();
			}
		}
	}

	/// <summary>
	/// 计划任务基类
	/// </summary>
	public abstract class BaseScheduledTask : IScheduledTask
	{
		FSEyeObject _securityObject;

		byte[] _automationData;

		bool _enabled;

		public FSEyeObject SecurityObject
		{
			get { return _securityObject; }
			set { _securityObject = value; }
		}

		public bool Enabled
		{
			get { return _enabled; }
			set { _enabled = value; }
		}

		public IAutomation Automation
		{
			get
			{
				return AdminServer.TheInstance.AutomationManager.Load(_automationData);
			}

			set
			{
				_automationData = AdminServer.TheInstance.AutomationManager.Save(value);
			}
		}

		public void Execute()
		{
			AdminServer.TheInstance.AutomationManager.Do(this.Automation);
		}

		public abstract ScheduledTaskType Type { get; }

		public abstract DateTime NextExecuteTime { get; }

		public BaseScheduledTask()
		{
		}

		protected BaseScheduledTask(SerializationInfo info, StreamingContext context)
		{
			_enabled = info.GetBoolean("enabled");
			_automationData = (byte[])info.GetValue("automation", typeof(byte[]));
		}

		public virtual void GetObjectData(SerializationInfo info, StreamingContext context)
		{
			info.AddValue("enabled", _enabled);
			info.AddValue("automation", _automationData, typeof(byte[]));
		}
	}

	/// <summary>
	/// 单次执行的计划任务
	/// </summary>
	/// <remarks>例如：2007-5-12 15:00:00执行</remarks>
	[Serializable]
	public class OnceScheduledTask : BaseScheduledTask
	{
		DateTime _executeTime;

		/// <summary>
		/// 执行时间
		/// </summary>
		public DateTime ExecuteTime
		{
			get { return _executeTime; }
			set { _executeTime = value; }
		}

		public override ScheduledTaskType Type
		{
			get { return ScheduledTaskType.Once; }
		}

		public override DateTime NextExecuteTime
		{
			get
			{
				if (_executeTime >= DateTime.Now)
					return _executeTime;
				else
					return DateTime.MinValue;
			}
		}

		public OnceScheduledTask()
		{
		}

		public override string ToString()
		{
			return string.Format("{0} [{1}] [Once: {2}]", SecurityObject.Name, SecurityObject.Comment, _executeTime);
		}

		protected OnceScheduledTask(SerializationInfo info, StreamingContext context)
			: base(info, context)
		{
			_executeTime = info.GetDateTime("ExecuteTime");
		}

		public override void GetObjectData(SerializationInfo info, StreamingContext context)
		{
			base.GetObjectData(info, context);
			info.AddValue("ExecuteTime", _executeTime);
		}
	}

	/// <summary>
	/// 一段时间内间隔一定时间执行的任务
	/// </summary>
	/// <remarks>例如：从5:00:00到12:00:00，每隔15分钟执行一次</remarks>
	[Serializable]
	public class RepeatScheduledTask : BaseScheduledTask
	{
		TimeSpan _startTime;

		TimeSpan _endTime;

		TimeSpan _interval;

		/// <summary>
		/// 开始时间
		/// </summary>
		public TimeSpan StartTime
		{
			get { return _startTime; }
			set { _startTime = value; }
		}

		/// <summary>
		/// 结束时间
		/// </summary>
		public TimeSpan EndTime
		{
			get { return _endTime; }
			set { _endTime = value; }
		}

		/// <summary>
		/// 间隔时间
		/// </summary>
		public TimeSpan Interval
		{
			get { return _interval; }
			set { _interval = value; }
		}

		public override ScheduledTaskType Type
		{
			get { return ScheduledTaskType.Repeat; }
		}

		public override DateTime NextExecuteTime
		{
			get
			{
				DateTime now = DateTime.Now;
				TimeSpan currentTime = now.TimeOfDay;
				if (_startTime >= currentTime)
				{
					return now.Date.Add(_startTime);
				}
				else if (_endTime >= currentTime)
				{
					//TODO 这里有待考虑
					TimeSpan span = currentTime.Subtract(_startTime);
					long period = span.Ticks / _interval.Ticks;
					return now.Date.Add(_startTime.Add(new TimeSpan((period + 1) * _interval.Ticks)));
				}
				else
				{
					return DateTime.MinValue;
				}
			}
		}

		public RepeatScheduledTask()
		{
		}

		public override string ToString()
		{
			return string.Format("{0} [{1}] [Repeat: From {2} to {3}, interval={4}]", SecurityObject.Name, SecurityObject.Comment, _startTime, _endTime, _interval);
		}

		protected RepeatScheduledTask(SerializationInfo info, StreamingContext context)
			: base(info, context)
		{
			_startTime = new TimeSpan((long)info.GetInt64("StartTime"));
			_endTime = new TimeSpan((long)info.GetInt64("EndTime"));
			_interval = new TimeSpan((long)info.GetInt64("Interval"));
		}

		public override void GetObjectData(SerializationInfo info, StreamingContext context)
		{
			base.GetObjectData(info, context);
			info.AddValue("StartTime", _startTime.Ticks);
			info.AddValue("EndTime", _endTime.Ticks);
			info.AddValue("Interval", _interval.Ticks);
		}
	}

	/// <summary>
	/// 每周的某天的计划任务
	/// </summary>
	/// <remarks>例如：每周二2:00:00执行</remarks>
    [Serializable]
	public class DayOfWeekScheduledTask : BaseScheduledTask
	{
		DayOfWeek[] _dayOfWeekArray;

		TimeSpan _timeOfDay;

		public DayOfWeek[] DayOfWeek
		{
			get { return _dayOfWeekArray; }
			set { _dayOfWeekArray = value; }
		}

		public TimeSpan TimeOfDay
		{
			get { return _timeOfDay; }
			set { _timeOfDay = value; }
		}

		public override ScheduledTaskType Type
		{
			get { return ScheduledTaskType.DayOfWeek; }
		}

		public override DateTime NextExecuteTime
		{
			get
			{
				DateTime now = DateTime.Now;
				DayOfWeek currentDayOfWeek = now.DayOfWeek;
				TimeSpan currentTimeOfDay = now.TimeOfDay;

				//TODO 这里需要添加逻辑
				
				foreach (DayOfWeek day in _dayOfWeekArray)
				{
					if (day == currentDayOfWeek)
					{
						if (_timeOfDay >= currentTimeOfDay)
						{
							return DateTime.Today.Add(_timeOfDay);
						}
					}
                    if (day > currentDayOfWeek)
                    {
                        return DateTime.Today.AddDays((double)(day - currentDayOfWeek)).Add(_timeOfDay);
                    }
				}
                return DateTime.Today.AddDays((double)(7 + _dayOfWeekArray[0] - currentDayOfWeek)).Add(_timeOfDay);
			}
		}

		public DayOfWeekScheduledTask()
		{
		}

        public override string ToString()
        {
            StringBuilder days = new StringBuilder();
            if (_dayOfWeekArray != null)
            {
                foreach (DayOfWeek day in _dayOfWeekArray)
                {
                    days.Append(day.ToString());
                    days.Append(',');
                }
            }
            return string.Format("{0} [{1}] [DayOfWeek: {2} {3} ]", SecurityObject.Name, SecurityObject.Comment, days.ToString(), _timeOfDay);
        }

        public DayOfWeekScheduledTask(SerializationInfo info, StreamingContext context)
            : base(info, context)
        {
            _dayOfWeekArray = (DayOfWeek[])info.GetValue("DayOfWeekArray", typeof(DayOfWeek[]));
            _timeOfDay = new TimeSpan((long)info.GetInt64("TimeOfDay"));
        }

        public override void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            base.GetObjectData(info, context);
            info.AddValue("DayOfWeekArray", _dayOfWeekArray, typeof(DayOfWeek[]));
            info.AddValue("TimeOfDay", _timeOfDay.Ticks);
        }
	}
}