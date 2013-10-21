using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using LuaInterface;
namespace LuaEditLogicHeader
{
    class CodeBuilder
    {
        public void Init(string strAPIInfoFile)
        {
            m_strCode = "";
            m_strAPIInfoFile = strAPIInfoFile;
            m_lua = new Lua();
            m_lua.DoFile(m_strAPIInfoFile);
            m_luatable = m_lua.GetTable("APIInfo");
            m_nClassCount = m_luatable.Keys.Count;
            m_strClass = new string[m_nClassCount];
            m_strClassInCode = new string[m_nClassCount];
            m_strIniFileName = Application.StartupPath + "\\Plugins\\LuaCheck\\API.temp.lua";
            m_lua1 = new Lua();
            m_lua1.DoFile(m_strIniFileName);
            int n = 0;
            foreach (string str in m_luatable.Keys)
            {
                m_strClass[n] = (string)(((LuaTable)m_luatable[str])["Name"]);
                n++;
            }
            
            //object ob;
            //foreach (ob in m_luatable)
            //{
            //    strClass[] = ((LuaTable)ob)["Name"];
            
            //}
            //object ob = m_luatable["1"];
            //object ob1 = ((LuaTable)ob)["Name"];
        }

        public void Add(string strAddCode)
        {
            m_strCode = m_strCode + strAddCode;
        }
        
        public string Code
        {
            get { return m_strCode;}
        }
        // 添加表
        public void AddTableInCode()
        {
            for (int n = 0; n < m_nClassCount; n++)
            {
                // 符合类的规范的，去掉K 改为_ 后面字符全小写
                string str = m_strClass[n];
                if ((str.Substring(0, 1) == "K") && (str.ToUpper() != str))
                {
                    ClassToTable(str, "_" + str.ToLower().Substring(1, str.Length - 1));
                }
                else if (str == "BaseFunction")
                {
                    ClassToTable(str, "");
                }
                else
                {
                    ClassToTable(str, str);
                }
                //StringBuilder sb = new StringBuilder(1024);
                //FileFolderHelper.ReadIniFile("GetTable", m_strClass[n], "NOT_FOUND", sb, 1024, m_strIniFileName);
                //if (sb.ToString() != "NOT_FOUND")
                //    ClassToTable(m_strClass[n], sb.ToString());
                //else
                //    ClassToTable(m_strClass[n], m_strClass[n]);
                //m_strClassInCode[n] = m_strClass[n];
            }
       }
        public void AddEventList()
        {
            string strEventList = "EVENT_LIST = {";
            string str = "";
            LuaTable EventList = (LuaTable)((LuaTable)m_luatable["Event"])["Function"];
            foreach (string strEvent in EventList.Keys)
            {
                str = "\t\t\"" + strEvent + "(";
                LuaTable ParamList = (LuaTable)((LuaTable)EventList[strEvent])["Param"];
                int nCount = ParamList.Keys.Count;
                for (int n = 0; n < nCount; n++ )
                {
                    string sb = GetStringFromFile("ParamForEventList", ((LuaTable)ParamList[n + 1])["Name"].ToString(), ((LuaTable)ParamList[n + 1])["Name"].ToString());
                    str = str +  sb + ", ";
                }
                if (nCount != 0)
                    str = str.Substring(0, str.Length - 2);

                str = str + ")\", \r\n";
                strEventList = strEventList + str;
            }
            strEventList = strEventList.Substring(0, strEventList.Length - 4);
            strEventList = strEventList + "};\r\n\r\n";
            Add(strEventList);
        }
        private string GetStringFromFile(string strTableName, string strFieldName, string strDefReturn)
        {
            LuaTable tbl = m_lua1.GetTable(strTableName);
            string strReturn = "";
            try
            {
                strReturn = (string)tbl[strFieldName];
            }
            catch (Exception e)
            {
                return strDefReturn;
            }
            return strReturn == null ? strDefReturn : strReturn;
        }
        public void AddFunction()
        {
            for (int n = 0; n < m_nClassCount; n++)
            {
                string str = m_strClass[n];
                if ((str.Substring(0, 1) == "K") && (str.ToUpper() != str))
                {
                    AddTableFunction(str, "_" + str.ToLower().Substring(1, str.Length - 1));
                    continue;
                }
                if (str == "BaseFunction")
                {
                    AddTableFunction(str, "");
                }
                //StringBuilder sb = new StringBuilder(1024);
                //FileFolderHelper.ReadIniFile("GetTable", m_strClass[n], "NOT_FOUND", sb, 1024, m_strIniFileName);
                //if (sb.ToString() != "NOT_FOUND")
                //{
                //    AddTableFunction(m_strClass[n], sb.ToString());
                //}
            }
         }
        private void AddTableFunction(string strClassName, string strTableName)
        {
            LuaTable luaClass = (LuaTable)m_luatable[strClassName];
            if (luaClass["Function"] != null)
            {
                string str = "";
                foreach (string strFuncName in ((LuaTable)(luaClass["Function"])).Keys)
                {
                    // 初始化返回值和参数
                    LuaTable luaFunction = (LuaTable)((LuaTable)(luaClass["Function"]))[strFuncName];
                    int nReturnsCount = 0;
                    if (luaFunction["Return"] != null)
                    {
                        nReturnsCount = ((LuaTable)luaFunction["Return"]).Keys.Count; 
                    }
                    int nParamsCount = 0;
                    if (luaFunction["Param"] != null)
                    {
                        nParamsCount = ((LuaTable)luaFunction["Param"]).Keys.Count;
                    }
                    string[] strParamsOptional = new string[nParamsCount];
                    string[] strReturns = new string[nReturnsCount];
                    string[] strParams = new string[nParamsCount];
                    for (int i = 0; i < nReturnsCount; i++)
                    {
                        strReturns[i] = ((LuaTable)((LuaTable)luaFunction["Return"])[i + 1])["ReturnType"].ToString();
                    }
                    for (int i = 0; i < nParamsCount; i++)
                    {
                        strParams[i] = ((LuaTable)((LuaTable)luaFunction["Param"])[i + 1])["Name"].ToString();
                        strParamsOptional[i] = ((LuaTable)((LuaTable)luaFunction["Param"])[i + 1])["Optional"].ToString();
                    }
                    //拼接字符串
                    //函数声明
                    str = strTableName + "." + strFuncName + " = function(";
                    if (strTableName == "")
                    {
                        str = strFuncName + " = function(";
                    }
                    for (int i = 0; i < nParamsCount; i++)
                    {
                        str = str + strParams[i] + ", "; 
                    }
                    if (nParamsCount > 0)
                    {
                        str = str.Substring(0, str.Length - 2);
                    }
                    str = str + ") \r\n";
                    //参数类型检查
                    if (nParamsCount > 0)
                    {
                        str = str + "\tif ";
                        for (int i = 0; i < nParamsCount; i++)
                        {
                            if (strParamsOptional[i].ToLower() == "true")
                            {
                                str = str + "(" + TypeCheck(strParams[i]) + "and (type(" + strParams[i] + ") ~= \"nil\" )) or ";
                            }
                            else
                            {
                                str = str + TypeCheck(strParams[i]) + "or ";
                            }
                        }
                        str = str.Substring(0, str.Length - 3);
                        str = str + "then \r\n";
                        str = str + "\t\tprint(\"[" + strFuncName + "] Args type Error.\"..GetLineCount())\r\n";
                        str = str + "\t\treturn\r\n\tend\r\n";
                    }
                    
                    //返回处理
                    str = str + "\treturn ";
                    for (int i = 0; i < nReturnsCount; i++)
                    { 
                        string sb = GetStringFromFile("GetReturnType", strReturns[i], "");
                        if (sb == "")
                        {
                            MessageBox.Show(strReturns[i] + "没有对应的返回值类型！");
                        }
                        str = str + sb + ", ";
                    }
                    if (nReturnsCount > 0)
                    {
                        str = str.Substring(0, str.Length - 2);
                    }
                    str = str + " \r\n";
                    str = str + "end \r\n\r\n";

                    // 拼接完成   加入代码
                    Add(str);
               }
            }
        }
        private string TypeCheck(string strParam)
        {
            string strReturn = "";
            string sb = GetStringFromFile("GetParamType", strParam, "NOT_FOUND");
            if (sb != "NOT_FOUND")        //先找ini中的特殊处理
            {
                strReturn = sb + " ";
            }
            else if (strParam.Length >= 4 && strParam.Substring(0, 4) == "pcsz") 
            {
                strReturn = "(type(" + strParam + ") ~= \"string\") ";
            }
            else if (strParam.Length >= 3 && strParam.Substring(0, 3) == "psz")
            {
                strReturn = "(type(" + strParam + ") ~= \"string\") ";
            }
            else 
            {
                switch (strParam.Substring(0, 1))
                {
                    case "d": strReturn = "(type(" + strParam + ") ~= \"number\") "; break;
                    case "n": strReturn = "(type(" + strParam + ") ~= \"number\") "; break;   
                    case "b": strReturn = "(type(" + strParam + ") ~= \"boolean\") "; break; //布尔
                    case "s": strReturn = "(type(" + strParam + ") ~= \"string\") "; break; //字符串
                    case "t": strReturn = "(type(" + strParam + ") ~= \"table\") "; break;  //表
                    case "e": strReturn = "(type(" + strParam + ") ~= \"number\") "; break;  //枚举
                    default: MessageBox.Show(strParam + "新的没有定义的参数类型,请更改配置重新生成！"); break;
                }
            }
            return strReturn;
        }
        private void ClassToTable(string strClassName, string  strTableName)
        {
            string strTable = strTableName + " = {";
            if (strTableName == "")
            {
                strTable = strClassName + " = {";
            }
            int nLen = strTable.Length;
            LuaTable luaClass = (LuaTable)m_luatable[strClassName];
            if (luaClass["Property"] != null)
            {
                LuaTable property = (LuaTable)luaClass["Property"];
                string str = "";
                foreach (string strPropertyName in property.Keys)
                {
                    str = strPropertyName;
                    if (strClassName == strTableName) 
                    {
                        int nIndex = 0;
                        object ob = ((LuaTable)property[strPropertyName])["Index"];
                        string strIndex = ob.ToString();
                        nIndex = Int32.Parse(strIndex);
                        str = str + " = " + nIndex.ToString() + ", ";
                    }
                    else 
                    {
                        switch(str.Substring(0, 1))
                        {
                            case "b": str = str + " = true, "; break;           //bool
                            case "d": str = str + " = 0, "; break;              //dw
                            case "n": str = str + " = 1, "; break;               //n
                            case "s": str = str + " = \"" + str + "\", "; break; //sz
                            default: MessageBox.Show(strTableName + "." +str + "没有初始化类的属性的默认值！"); break;
                        }
                    }
                    strTable = strTable + str;
                }
            }
            
            if (luaClass["Function"] != null)
            {
                LuaTable function = (LuaTable)luaClass["Function"];
                string str = "";
                foreach (string strFunctionName in function.Keys)
                {
                    str = strFunctionName + ", ";
                    strTable = strTable + str;
                }
            }
            if (nLen != strTable.Length)
            {
                strTable = strTable.Substring(0, strTable.Length - 2);
            }
            
            strTable += "}\r\n\r\n";
            Add(strTable);
        }
        string m_strCode;
        string m_strAPIInfoFile;
        LuaTable m_luatable;
        Lua m_lua;
        Lua m_lua1;
        string[] m_strClass;
        string[] m_strClassInCode;
        int m_nClassCount;
        string m_strIniFileName;
    }


    class HeaderBuilder
    {
        public void Init()
        {
            m_strDir = Application.StartupPath;
            m_strLuaHeaderFileName = m_strDir + "\\Plugins\\LuaCheck\\LuaHeader.lh";
            m_codebuilder = new CodeBuilder();
            m_codebuilder.Init(m_strDir + "\\Plugins\\LuaCheck\\API.lua");
        }

        public void Run()
        {
            string str = GetFixedCode();
            if (str == "")
                return;
            m_codebuilder.Add(str);

            m_codebuilder.AddEventList();

            m_codebuilder.AddTableInCode();
            m_codebuilder.AddFunction();
            //

            FileFolderHelper.StringToFile(m_codebuilder.Code, m_strLuaHeaderFileName);
        }
        private string GetFixedCode()
        {
            string FixedCodeFile = m_strDir + "\\Plugins\\LuaCheck\\FixedCode.txt";
            string str = FileFolderHelper.FileToString(FixedCodeFile);
            if (str == "")
            {
                MessageBox.Show("缺少必要的文件:" + FixedCodeFile);
            }
            return str;
        }
        private CodeBuilder m_codebuilder;
        private string m_strLuaHeaderFileName;
        private string m_strDir;
    }
}

