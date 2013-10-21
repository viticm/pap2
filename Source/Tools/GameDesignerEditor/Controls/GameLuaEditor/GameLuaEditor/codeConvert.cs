using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;
using System.Data;
using luaEditor;

namespace DataBaseIO
{
    public static class CodeConvert
    {
        /// <summary>
        /// 解码：把不带中文的给变成带中文的
        /// </summary>
        /// <param name="strCode">源代码</param>
        /// <param name="ls_ht">ls表 Key为数字字符串，Value为中文字符串</param>
        /// <param name="tableName">字符串表名</param>
        /// <returns>带中文的代码</returns>
        public static string Decode(string strCode, Hashtable ls_ht, string tableName)
        {            
            string[] as_str = strCode.Split(new string[] { string.Format("{0}[", tableName) }, StringSplitOptions.None);
            
            for (int i = 1; i < as_str.Length; i++)
            {
                if ((as_str[i].IndexOf("]")) == -1)
                {
                    continue;
                }

                string strKey = as_str[i].Substring(0, as_str[i].IndexOf("]"));

                if (!ls_ht.ContainsKey(strKey))
                {
                    continue;
                }

                strCode = strCode.Replace(string.Format("{0}[{1}]", tableName, strKey.Trim()), ls_ht[strKey].ToString());
            }

            return strCode;
        }

        /// <summary>
        /// 获取包含中文字符串信息的hashtable
        /// </summary>
        /// <param name="strCode">原字符串</param>
        /// <returns>包含中文字符串信息的hashtable</returns>
        private static Hashtable GetSubstringHashtable(string strCode)
        {
            GameLuaEditor.DFATable dfa = new GameLuaEditor.DFATable();

            return dfa.getStringList(strCode);
        }

        /// <summary>
        /// 将ls数据行转换为ls文件中的字符串
        /// </summary>
        /// <param name="lsRows">ls数据</param>
        /// <returns>ls文本内容</returns>
        public static string ConvertLsFileContent(DataRow[] lsRows, string tableName)
        {
            string data = string.Format("Include('scripts/Include/String.lh')\r\n{0} = {{\r\n", tableName);
            string content;
            string id;

            foreach (DataRow r in lsRows)
            {
                id = r["id"].ToString();
                content = r["content"].ToString();
                data = data + string.Format("    [{0}] = {1},\r\n", id, content);
            }

            data = data.Substring(0, data.Length - 3) + "\r\n";
            data = data + "}";

            return data;
        }

        /// <summary>
        /// 查找代码中所有中文字符串
        /// </summary>
        /// <param name="strCode">含有中文字符串的代码</param>
        /// <returns>中文字符串列表</returns>
        public static List<string> GetStringList(string strCode)
        {
            List<string> list = new List<string>();
            GameLuaEditor.DFATable dfa = new GameLuaEditor.DFATable();
            Hashtable table = dfa.getStringList(strCode);
            foreach(string s in table.Values)
            {
                list.Add(s);
            }

            return list;
        }

        /// <summary>
        /// 编码：把带中文的给变成不带中文的
        /// </summary>
        /// <param name="strcode">带中文的编码</param>
        /// <param name="ht_value">编码hash表</param>
        /// <param name="tableName">字符串表的名称</param>
        /// <returns>不带中文的编码</returns>
        public static string Encode(string strcode, Hashtable ht_value, string tableName)
        {
            string result = strcode;

            Hashtable table = GetSubstringHashtable(result);
            List<int> keyList = new List<int>();

            foreach (string s in table.Keys)
            {
                keyList.Add(int.Parse(s));
            }
            keyList.Sort();

            for (int i = keyList.Count - 1; i >= 0; i--)
            {
                int index = keyList[i];
                string content = table[index.ToString()] as string;
                string tempStr = result.Remove(index, content.Length);
                result = tempStr.Insert(index, string.Format("{0}[{1}]", tableName, ht_value[content]));
            }

            return result;
        }
    }
}

