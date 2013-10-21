//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 2007-4-12
//      File_base        : AutomationManager
//      File_ext         : cs
//      Author           : 徐晓刚
//      Description      : 自动化管理器
//
//////////////////////////////////////////////////////////////////////

using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Threading;
using FSEye.Security;

namespace FSEye
{
	/// <summary>
	/// 自动化接口
	/// </summary>
	public interface IAutomation
	{
		/// <summary>
		/// GUID
		/// </summary>
		string Guid { get; }

		/// <summary>
		/// 名字
		/// </summary>
		string Name { get; }

        /// <summary>
        /// 描述
        /// </summary>
        string Description { get;}

        /// <summary>
        /// 参数描述
        /// </summary>
        string[] ParameterDes { get;}

		/// <summary>
		/// 状态
		/// </summary>
		AutomationState State { get; }

		/// <summary>
		/// 结果
		/// </summary>
		AutomationResult Result { get; }

		/// <summary>
		/// 参数
		/// </summary>
		object[] Parameter { get; set; }

		/// <summary>
		/// 执行
		/// </summary>
		void Do(AutomationContext context);

		/// <summary>
		/// 完毕后续
		/// </summary>
		IAutomation Complete { get; set; }

		/// <summary>
		/// 成功后续
		/// </summary>
		IAutomation Success { get; set; }

		/// <summary>
		/// 失败后续
		/// </summary>
		IAutomation Failure { get; set; }
	}

	/// <summary>
	/// 自动化状态
	/// </summary>
	public enum AutomationState
	{
		Init,//初始
		Doing,//正在执行
		Done//已经完成
	}

	/// <summary>
	/// 自动化结果
	/// </summary>
	public enum AutomationResult
	{
		Success,//成功
		Failure//失败
	}

	/// <summary>
	/// 自动化上下文
	/// </summary>
	public class AutomationContext : ICloneable
	{
		/// <summary>
		/// 服务器
		/// </summary>
		public GameServer Server;

		/// <summary>
		/// 服务器列表
		/// </summary>
		public IList<GameServer> ServerList = new List<GameServer>();

		/// <summary>
		/// 服务器组
		/// </summary>
		public ServerGroup Group;

		/// <summary>
		/// 服务器组列表
		/// </summary>
		public IList<ServerGroup> ServerGroupList = new List<ServerGroup>();

		/// <summary>
		/// 消息
		/// </summary>
		public string Message;

		/// <summary>
		/// 额外的对象
		/// </summary>
		public ICloneable AdditionalObject;

		public AutomationContext()
		{
		}

		public object Clone()
		{
			AutomationContext cloneObj = new AutomationContext();

			cloneObj.Server = Server;
			if (ServerList != null)
			{
				cloneObj.ServerList = new List<GameServer>(ServerList);
			}

			cloneObj.Group = Group;
			if (ServerGroupList != null)
			{
				cloneObj.ServerGroupList = new List<ServerGroup>(ServerGroupList);
			}

			cloneObj.Message = Message;
			if (AdditionalObject != null)
			{
				cloneObj.AdditionalObject = (ICloneable)AdditionalObject.Clone();
			}

			return cloneObj;
		}

		/// <summary>
		/// 得到上下文中所有的服务器
		/// </summary>
		public IList<GameServer> GetAllServers()
		{
			IList<GameServer> allServers = new List<GameServer>();

			if (Server != null)
			{
				allServers.Add(Server);
			}

			foreach (GameServer server in ServerList)
			{
				allServers.Add(server);
			}

			if (Group != null)
			{
				GetServerInGroup(Group, allServers);
			}
						
			foreach (ServerGroup group in ServerGroupList)
			{
				GetServerInGroup(group, allServers);
			}

			return allServers;
		}

		/// <summary>
		/// 清空上下文中所有的服务器
		/// </summary>
		public void ClearAllServers()
		{
			if (Server != null)
			{
				Server = null;
			}

			if (ServerList != null)
			{
				ServerList.Clear();
			}

			if (Group != null)
			{
				Group = null;
			}

			if (ServerGroupList != null)
			{
				ServerGroupList.Clear();
			}
		}

		private void GetServerInGroup(ServerGroup group, IList<GameServer> serverList)
		{
			if (group.GroupType == ServerGroup.Type.Group)
			{
				foreach (ServerGroup servergroup in group.List)
				{
					GetServerInGroup(servergroup, serverList);
				}
			}
			else if (group.GroupType == ServerGroup.Type.Server)
			{
				foreach (GameServer server in group.List)
				{
					serverList.Add(server);
				}
			}
		}

		/// <summary>
		/// 删除服务器
		/// </summary>
		/// <remarks>只能删除存放在Server和ServerList中的，但是无法删除GroupList中的</remarks>
		public void RemoveGameServer(GameServer server)
		{
			if (Server == server)
				Server = null;

			ServerList.Remove(server);
		}
	}

	/// <summary>
	/// 自动化抽象基类
	/// </summary>
	public abstract class Automation : IAutomation
	{
		protected AutomationState _state;

		protected AutomationResult _result;

		IAutomation _complete;

		IAutomation _success;

		IAutomation _failure;

		public AutomationState State
		{
			get { return _state; }
		}

		public AutomationResult Result
		{
			get { return _result; }
		}

		public IAutomation Complete
		{
			get { return _complete; }
			set { _complete = value; }
		}

		public IAutomation Success
		{
			get { return _success; }
			set { _success = value; }
		}

		public IAutomation Failure
		{
			get { return _failure; }
			set { _failure = value; }
		}

		public Automation()
		{
			_state = AutomationState.Init;
		}

		public override string ToString()
		{
			StringBuilder parameterString = new StringBuilder();
            for (int i = 0; i != (int)Parameter[0]; ++i)//param in Parameter[0])
            {
                if (!Parameter[i + 1].ToString().Equals(string.Empty))
                {
                    parameterString.Append(Parameter[i + 1]);
                    parameterString.Append(',');
                }
            }
            if (parameterString.Length != 0)
                return Name + " (" + parameterString.ToString(0, parameterString.Length - 1) + ")";
            else return Name;
		}

		public abstract string Guid { get; }

		public abstract string Name { get; }

        public abstract string Description { get;}

        public abstract string[] ParameterDes { get;}

        //参数,string数组,其中第一个表明参数的个数.
		public abstract object[] Parameter { get; set; }

		public abstract void Do(AutomationContext context);
	}

	public delegate void DelegateAutomationDone(object stateObject);

	/// <summary>
	/// 自动化管理器
	/// </summary>
	public class AutomationManager : IDisposable, ISecurity
	{
		FSEyeObject _securityObject;

		public FSEyeObject SecurityObject
		{
			get { return _securityObject; }
			set { _securityObject = value; }
		}

		/// <summary>
		/// 自动化执行单元
		/// </summary>
		public class AutomationUnit
		{
			IAutomation _automation;

			AutomationContext _context;

			DelegateAutomationDone _callback;

			object _callbackStateObject;

			AutomationUnit _callingUnit;

			/// <summary>
			/// 自动化作业
			/// </summary>
			public IAutomation Automation
			{
				get { return _automation; }
				set { _automation = value; }
			}

			/// <summary>
			/// 上下文
			/// </summary>
			public AutomationContext Context
			{
				get { return _context; }
				set { _context = value; }
			}

			/// <summary>
			/// 完成回调
			/// </summary>
			public DelegateAutomationDone Callback
			{
				get { return _callback; }
				set { _callback = value; }
			}

			/// <summary>
			/// 回调状态对象
			/// </summary>
			public object CallbackStateObject
			{
				get { return _callbackStateObject; }
				set { _callbackStateObject = value; }
			}

			/// <summary>
			/// 调用该单元的单元
			/// </summary>
			public AutomationUnit CallingUnit
			{
				get { return _callingUnit; }
				set { _callingUnit = value; }
			}

			public AutomationUnit()
			{
			}

			public AutomationUnit(IAutomation automation, AutomationContext context, DelegateAutomationDone callback, object stateObject, AutomationUnit callingUnit)
			{
				_automation = automation;
				_context = context;
				_callback = callback;
				_callbackStateObject = stateObject;
				_callingUnit = callingUnit;
			}
		}

		/// <summary>
		/// 正在工作的列表
		/// </summary>
		IList _workingList;

		/// <summary>
		/// 需要添加的列表
		/// </summary>
		IList _toBeAddList;

		/// <summary>
		/// 需要刪除的列表
		/// </summary>
		IList _toBeDelList;

		/// <summary>
		/// 需要移动到等待列表中的自动化单元列表
		/// </summary>
		IList _toBeWaitList;

		/// <summary>
		/// 等待后续完成的列表
		/// </summary>
		IList _waitForCompleteList;

		/// <summary>
		/// 正在工作
		/// </summary>
		bool _working;

		/// <summary>
		/// 销毁
		/// </summary>
		bool _dispose;

		Thread _workingThread;

		/// <summary>
		/// 自动化作业表
		/// </summary>
		Hashtable _automationTable;

		public AutomationManager()
		{
			_automationTable = new Hashtable();
			_workingList = ArrayList.Synchronized(new ArrayList());
			_toBeAddList = ArrayList.Synchronized(new ArrayList());
			_toBeDelList = ArrayList.Synchronized(new ArrayList());
			_toBeWaitList = ArrayList.Synchronized(new ArrayList());
			_waitForCompleteList = ArrayList.Synchronized(new ArrayList());

			_dispose = false;
			_working = false;

			_workingThread = new Thread(new ThreadStart(Process));
			_workingThread.Start();
		}

		/// <summary>
		/// 正在工作
		/// </summary>
		public bool Working
		{
			get { return _working; }
			set { _working = value; }
		}

		/// <summary>
		/// 执行自动化作业
		/// </summary>
		/// <remarks>简化版，内部自动生成上下文对象</remarks>
		public void Do(IAutomation automation)
		{
			Do(automation, new AutomationContext(), null, null, null);
		}

		/// <summary>
		/// 执行自动化作业
		/// </summary>
		/// <remarks>简化版，免回调</remarks>
		public void Do(IAutomation automation, AutomationContext context)
		{
			Do(automation, context, null, null, null);
		}

		/// <summary>
		/// 执行自动化作业
		/// </summary>
		/// <remarks>简化版，免回调</remarks>
		public void Do(IAutomation automation, AutomationContext context, AutomationUnit callingUnit)
		{
			Do(automation, context, null, null, callingUnit);
		}

		/// <summary>
		/// 执行自动化作业
		/// </summary>
		/// <remarks>简化版，免回调</remarks>
		public void Do(IAutomation automation, AutomationContext context, DelegateAutomationDone callback, object stateObject)
		{
			Do(automation, context, callback, stateObject, null);
		}

		/// <summary>
		/// 执行自动化作业
		/// </summary>
		/// <remarks>作业上下文会在这个函数内部被复制，从而保证作业各自的上下文不会互相干扰，而只能对各自的后续作业产生影响</remarks>
		public void Do(IAutomation automation, AutomationContext context, DelegateAutomationDone callback, object stateObject, AutomationUnit callingUnit)
		{
			if (automation == null)
			{
				throw new ArgumentNullException("automation");
			}

			if (context == null)
			{
				throw new ArgumentNullException("context");
			}

			_toBeAddList.Add(new AutomationUnit(automation, (AutomationContext)context.Clone(), callback, stateObject, callingUnit));
		}

		/// <summary>
		/// 列出注册的自动化类型
		/// </summary>
		public Type[] Automations
		{
			get
			{
				Type[] automationType = new Type[_automationTable.Count];
				_automationTable.Values.CopyTo(automationType, 0);
				return automationType;
			}
		}

		/// <summary>
		/// 注册自动化类型
		/// </summary>
		public void Register(IAutomation automation)
		{
			if (!_automationTable.Contains(automation.Guid))
			{
				_automationTable.Add(automation.Guid, automation.GetType());
			}
		}

		/// <summary>
		/// 创建自动化实例
		/// </summary>
		/// <param name="automationGuid"></param>
		/// <returns></returns>
		public IAutomation Create(string automationGuid)
		{
			Type type = _automationTable[automationGuid] as Type;
			return (IAutomation)Activator.CreateInstance(type);
		}

		/// <summary>
		/// 载入自动化作业对象
		/// </summary>
		public IAutomation Load(byte[] data)
		{
			if (data == null)
				return null;

			BinaryReader reader = null;
			try
			{
				reader = new BinaryReader(new MemoryStream(data));

				int length = reader.ReadInt32();
				string guid = Encoding.Default.GetString(reader.ReadBytes(length));
				IAutomation automation = Create(guid);

				length = reader.ReadInt32();
				if (length > 0)
				{
                    //修改By姚2007-8-22
                    string[] param = SystemConfig.Current.DefaultEncoding.GetString(reader.ReadBytes(length)).Split(',');
                    object[] paramOjb = new object[int.Parse(param[0])+1];
                    for(int index=0;index!=param.Length;++index)
                        paramOjb[index] = param[index].Clone();
                    try
                    {
                        automation.Parameter = paramOjb;
                    }
                    catch (Exception)
                    { 
                        //Go on 
                        //忽略不对的参数，大部分是由于服务器或者服务器组被删除后读取该参数所致。
                    }

					//automation.Parameter = Encoding.Default.GetString(reader.ReadBytes(length));
				}

				length = reader.ReadInt32();
				if (length > 0)
				{
					automation.Complete = Load(reader.ReadBytes(length));
				}

				length = reader.ReadInt32();
				if (length > 0)
				{
					automation.Success = Load(reader.ReadBytes(length));
				}

				length = reader.ReadInt32();
				if (length > 0)
				{
					automation.Failure = Load(reader.ReadBytes(length));
				}

				return automation;
			}
			catch (Exception)
			{
			}
			finally
			{
				if (reader != null)
					reader.Close();
			}

			return null;
		}

		/// <summary>
		/// 保存自动化作业对象
		/// </summary>
		public byte[] Save(IAutomation automation)
		{
			if (automation == null)
				return null;

			BinaryWriter writer = null;
			try
			{
				writer = new BinaryWriter(new MemoryStream());

				byte[] guidBytes = Encoding.Default.GetBytes(automation.Guid);
				writer.Write(guidBytes.Length);
				writer.Write(guidBytes, 0, guidBytes.Length);

                //################################
                StringBuilder paramBuilder = new StringBuilder();
                foreach (object obj in automation.Parameter)
                {
                    paramBuilder.Append(obj);
                    //if (obj.GetType() == typeof(int))
                    //{
                    //    paramBuilder.Append((int)obj);
                    //}
                    //else if (obj.GetType() == typeof(string))
                    //{
                    //    paramBuilder.Append(obj as string);
                    //}
                    //else if(obj.GetType() == typeof(long))
                    //{
                    //    paramBuilder.Append((long)obj);
                    //}
                    paramBuilder.Append(',');
                }
                string param = paramBuilder.ToString(0,paramBuilder.Length-1);
                //################################
                //string param = automation.Parameter;
				if (param != null)
				{
                    byte[] paramBytes = SystemConfig.Current.DefaultEncoding.GetBytes(param);
					writer.Write(paramBytes.Length);
					writer.Write(paramBytes, 0, paramBytes.Length);
				}
				else
				{
					writer.Write(0);
				}

				if (automation.Complete != null)
				{
					byte[] completeBytes = Save(automation.Complete);
					writer.Write(completeBytes.Length);
					writer.Write(completeBytes, 0, completeBytes.Length);
				}
				else
				{
					writer.Write(0);
				}

				if (automation.Success != null)
				{
					byte[] successBytes = Save(automation.Success);
					writer.Write(successBytes.Length);
					writer.Write(successBytes, 0, successBytes.Length);
				}
				else
				{
					writer.Write(0);
				}

				if (automation.Failure != null)
				{
					byte[] failureBytes = Save(automation.Failure);
					writer.Write(failureBytes.Length);
					writer.Write(failureBytes, 0, failureBytes.Length);
				}
				else
				{
					writer.Write(0);
				}

				byte[] returnBytes = (writer.BaseStream as MemoryStream).ToArray();

				return returnBytes;
			}
			catch (Exception)
			{
			}
			finally
			{
				if (writer != null)
					writer.Close();
			}

			return null;
		}

		/// <summary>
		/// 删除自动化作业
		/// </summary>
		private void Remove(AutomationUnit automationUnit)
		{
			_toBeDelList.Add(automationUnit);
		}

		/// <summary>
		/// 等待后续完成
		/// </summary>
		/// <param name="automationUnit"></param>
		private void WaitForComplete(AutomationUnit automationUnit)
		{
			_toBeWaitList.Add(automationUnit);
		}

		/// <summary>
		/// 处理
		/// </summary>
		private void Process()
		{
			while (!_dispose)
			{
				if (_working)
				{
					//执行每个自动化作业单元
					foreach (AutomationUnit automationUnit in _workingList)
					{
						IAutomation automation = automationUnit.Automation;
						AutomationContext context = automationUnit.Context;
						switch (automation.State)
						{
							case AutomationState.Init:
								//开始执行
								automation.Do(context);
								break;
							case AutomationState.Doing:
								//正在执行中
								automation.Do(context);
								break;
							case AutomationState.Done:	
								//执行完毕
								{
									bool completed = true;
									if (automation.Result == AutomationResult.Success && automation.Success != null)
									{
										completed = false;
										Do(automation.Success, context, automationUnit);
									}

									if (automation.Result == AutomationResult.Failure && automation.Failure != null)
									{
										completed = false;
										Do(automation.Failure, context, automationUnit);
									}

									if (automation.Complete != null)
									{
										completed = false;
										Do(automation.Complete, context, automationUnit);
									}

									if (completed)
									{
										Remove(automationUnit);

										if (automationUnit.Callback != null)
										{
                                            automationUnit.Callback.Invoke(automationUnit.CallbackStateObject);
										}

										if (automationUnit.CallingUnit != null)
										{
											NotifyWaitUnit(automation, automationUnit.CallingUnit);
										}
									}
									else
									{
										WaitForComplete(automationUnit);										
									}
								}
								break;
							default:
								throw new Exception("Invalid automation state!");
						}
					}

					//删除需要删除的自动化作业单元
					if (_toBeDelList.Count > 0)
					{
						foreach (AutomationUnit automationUnit in _toBeDelList)
						{
							_workingList.Remove(automationUnit);
						}

						_toBeDelList.Clear();
					}

					//删除等待完成的自动化作业单元
					if (_toBeWaitList.Count > 0)
					{
						foreach (AutomationUnit automationUnit in _toBeWaitList)
						{
							_workingList.Remove(automationUnit);
							_waitForCompleteList.Add(automationUnit);
						}

						_toBeWaitList.Clear();
					}

					//添加需要添加的自动化作业单元
					if (_toBeAddList.Count > 0)
					{
						foreach (AutomationUnit automationUnit in _toBeAddList)
						{
							_workingList.Add(automationUnit);
						}

						_toBeAddList.Clear();
					}
				}

				Thread.Sleep(1);
			}
		}

		void NotifyWaitUnit(IAutomation automation, AutomationUnit callingUnit)
		{
			if (callingUnit != null)
			{
				int index = _waitForCompleteList.IndexOf(callingUnit);
                if (index >= 0 && index < _waitForCompleteList.Count)
				{
					AutomationUnit waitUnit = _waitForCompleteList[index] as AutomationUnit;
					IAutomation waitAutomation = waitUnit.Automation;
					if (waitAutomation.Success == automation)
					{
						waitAutomation.Success = null;
					}
					else if (waitAutomation.Failure == automation)
					{
						waitAutomation.Failure = null;
					}
					else if (waitAutomation.Complete == automation)
					{
						waitAutomation.Complete = null;
					}

                    switch (waitAutomation.Result)
                    {
                        case AutomationResult.Success:
                            if (waitAutomation.Success == null && waitAutomation.Complete == null)
                            {
                                if (waitUnit.Callback != null)
                                {
                                    waitUnit.Callback.Invoke(waitUnit.CallbackStateObject);
                                }

                                if (waitUnit.CallingUnit != null)
                                {
                                    NotifyWaitUnit(waitAutomation, waitUnit.CallingUnit);
                                }

                                _waitForCompleteList.Remove(waitUnit);
                            }
                            break;
                        case AutomationResult.Failure:
                            if (waitAutomation.Failure == null && waitAutomation.Complete == null)
                            {
                                if (waitUnit.Callback != null)
                                {
                                    waitUnit.Callback.Invoke(waitUnit.CallbackStateObject);
                                }

                                if (waitUnit.CallingUnit != null)
                                {
                                    NotifyWaitUnit(waitAutomation, waitUnit.CallingUnit);
                                }

                                _waitForCompleteList.Remove(waitUnit);
                            }
                            break;
                    }
					
				}
			}
		}

		public void Dispose()
		{
			_dispose = true;
		}
	}

	public class BadParameterException : Exception
	{
		public BadParameterException()
		{
		}

		public BadParameterException(string msg, Exception innerException)
			: base(msg, innerException)
		{
		}
	}
}
