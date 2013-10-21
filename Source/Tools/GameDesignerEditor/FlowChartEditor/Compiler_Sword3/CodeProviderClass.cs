using System;
using System.Collections.Generic;
using System.Text;
using BaseExp;
using ActionNode;
using ConditionNode;
using ConditionLine;

namespace CodeProvider
{
    public static class CodeProviderClass
    {
        public static string ConvertToCode(object data, string NodeTypeString)
        {
            string code = "";
            if (data == null)
            {
                return code;
            }
            
            //动作节点
            if (data is ActionData)
            {
                ActionExp[] expList = (data as ActionData).ActionExpArray;
                if (expList != null)
                {
                    StringBuilder sb = new StringBuilder();
                    foreach (ActionExp exp in expList)
                    {
                        sb.Append("\t\t" + ExpToString(exp) + ";\n");
                    }
                    code = sb.ToString();
                }
                return code;
            }

            //条件节点
            if (data is ConditionData)
            {
                code = ExpToString((data as ConditionData).datalist[0] as Exp);
                return "local ret = " + code + ";";
            }

            //条件线
            if (data is Exp)
            {
                code = ExpToString(data as Exp);
                return code;
            }
            return code;
        }

        public static string ExpToString(Exp exp)
        {
            if(exp is ActionExp)
            {
                ActionExp aexp = exp as ActionExp;
                string strRet = string.Format("API.fun_{0}_{1}(",aexp.API.DBID, aexp.API.strName);
                if (aexp.API.isAsyn)
                {
                    strRet += "context, ";
                }
                foreach ( Exp e in aexp.API.ArgValues)
                {
                    strRet += ExpToString(e) + ", ";
                }
                strRet = strRet.TrimEnd(new char[] { ' ', ',' });
                strRet += ")";

                if (aexp.API.isAsyn)
                {
                    strRet = "(function() local argslist = SaveArgsToID(arg) local tmp = " + strRet + " arg = RestoreArgsFromID(argslist) return tmp end)()";
                }
                return strRet;
            }
            else if (exp is ConstExp)
            {
                ConstExp cexp = exp as ConstExp;
                /*int[] strIDList = new int[] { 
                    04, 27, 13 ,28, 32, 33, 36, 37, 39, 40, 41, 45, 47, 48, 49, 50, 
                    51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 
                    67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 80, 81, 82, 83, 85 
                };  //需要生成时加双引号的类型的ID
                List<int> l = new List<int>();
                l.AddRange(strIDList);
                if (l.Contains(cexp.ReturnType.DBID))        //字符串，加引号
                {
                    return string.Format("\"{0}\"", cexp.DBValue.Replace("\"", "\\\""));
                }
                else
                {                    
                    return cexp.DBValue;
                }*/

                if(cexp.ReturnType.genAsString) // 字符串加引号
                {
                    return string.Format("\"{0}\"", cexp.DBValue.Replace("\"", "\\\""));
                }
                else
                {
                    return cexp.DBValue;
                }
            }
            else if (exp is ObjectExp)
            {
                return string.Format("arg[{0}]", (exp as ObjectExp).AsEventArgPos + 1); //是事件的第几个参数, 0是第1个
            }
            else if( exp is VarExp)
            {
                return "ret";
            }
            else
            {
                return "";
            }
        }
    }
}
