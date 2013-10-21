//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 2007
//      File_base        : Util
//      File_ext         : cs
//      Author           : 徐晓刚
//      Description      : FSEye工具类
//
//////////////////////////////////////////////////////////////////////

using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Runtime.InteropServices;

namespace FSEye
{
	/// <summary>
	/// 工具类
	/// </summary>
	/// <author>徐晓刚</author>
	public class Util
	{
		/// <summary>
		/// 调试用日志
		/// </summary>
		/// <param name="msg">日志消息</param>
		public static void DebugLog(string msg)
		{
			if (SystemConfig.Current.LogEnabled)
			{
				string logDir = SystemConfig.Current.LogDir;
				if (logDir != null)
				{
					using (StreamWriter writer = new StreamWriter(logDir + "/log.txt", true))
					{
						writer.WriteLine(DateTime.Now.ToString() + ": " + msg);
						writer.Flush();
                        //writer.Dispose();
					}
				}
			}
		}

		/// <summary>
		/// 把byte数组转化为16进制字符串表现形式
		/// </summary>
		public static string ConvertBytesToHexString(byte[] data)
		{
			if (data == null || data.Length == 0)
				return string.Empty;

			StringBuilder sb = new StringBuilder();
			sb.Append("0x");
			for (int i = 0; i < data.Length; i++)
			{
				sb.AppendFormat("{0:X2}", data[i]);
			}

			return sb.ToString();
		}

		/// <summary>
		/// 去掉字符串结尾
		/// </summary>
		/// <remarks>去掉字符串后面的0</remarks>
		public static void TrimStringEnd(ref byte[] buff)
		{
			if (buff == null || buff.Length == 0)
				return;

			int endIndex = 0;
			for (int i = 0; i < buff.Length; i++)
			{
				if (buff[i] == 0)
				{
					endIndex = i;
					break;
				}
			}
			Array.Resize<byte>(ref buff, endIndex);
		}

		/// <summary>
		/// 把“键-值”对字符串转化为哈希表
		/// </summary>
		/// <remarks>字符串格式为“key1=value1\nkey2=value2\nkey3=value3”</remarks>
		public static Hashtable ConvertKeyValuePairToHashtable(string keyValuePairString)
		{
			if (keyValuePairString == null)
				throw new ArgumentNullException("keyValuePairString");

			Hashtable hashTable = new Hashtable();

			string[] pairs = keyValuePairString.Split(new char[] { '\n' }, StringSplitOptions.RemoveEmptyEntries);
			if (pairs != null || pairs.Length > 0)
			{
				foreach (string pair in pairs)
				{
					int splitPos = pair.IndexOf('=');
					string key = pair.Substring(0, splitPos);
					string value = pair.Remove(0, splitPos + 1);
					hashTable.Add(key, value);
				}
			}

			return hashTable;
		}

		/// <summary>
		/// 根据X计算Y
		/// </summary>
		/// <remarks>认证密码算法</remarks>
		public static int CaculateY(int x)
		{
			return 101 * x * x + 43 * x + 72;
		}
	}
}
