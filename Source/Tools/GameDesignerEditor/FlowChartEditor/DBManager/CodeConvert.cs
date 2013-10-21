using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;
using System.Data;

namespace DBManager
{
    public class CodeConvert
    {
        private static CodeConvert codeConvert; // 文本转换类实例
        private string convertTableName; // 需要转换的表名

        /// <summary>
        /// 私有构造函数
        /// </summary>
        private CodeConvert()
        {

        }

        /// <summary>
        /// 需要转换的表名
        /// </summary>
        public string ConvertTableName
        {
            set
            {
                convertTableName = value;
            }

            get
            {
                return convertTableName;
            }
        }

        /// <summary>
        /// 获取文本转换类实例
        /// </summary>
        public static CodeConvert GetCodeConvert()
        {
            if (codeConvert == null)
            {
                codeConvert = new CodeConvert();
            }

            return codeConvert;
        }

        /// <summary>
        /// 解码：把SCENE_STRING_TABLE['123']变成汉字
        /// </summary>
        /// <param name="strCode">源代码</param>
        /// <param name="ls_ht">ls表 Key为数字字符串，Value为中文字符串</param>
        /// <returns>带中文的代码</returns>
        public string Decode(string strCode, Hashtable ls_ht)
        {
            string tableString = convertTableName + "[";
            string[] asp = { tableString };
            string[] as_str = strCode.Split(asp, StringSplitOptions.None);

            for (int i = 1; i < as_str.Length; i++)
            {
                if ((as_str[i].IndexOf("]")) == -1) continue;
                string strKey = as_str[i].Substring(0, as_str[i].IndexOf("]"));
                if (!ls_ht.ContainsKey(strKey))
                {
                    continue;
                }
                strCode = strCode.Replace(tableString + strKey.Trim() + "]", ls_ht[strKey].ToString());
            }

            return strCode;
        }

        /// <summary>
        /// 获取包含中文字符串信息的hashtable
        /// </summary>
        /// <param name="strCode">原字符串</param>
        /// <returns>包含中文字符串信息的hashtable</returns>
        private Hashtable GetSubstringHashtable(string strCode)
        {
            DFATable dfa = new DFATable();

            return dfa.getStringList(strCode);
        }

        /// <summary>
        /// 判断脚本是否需要ls文件
        /// </summary>
        /// <param name="content">脚本内容</param>
        /// <returns>是否需要ls文件</returns>
        public static bool RequireLsFile(string content)
        {
            return content.Contains("SCENE_STRING_TABLE[");
        }

        /// <summary>
        /// 将ls数据行转换为ls文件中的字符串
        /// </summary>
        /// <param name="lsRows"></param>
        /// <returns></returns>
        public static string ConvertLsFileContent(DataRow[] lsRows)
        {
            string data = "Include('scripts/Include/String.lh')\r\nSCENE_STRING_TABLE = {\r\n";
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
        /// 编码：把带中文的给变成不带中文的
        /// </summary>
        /// <param name="strcode">带中文的源代码</param>
        /// <param name="ht_value">中文在ls中的key的对应表。Key: 中文字符串 Value: 数字字符串</param>
        /// <returns>可以入库的、不带中文的源代码</returns>
        private string EncodeString(string strcode, Hashtable ht_value)
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
                result = tempStr.Insert(index, string.Format("{0}[{1}]", convertTableName, ht_value[content]));
            }

            return result;
        }

        /// <summary>
        /// 查找代码中所有中文字符串
        /// </summary>
        /// <param name="strCode">含有中文字符串的代码</param>
        /// <returns>中文字符串列表</returns>
        public List<string> GetStringList(string strCode)
        {
            List<string> list = new List<string>();
            DFATable dfa = new DFATable();
            Hashtable table = dfa.getStringList(strCode);
            foreach (string s in table.Values)
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
        /// <returns>不带中文的编码</returns>
        public string Encode(string strcode, Hashtable ht_value)
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
                result = tempStr.Insert(index, string.Format("{0}[{1}]", convertTableName, ht_value[content]));
            }

            return result;
        }
    }
}
