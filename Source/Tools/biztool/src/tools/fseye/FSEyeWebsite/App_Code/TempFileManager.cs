//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-10
//      File_base        : TempFileManager
//      File_ext         : cs
//      Author           : 徐晓刚
//      Description      : 临时文件管理器
//
//////////////////////////////////////////////////////////////////////

using System;
using System.IO;
using System.Collections;

/// <summary>
/// 临时文件管理器类
/// </summary>
public class TempFileManager
{
	/// <summary>
	/// 临时文件类
	/// </summary>
	private class TempFile
	{
		private string _path;

		private long _timeToLive;

		public TempFile(string filePath, long timeToLive)
		{
			_path = filePath;
			_timeToLive = timeToLive;
		}

		public void TimeEsacpe(long time)
		{
			_timeToLive -= time;
		}

		public string Path
		{
			get { return _path; }
		}

		public long TimeToLive
		{
			get { return _timeToLive; }
		}
	}

	private static TempFileManager g_theInstance = new TempFileManager();

	private ArrayList _fileList;

	private long _checkInterval;

	private System.Threading.Timer _checkTempFileTimer;

	public static TempFileManager TheInstance
	{
		get { return g_theInstance; }
	}

	/// <summary>
	/// 检查间隔
	/// </summary>
	/// <remarks>默认值是：5000毫秒</remarks>
	public long CheckInterval
	{
		get { return _checkInterval; }
		set
		{
			_checkInterval = value;
			//改变检查间隔就需要改变定时器的间隔
			_checkTempFileTimer.Change(0, _checkInterval);
		}
	}

	/// <summary>
	/// 使用单体模式
	/// </summary>
	private TempFileManager()
	{
		_checkInterval = 5000;
		_fileList = ArrayList.Synchronized(new ArrayList());
		_checkTempFileTimer = new System.Threading.Timer(new System.Threading.TimerCallback(CheckFiles), null, 0, _checkInterval);
	}

	/// <summary>
	/// 清除所有临时文件
	/// </summary>
	public void ClearAllTempFiles()
	{
		lock (_fileList.SyncRoot)
		{
			foreach (TempFile file in _fileList)
			{
				FileInfo info = new FileInfo(file.Path);
				if (info.Exists)
				{
					info.Delete();
				}
			}

			_fileList.Clear();
		}
	}

	~TempFileManager()
	{
		ClearAllTempFiles();
	}

	/// <summary>
	/// 添加临时文件
	/// </summary>
	/// <param name="filePath">文件路径</param>
	/// <param name="timeToLive">文件生存时间，单位：毫秒</param>
	public void AddTempFile(string filePath, long timeToLive)
	{
		_fileList.Add(new TempFile(filePath, timeToLive));
	}

	/// <summary>
	/// 检查一次临时文件
	/// </summary>
	private void CheckFiles(object state)
	{
		lock (_fileList.SyncRoot)
		{
			if (_fileList.Count > 0)
			{
				ArrayList toBeRemoved = new ArrayList();//需要删除的临时文件列表

				int count = _fileList.Count;
				TempFile file = null;
				for (int i = 0; i < count; i++)
				{
					file = _fileList[i] as TempFile;

					//将临时文件的生存时间减少一次
					file.TimeEsacpe(CheckInterval);

					//如果临时文件到了生命终点
					if (file.TimeToLive <= 0)
					{
						toBeRemoved.Add(file);

						//从磁盘中删除这个临时文件
						FileInfo info = new FileInfo(file.Path);
						if (info.Exists)
						{
							info.Delete();
						}
					}
				}

				//从临时文件列表中删除这个临时文件
				foreach (TempFile tempfile in toBeRemoved)
				{
					_fileList.Remove(tempfile);
				}
				toBeRemoved.Clear();
			}
		}
	}
}
