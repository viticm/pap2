/* -----------------------------------------------------------------------*\
 * 文件名     :  Functions.cs
 * 创建者     :  魏伟存
 * 创建时间   :  2002-1-22 13:49:21
 * 功能描述   :  功能函数与其它工具函数类
 * 注意事项   :  
 * -----------------------------------------------------------------------*/

using System;
using System.Data ;
using System.Data.SqlClient;
using System.IO;

namespace XSJ.Sword3.Editor.Studio
{
    /// <summary>
    /// XML转义函数库
    /// </summary>
    public class XMLFunctionLib
    {
        /// <summary>
        /// 因为FullDescription字段存储的格式是一份完整的XML文档,所以要将"<>"转义存储.
        /// </summary>
        /// <param name="oristring">要转义的源字符串</param>
        /// <returns>返回转义后的字符串</returns>
        static public string TransformXMLCharacters(string oristring)
        {
            string retstring = string.Empty;
            retstring = oristring.Replace("&", "&amp;");
            retstring = retstring.Replace("<", @"&lt;");
            retstring = retstring.Replace(">", @"&gt;");
            return retstring;
        }
        /// <summary>
        /// 将已经被转义过的XML标记符还原
        /// </summary>
        /// <param name="transformedString">被转义过的字符串</param>
        /// <returns>返回还原的字符串</returns>
        static public string UnTransformXMLCharacters(string transformedString)
        {
            string retstring = string.Empty;
            retstring = transformedString.Replace(@"&amp;", "&");
            retstring = retstring.Replace(@"&lt;", "<");
            retstring = retstring.Replace(@"&gt;", ">");
            return retstring;
        }
    }
    public class HtmlFunctionLib
    {
        /// <summary>
        /// 将所有换行符转换为"<BR>"再输出
        /// </summary>
        /// <param name="inStr">输入的字符串</param>
        /// <returns>返回结果</returns>
        static public string OutputHTML(string inStr)
        {
            string tmpstr;
            tmpstr = inStr.Replace("\n", "<BR>");
            tmpstr = tmpstr.Replace(" ", "&nbsp;");
            return tmpstr;
        }
        /// <summary>
        /// 打开IE窗口的样式
        /// </summary>
        public class IEWinStyle
        {
            public const string noToolbar = "'toolbar=no,location=no,directories=no,status=no,scrollbars=yes,resizable=yes,copyhistory=no,width=600,height=400'";
            public const string noToolbar2 = "'toolbar=no,location=no,directories=no,menubar=yes,titlebar=no,status=yes,scrollbars=yes,resizable=yes,copyhistory=no'";
        }
        /// <summary>
        /// 截取字符串
        /// </summary>
        /// <param name="inStr">源字符串</param>
        /// <param name="length">截取长度</param>
        /// <returns>返回截取后的字符串</returns>
        static public string SubString(string inStr, int length)
        {
            string _retStr = "";
            if (inStr.Length > 0)
            {
                try
                {
                    _retStr = inStr.Substring(0, length);
                    _retStr += "...";
                    _retStr = "<ACRONYM title='" + inStr + "'>" + _retStr + "</ACRONYM>";
                }
                catch
                {
                    _retStr = inStr;
                }
            }
            return _retStr;
        }
        /// <summary>
        /// 按指定的文本长度进行折行(会失真!失真率为1%,最后20个字符丢失)
        /// </summary>
        /// <param name="inStr">源字符串</param>
        /// <param name="length">每行字符个数</param>
        /// <returns>返回处理后的字符串</returns>
        static public string WrapText(string inStr, int length)
        {
            string _OrginalStr = inStr;
            string _retStr = "";///转换后的最终字符串
            string[] _tmpStr;
            int _inStrLen = inStr.Length;
            int _fragment = 0;
            int _basePtr = 0;
            if (inStr.Length < length) { return inStr; }
            if (inStr.Length > 0)
            {
                try
                {
                    _fragment = _inStrLen / length;
                    _tmpStr = new String[_fragment];
                    ///分离成若干段!
                    for (int i = 0; i < _fragment; i++)
                    {
                        _tmpStr[i] = inStr.Substring(_basePtr, length);
                        ///在每段后加"<BR>",即HTML换行符
                        _tmpStr[i] += "<BR>";
                        _retStr += _tmpStr[i];
                        _basePtr += length + 1;
                    }
                }
                catch
                {
                    //_retStr = _OrginalStr;
                }
            }
            return _retStr;
        }
    }
    //=============================以下为一般函数类=============================================
    /// <summary>
    /// 一般的函数类.
    /// </summary>
    public class Functions
    {
        /// <summary>
        /// 功能函数
        /// </summary>
        public Functions()
        {
            //
            // TODO: Add constructor logic here
            //
        }
        /// <summary>
        /// Creates a temporary password of a specified length.
        /// </summary>
        /// <param name="length">The maximum length of the temporary password to create.</param>
        /// <returns>A temporary password less than or equal to the length specified.</returns>
        public static String CreateTemporaryPassword(int length)
        {
            string strTempPassword = Guid.NewGuid().ToString("N");
            for (int i = 0; i < (length / 32); i++)
            {
                strTempPassword += Guid.NewGuid().ToString("N");
            }
            return strTempPassword.Substring(0, length);
        }

        /// <summary>
        /// 从表字段读取出来的字符串可能含有(双引号),需要将其转义成C#的格式.
        /// </summary>
        /// <param name="inStr">需要转义的字符串</param>
        /// <returns>返回转义后的字符串</returns>
        static public string ConvertStringForCSharp(string inStr)
        {
            string strtmp = inStr;
            if (inStr.Length > 0)
            {
                strtmp = inStr.Replace("'", "\'");
                strtmp = strtmp.Replace((char)0x3D, (char)0xA3A2);
                strtmp = strtmp.Replace("\"", "\"");
            }
            return strtmp;
        }
        /// <summary>
        /// 为了避免用户输入"'"(单引号),和(双引号),因为SQL SERVER会出错,所以需将其转义.
        /// </summary>
        /// <param name="inStr">需要转义的字符串</param>
        /// <returns>返回转义后的字符串</returns>
        static public string ConvertStringForDB(string inStr)
        {
            string strtmp = inStr;
            if (inStr.Length > -1)
            {
                strtmp = strtmp.Replace("'", "''");
                strtmp = strtmp.Replace("\"", "\"");
            }
            return strtmp;
        }
        /// <summary>
        /// 过滤非法字符,主要用于用户登录
        /// </summary>
        /// <param name="inStr"></param>
        /// <returns></returns>
        static public string VerifyString2(string inStr)
        {
            string strtmp = "";
            strtmp = inStr.Replace("'", "");
            strtmp = strtmp.Replace(" ", "");
            strtmp = strtmp.Replace("@", "");
            strtmp = strtmp.Replace("#", "");
            strtmp = strtmp.Replace("!", "");
            strtmp = strtmp.Replace("&", "");
            strtmp = strtmp.Replace("^", "");
            strtmp = strtmp.Replace("$", "");
            strtmp = strtmp.Replace("+", "");
            strtmp = strtmp.Replace("?", "");
            strtmp = strtmp.Replace("/", "");
            strtmp = strtmp.Replace("\\", "");
            strtmp = strtmp.Replace((char)0x22, (char)'‘');
            return strtmp;
        }
    }


    /// <summary>
    /// 翻页动作
    /// </summary>
    public enum PageAction
    {
        NextPage = 1, PreviousPage, FirstPage, LastPage
    }
    /// <summary>
    /// 操作符类
    /// </summary>
    public class Operater
    {
        /// <summary>
        /// 运算符的结构
        /// </summary>
        public struct TOperater
        {
            public string Name;
            public string Value;
        }
        /// <summary>
        /// 逻辑运算符
        /// </summary>
        public struct TLogic
        {
            public string Name;
            public string Value;
        }
        /// <summary>
        /// 获取数据字段运算的操作符
        /// </summary>
        /// <returns>返回所有操作符</returns>
        public TOperater[] GetOperaters()
        {
            TOperater[] _ops = new TOperater[8];
            _ops[0].Name = "等于";
            _ops[0].Value = "=";
            _ops[1].Name = "大于";
            _ops[1].Value = ">";
            _ops[2].Name = "小于";
            _ops[2].Value = "<";
            _ops[3].Name = "不等于";
            _ops[3].Value = "<>";
            _ops[4].Name = "大于等于";
            _ops[4].Value = ">=";
            _ops[5].Name = "小于等于";
            _ops[5].Value = "<=";
            _ops[6].Name = "类似于";
            _ops[6].Value = "like";
            _ops[7].Name = "不类似于";
            _ops[7].Value = "not like";
            return _ops;
        }
        public TLogic[] GetLogicOperaters()
        {
            TLogic[] _lgs = new TLogic[2];
            _lgs[0].Name = "并且";
            _lgs[0].Value = " and ";
            _lgs[1].Name = "或者";
            _lgs[1].Value = " or ";
            return _lgs;
        }
    }
    /// <summary>
    /// Summary description for TypeConverter.
    /// </summary>
    public class TypeConverter
    {
        public TypeConverter()
        {
            //
            // TODO: Add constructor logic here
            //
        }
        static public bool IsNumberic(char chr)
        {
            switch (chr.ToString())
            {
                case "1":
                case "2":
                case "3":
                case "4":
                case "5":
                case "6":
                case "7":
                case "8":
                case "9":
                case "0":
                    return true;

            }
            return false;
        }
        static public int HexToInteger(string HexString)
        {
            return System.Convert.ToInt32(HexString, 16);
        }
        static public string IntegerToHex(int IntValue, int length)
        {
            return IntValue.ToString("X" + length.ToString());
        }
    }
}

