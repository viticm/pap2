using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.Remoting.Messaging;
using System.Threading;
using System.Data;
using System.Runtime.Serialization;
using FSEye.Security;
using FSEye.DAL;
using System.IO;

namespace FSEye
{
	/// <summary>
	/// 批量任务单元
	/// </summary>
	public class BatchTaskUnit
	{
		/// <summary>
		/// 状态
		/// </summary>
		public enum State
		{
			Querying,//等待执行
			Doing,//执行中
			Done//已完成
		}

		public int ServerId;

		public State UnitState = State.Querying;
	}

	/// <summary>
	/// 批量任务
	/// </summary>
    public class BatchTask
	{
		public delegate void DelegateDoTask();

        public delegate void BatchTaskComplete(object callBackObject);

		/// <summary>
		/// 状态
		/// </summary>
		public enum State
		{
			Init,//初始化
			Doing,//执行中
			Paused,//已暂停
			Aborted,//已终止
			Done//已完成
		}

		IList<BatchTaskUnit> _taskUnitList = new List<BatchTaskUnit>();

		int _step = 1;

		int _workingUnitCount = 0;

		int _nextUnitIndex = 0;

		string _desc = string.Empty;

		State _state = State.Init;

		bool _needPause = false;

		bool _needAbort = false;

		bool _needContinue = false;

		object _lock = new object();

		byte[] _automationData;

        bool _success = false;

        int _completeServerCount = 0;

		DateTime _startTime = DateTime.MinValue;

		DateTime _endTime = DateTime.MinValue;

		FSEyeObject _securityObject;

        BatchTaskComplete _batchTaskComplete;
        
        object _batchTaskCompleteCallBackObject;

		public FSEyeObject SecurityObject
		{
			get { return _securityObject; }
			set { _securityObject = value; }
		}

        public int ID
        {
            get { return _securityObject.Id; }
        }

        public bool Success
        {
            get { return _success; }
        }

		/// <summary>
		/// 自动化对象
		/// </summary>
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

		/// <summary>
		/// 步长
		/// </summary>
		/// <remarks>即一次同时对多少台服务器进行操作</remarks>
		public int Step
		{
			get { return _step; }
			set { _step = value; }
		}

        /// <summary>
		/// 服务器编号数组
		/// </summary>
		public int[] Servers
		{
			get
			{
				int[] serverArray = new int[_taskUnitList.Count];
				for (int i = 0; i < serverArray.Length; i++)
				{
					serverArray[i] = _taskUnitList[i].ServerId;
				}
				return serverArray;
			}
		}
		
		/// <summary>
		/// 任务描述
		/// </summary>
		public string Desc
		{
			get { return _desc; }
			set { _desc = value; }
		}

		/// <summary>
		/// 任务状态
		/// </summary>
		public State TaskState
		{
			get { return _state; }
		}

		/// <summary>
		/// 开始时间
		/// </summary>
		public DateTime StartTime
		{
			get { return _startTime; }
		}

		/// <summary>
		/// 结束时间
		/// </summary>
		public DateTime EndTime
		{
			get { return _endTime; }
		}

		public BatchTask()
		{
		}

		/// <summary>
		/// 清空所有的服务器
		/// </summary>
		public void ClearServers()
		{
			if (_state == State.Doing)
				throw new InvalidOperationException();

			_taskUnitList.Clear();
		}

		/// <summary>
		/// 添加服务器
		/// </summary>
		/// <param name="serverId"></param>
		public void AddServer(int serverId)
		{
			if (_state == State.Doing)
				throw new InvalidOperationException();

			for (int i = 0; i < _taskUnitList.Count; i++)
			{
				if (_taskUnitList[i].ServerId == serverId)
				{
					return;
				}
			}

			BatchTaskUnit unit = new BatchTaskUnit();
			unit.ServerId = serverId;
			unit.UnitState = BatchTaskUnit.State.Querying;
			_taskUnitList.Add(unit);
		}

		/// <summary>
		/// 删除服务器
		/// </summary>
		/// <param name="serverId"></param>
		public void DeleteServer(int serverId)
		{
			if (_state == State.Doing)
				throw new InvalidOperationException();

			int index = -1;
			for (int i = 0; i < _taskUnitList.Count; i++)
			{
				if (_taskUnitList[i].ServerId == serverId)
				{
					index = i;
				}
			}

			if (index >= 0)
			{
				_taskUnitList.RemoveAt(index);
			}
		}

		public BatchTaskUnit.State GetUnitState(int serverIndex)
		{
			if (serverIndex >= 0 && serverIndex < _taskUnitList.Count)
			{
				return _taskUnitList[serverIndex].UnitState;
			}
			else
			{
				throw new IndexOutOfRangeException();
			}
		}

		/// <summary>
		/// 启动
		/// </summary>
		public void Start()
		{
			lock (_lock)
			{
				if (_state != State.Init)
					throw new InvalidOperationException();

				DelegateDoTask dele = new DelegateDoTask(DoTask);
                _startTime = DateTime.Now;
                _state = State.Doing;
                dele.BeginInvoke(new AsyncCallback(DoTaskComplete), null);
			}
		}

        /// <summary>
        /// 启动
        /// </summary>
        public void Start(BatchTaskComplete batchTaskComplete,object callBackObject)
        {
            if (batchTaskComplete != null)
            {
                _batchTaskComplete = batchTaskComplete;
                _batchTaskCompleteCallBackObject = callBackObject;
            }
            Start();
        }

		/// <summary>
		/// 暂停
		/// </summary>
		public void Pause()
		{
			lock (_lock)
			{
				if (_state != State.Doing)
					throw new InvalidOperationException();

				_needPause = true;
			}
		}

		/// <summary>
		/// 继续
		/// </summary>
		public void Continue()
		{
			lock (_lock)
			{
				if (_state != State.Paused)
					throw new InvalidOperationException();

				_needContinue = true;
			}
		}

		/// <summary>
		/// 终止
		/// </summary>
		public void Stop()
		{
			lock (_lock)
			{
				if (_state != State.Doing)
					throw new InvalidOperationException();

				_needAbort = true;
			}
		}

		public void Reset()
		{
			lock (_lock)
			{
				if (_state != State.Done && _state != State.Init)
					throw new InvalidOperationException();

				foreach (BatchTaskUnit unit in _taskUnitList)
				{
					unit.UnitState = BatchTaskUnit.State.Querying;
				}
				
				_startTime = DateTime.MinValue;
				_endTime = DateTime.MinValue;
				_nextUnitIndex = 0;
				_state = State.Init;
			}
		}

		/// <summary>
		/// 执行任务
		/// </summary>
		/// <remarks>耗时的操作</remarks>
		void DoTask()
		{
			while (!_needAbort && _state != State.Done)
			{
				lock (_lock)
				{
					if (_state == State.Doing)
					{
						if (_needPause)
						{
							_state = State.Paused;
							_needPause = false;
						}
						else
						{
							if (_workingUnitCount < _step)
							{
								if (_nextUnitIndex < _taskUnitList.Count)
								{
									BatchTaskUnit unit = _taskUnitList[_nextUnitIndex];
									if (unit.UnitState == BatchTaskUnit.State.Querying)
									{
										GameServer server = AdminServer.TheInstance.GameServerManager.GetGameServer(unit.ServerId);
										if (server != null)
										{
											unit.UnitState = BatchTaskUnit.State.Doing;
											_workingUnitCount++;

											AutomationContext context = new AutomationContext();
											context.ServerList.Add(server);
											AdminServer.TheInstance.AutomationManager.Do(Automation, context, new DelegateAutomationDone(AutomationDone), unit);
										}
										else
										{
											unit.UnitState = BatchTaskUnit.State.Done;
										}
									}

									_nextUnitIndex++;
								}
								else if (_workingUnitCount == 0)
								{
									_state = State.Done;
								}
							}
						}
					}
					else if (_state == State.Paused)
					{
						if (_needContinue)
						{
							_state = State.Doing;
							_needContinue = false;
						}
						else
						{
							Thread.Sleep(1000);
						}
					}
				}

				Thread.Sleep(1);
			}

			if (_needAbort)
			{
				_state = State.Aborted;
				_needAbort = false;
			}

			_endTime = DateTime.Now;
		}

		void AutomationDone(object stateObject)
		{
			lock (_lock)
			{
				BatchTaskUnit unit = stateObject as BatchTaskUnit;
                if (unit != null)
                {
                    unit.UnitState = BatchTaskUnit.State.Done;
                    _workingUnitCount--;
                    ++_completeServerCount;
                }
			}
		}

        private void DoTaskComplete(IAsyncResult ar)
        {
            if (_batchTaskComplete != null)
            {
                if (TaskState == State.Aborted)
                {
                    _success = false;
                }
                else
                {
                    _success = _completeServerCount >= _taskUnitList.Count;
                }
                _batchTaskComplete(_batchTaskCompleteCallBackObject);
                _batchTaskComplete = null;
            }
        }
	}

	/// <summary>
	/// 批量任务管理器
	/// </summary>
	public class BatchTaskManager : IDisposable
	{
        const string SecurityObjectBatchTaskManager = "/FSEye/System/BatchTaskManager";

		IList<BatchTask> _batchTaskList = new List<BatchTask>();

		FSEyeObject _securityObject;

		bool _on;

        public bool On
        {
            get { return _on; }
            set { _on = value; }
        }

        public FSEyeObject SecurityObject
        {
            get { return _securityObject; }
            set { _securityObject = value; }
        }

		/// <summary>
		/// 任务列表
		/// </summary>
		public IList<BatchTask> TaskList
		{
			get { return _batchTaskList; }
		}

        public BatchTask GetBatchTask(int taskId)
        {
            foreach (BatchTask task in _batchTaskList)
            {
                if (task.ID == taskId) return task;
            }
            return null;
        }

		/// <summary>
		/// 添加任务
		/// </summary>
        public bool AddTask(BatchTask task, string name, string comment)
		{
			if (!_batchTaskList.Contains(task))
			{
                using (IBlazeDatabase db = DbFactory.GetDatabase())
                {
                    try
                    {
                        db.BeginTrans();

                        SecurityManager sm = AdminServer.TheInstance.SecurityManager;
                        FSEyeObject newObject = sm.Get(SecurityObject.FullPath + SecurityManager.ObjectPathDelimiter + name, db);
                        byte[] taskAutomationBytes = AdminServer.TheInstance.AutomationManager.Save(task.Automation);

                        if (newObject != null && taskAutomationBytes != null)
                        {
                            IBlazeTable table = db.GetTable(TableString.BatchTaskTableName);
                            DataSet taskDataSet = new DataSet();
                            table.Get(taskDataSet);
                            DataTable taskTable = taskDataSet.Tables[TableString.BatchTaskTableName];

                            DataRow newRow = taskTable.NewRow();
                            newRow[TableString.BatchTaskId] = newObject.Id;
                            newRow[TableString.BatchTaskAutomation] = taskAutomationBytes;
                            newRow[TableString.BatchTaskStep] = task.Step;

                            StringBuilder serverIdText = new StringBuilder();
                            foreach (int serverId in task.Servers)
                            {
                                serverIdText.Append(serverId);
                                serverIdText.Append(',');
                            }

                            newRow[TableString.BatchTaskServerIds] = SystemConfig.Current.DefaultEncoding.GetBytes(serverIdText.Length == 0 ? string.Empty : serverIdText.ToString(0, serverIdText.Length - 1));
                            taskTable.Rows.Add(newRow);
                            table.Set(taskTable);

                            //设置权限并更新SecurityObject
                            AdminServer.TheInstance.SecurityManager.CopyAce(newObject.Parent, newObject, true);
                            newObject.Comment = comment;
                            sm.Set(newObject);
                            task.SecurityObject = newObject;                            

                            _batchTaskList.Add(task);

                            db.CommitTrans();
                            return true;
                        }
                    }
                    catch (Exception)
                    {
                        if (db != null)
                            db.RollbackTrans();
                        return false;
                    }
                }
			}
            return false;
		}

        /// <summary>
        /// 修改任务
        /// </summary>        
        public bool EditTask(int taskId, int step, IAutomation automation, GameServer[] servers, string name, string comment)
        {
            using (IBlazeDatabase db = DbFactory.GetDatabase())
            {
                try
                {
                    BatchTask task = this.GetBatchTask(taskId);
                    if (task == null)
                    {
                        return false;
                    }

                    db.BeginTrans();

                    SecurityManager sm = AdminServer.TheInstance.SecurityManager;
                    FSEyeObject taskObject = sm.Get(taskId);
                    taskObject.Name = name;
                    taskObject.Comment = comment;
                    sm.Set(taskObject, db);

                    StringBuilder serverIdText = new StringBuilder();
                    foreach (GameServer server in servers)
                    {
                        serverIdText.Append(server.Id);
                        serverIdText.Append(',');
                    }

                    IBlazeTable batchTaskTable = db.GetTable(TableString.BatchTaskTableName);
                    DataSet batchTaskDataSet = new DataSet();
                    batchTaskTable.Get(batchTaskDataSet);
                    DataTable taskTable = batchTaskDataSet.Tables[TableString.BatchTaskTableName];
                    DataRow[] taskRows = taskTable.Select(string.Concat(TableString.BatchTaskId, "=", taskId));
                    if (taskRows != null && taskRows.Length > 0)
                    {
                        taskRows[0][TableString.BatchTaskStep] = step;
                        taskRows[0][TableString.BatchTaskAutomation] = AdminServer.TheInstance.AutomationManager.Save(automation);
                        taskRows[0][TableString.BatchTaskServerIds] = SystemConfig.Current.DefaultEncoding.GetBytes(serverIdText.Length == 0 ? string.Empty : serverIdText.ToString(0, serverIdText.Length - 1));
                        batchTaskTable.Set(taskTable);
                    }

                    //修改内存中数据
                    task.Step = step;
                    task.Automation = automation;
                    task.ClearServers();
                    foreach (GameServer server in servers)
                    {
                        task.AddServer(server.Id);
                    }

                    db.CommitTrans();
                    return true;
                }
                catch (Exception)
                {
                    if (db != null)
                        db.RollbackTrans();
                    return false;
                }
            }
        }

		/// <summary>
		/// 删除任务
		/// </summary>
		public bool DeleteTask(int taskId)
        {
            using (IBlazeDatabase db = DbFactory.GetDatabase())
            {
                try
                {
                    BatchTask task = this.GetBatchTask(taskId);
                    if (task == null)
                    {
                        return false;
                    }

                    db.BeginTrans();

                    AdminServer.TheInstance.SecurityManager.Delete(taskId, db);

                    IBlazeTable batchTaskTable = db.GetTable(TableString.BatchTaskTableName);
                    batchTaskTable.Delete(FilterFactory.CreateEqualFilter(TableString.BatchTaskId, taskId));

                    _batchTaskList.Remove(task);

                    db.CommitTrans();
                    return true;
                }
                catch (Exception)
                {
                    if (db != null)
                        db.RollbackTrans();
                    return false;
                }
            }
		}

        public bool Load()
        {
            this.SecurityObject = AdminServer.TheInstance.SecurityManager.Get(SecurityObjectBatchTaskManager);
            this._batchTaskList.Clear();

            using (IBlazeDatabase db = DbFactory.GetDatabase())
            {
                IBlazeTable table = db.GetTable(TableString.BatchTaskTableName);
                DataSet batchTaskDataSet = new DataSet();
                table.Get(batchTaskDataSet);
                DataTable batchTaskTable = batchTaskDataSet.Tables[TableString.BatchTaskTableName];
                StrategyManager sm = AdminServer.TheInstance.StrategyManager;
                AutomationManager am = AdminServer.TheInstance.AutomationManager;
                foreach (DataRow row in batchTaskTable.Rows)
                {
                    int batchTaskId = (int)row[TableString.BatchTaskId];

                    try
                    {
                        BatchTask batchTask = new BatchTask();
                        batchTask.Step = (int)row[TableString.BatchTaskStep];
                        batchTask.Automation = am.Load((byte[])row[TableString.BatchTaskAutomation]);
                        string serverIds = SystemConfig.Current.DefaultEncoding.GetString((byte[])row[TableString.BatchTaskServerIds]);
                        foreach (string serverIdText in serverIds.Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries))
                        {
                            int serverId = int.Parse(serverIdText);
                            GameServer server = AdminServer.TheInstance.GameServerManager.GetGameServer(serverId);
                            if (server != null)
                                batchTask.AddServer(serverId);
                        }

                        FSEyeObject secObj = AdminServer.TheInstance.SecurityManager.Get(batchTaskId);
                        if (secObj != null)
                        {
                            batchTask.SecurityObject = secObj;
                            _batchTaskList.Add(batchTask);
                        }
                    }
                    catch (Exception e)
                    {
                        Util.DebugLog(e.StackTrace);
                        continue;
                    }
                }
                return true;
            }
        }

		#region IDisposable Members

		public void Dispose()
		{
			_batchTaskList.Clear();
		}

		#endregion
	}
}