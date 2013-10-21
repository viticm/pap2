using System;
using System.IO;
using System.Collections.Generic;
using System.Text;
using System.Net;
using System.Collections;
using System.Data;

using FlowChartDataLibrary;
using DBManager;
using DataInteractor;
using CodeCompiler;

namespace Compiler_Sword3_AI
{
    public class Main : Compiler
    {
        private DataTable actionTable; // 动作数据表
        private List<string> customFunctionList = new List<string>(); // 自定义动作脚本链表

        /// <summary>
        /// 生成代码主体
        /// </summary>
        /// <param name="code">代码</param>
        /// <param name="metaData">流程图元数据</param>
        /// <param name="list">流程图元数据链表</param>
        private void GenerateCode(StringBuilder code, FlowChartMetaData metaData, List<FlowChartMetaData> list)
        {
            if(!list.Contains(metaData))
            {
                list.Add(metaData);

                switch(metaData.DataType)
                {
                    case "AIStateNode": // AI状态结点
                        {
                            code.AppendLine(string.Format("    -- {0}", metaData.DisplayText));
                            code.AppendLine(string.Format("    state = ai.NewState({0})", 
                                            metaData.Code));

                            List<FlowChartMetaData> nextMetaDataList = new List<FlowChartMetaData>();

                            foreach (FlowChartMetaData aiLine in metaData.NextMetaDataList)
                            {
                                code.AppendLine(string.Format("    state.HandleEvent({0}, {1})",
                                                CodeProviderClass.GetEventString(aiLine.Data), aiLine.NextMetaDataList[0].ID));
                                nextMetaDataList.Add(aiLine.NextMetaDataList[0]);
                            }

                            code.AppendLine();

                            foreach (FlowChartMetaData nextMetaData in nextMetaDataList)
                            {
                                GenerateCode(code, nextMetaData, list);
                            }                            

                            break;
                        }
                    case "AIActionNode": // AI动作结点
                        {
                            if(metaData.DisplayText != "")
                            {
                                code.AppendLine(string.Format("    -- {0}", metaData.DisplayText));
                            }
                            
                            string[] information = CodeProviderClass.GetActionInformation(metaData.Data);
                            int actionID = int.Parse(information[2]);

                            if(actionID > 1000) // 自定义动作
                            {
                                code.AppendLine(string.Format("    action = ai.NewAction({0}, {1})",
                                                              metaData.ID, information[2]));

                                if (actionID > 2000) // ID大于2000的Action为本文件内自定义动作
                                {
                                    customFunctionList.Add(information[3]);
                                }
                            }                            
                            else // 系统动作
                            {
                                code.AppendLine(string.Format("    action = ai.NewAction({0}, AI_ACTION.{1})",
                                    metaData.ID, information[0]));
                            }                            

                            if(information[1] != "")
                            {                                
                                code.AppendLine(string.Format("    action.SetParam({0})", information[1]));
                            }                            
                            
                            List<FlowChartMetaData> nextMetaDataList = new List<FlowChartMetaData>();
                            SortedList<string, string> sortedList = new SortedList<string, string>();

                            foreach(FlowChartMetaData aiLine in metaData.NextMetaDataList)
                            {
                                sortedList.Add(aiLine.Data as string, aiLine.NextMetaDataList[0].ID.ToString());
                                nextMetaDataList.Add(aiLine.NextMetaDataList[0]);
                            }
                            
                            string branchArgs = "";

                            for(int i = 0; i < sortedList.Count; i++)
                            {
                                branchArgs += string.Format("{0}, ", sortedList.Values[i]);
                                
                            }

                            branchArgs = branchArgs.TrimEnd(new char[] { ',', ' ' });                                                        

                            if(branchArgs != "")
                            {
                                code.AppendLine(string.Format("    action.SetBranch({0})", branchArgs));
                            }

                            // 注册调试输出
                            code.AppendLine(string.Format("    g_AIDebugInfo[{0}] = '[{0}] {1}({2})'", metaData.ID.ToString(),
                                                          information[0], information[1].Replace("'", "")));
                            code.AppendLine();
                            
                            foreach(FlowChartMetaData nextMetaData in nextMetaDataList)
                            {
                                GenerateCode(code, nextMetaData, list);
                            }

                            break;
                        }
                    case "AISetStateNode": // AI状态转换结点
                        {
                            code.AppendLine(string.Format("    -- {0}", metaData.DisplayText));
                            code.AppendLine(string.Format("    action = ai.NewAction({0}, AI_ACTION.SET_STATE)", metaData.ID));
                            code.AppendLine(string.Format("    action.SetParam({0})", metaData.NextMetaDataList[0].NextMetaDataList[0].Code));

                            // 注册调试输出
                            code.AppendLine(string.Format("    g_AIDebugInfo[{0}] = '[{0}] SetParam({1})'", metaData.ID.ToString(),
                                                          metaData.NextMetaDataList[0].NextMetaDataList[0].Code));
                            code.AppendLine();

                            GenerateCode(code, metaData.NextMetaDataList[0].NextMetaDataList[0], list);

                            break;
                        }
                }
            }
        }

        /// <summary>
        /// 构建LS表
        /// </summary>
        /// <returns>LS表内容</returns>
        private string ConstructLSFile()
        {
            DataBaseManager dataBaseManager = DataBaseManager.GetDataBaseManager();
            string sqlString = "SELECT * FROM AI_LS";
            DataTable lsTable = dataBaseManager.GetDataTable(sqlString, dataBaseManager.Connection_Jx3web);
            StringBuilder code = new StringBuilder();

            code.AppendLine("ARGUMENT_STRING_TABLE = {");

            for (int i = 0; i < lsTable.Rows.Count; i++)
            {
                DataRow row = lsTable.Rows[i];
                string id = row["ID"].ToString();
                string word = row["Content"].ToString();

                if(i == lsTable.Rows.Count - 1) // 最后一个数据
                {                    
                    if ((word.StartsWith("'") && word.EndsWith("'")) ||
                        (word.StartsWith("\"") && word.EndsWith("\"") ||
                        (word.StartsWith("[[") && word.EndsWith("]]")))) // 检查是否已经带字符串标记
                    {
                        code.AppendLine(string.Format("    [{0}] = {1}", id, word));
                    }
                    else
                    {
                        code.AppendLine(string.Format("    [{0}] = '{1}'", id, word));
                    }                    
                }
                else
                {
                    if ((word.StartsWith("'") && word.EndsWith("'")) ||
                        (word.StartsWith("\"") && word.EndsWith("\"") ||
                        (word.StartsWith("[[") && word.EndsWith("]]")))) // 检查是否已经带字符串标记
                    {
                        code.AppendLine(string.Format("    [{0}] = {1},", id, word));
                    }
                    else
                    {
                        code.AppendLine(string.Format("    [{0}] = '{1}',", id, word));
                    } 
                }
            }

            code.Append("}");

            return code.ToString();
        }

        /// <summary>
        /// 检查动作参数是否是字符串
        /// </summary>
        /// <param name="actionID">动作ID</param>
        /// <param name="paramIndex">参数ID</param>
        /// <returns>动作参数是否是字符串字符串</returns>
        private bool IsParamString(string actionID, int paramIndex, DataTable actionTable)
        {
            bool result = false;

            DataRow[] rows = actionTable.Select(string.Format("actionid = '{0}'", actionID));
            if(rows.Length > 0)
            {               
                if(!(rows[0]["parms"] is DBNull) && rows[0]["parms"].ToString() != "")
                {
                    string[] data = rows[0]["parms"].ToString().Split(new char[] { '\n' });
                    string[] value = data[paramIndex].TrimEnd(new char[] { '\r' }).Split(new char[] { ',' });
                    result = (value[1] == "-2"); 
                }                
            }

            return result;
        }

        /// <summary>
        /// 编辑流程图
        /// </summary>
        /// <param name="FlowID">流程图ID</param>
        /// <param name="data">元数据链表</param>
        /// <param name="manager">元数据管理器</param>
        /// <param name="flowVersion">绘图的版本号</param>
        /// <returns>是否编译成功</returns>
        public override bool GenerateCode(int FlowID, List<FlowChartMetaData> data, FlowChartMetaDataManager manager)
        {            
            // 初始化数据            
            string[] dataArray = scriptPath.Split(new char[] { '\\' });
            string shortPath = dataArray[1];

            for (int i = 2; i < dataArray.Length; i++)
            {
                shortPath = shortPath + string.Format("\\{0}", dataArray[i]);
            }

            string fullPath = string.Format(@"{0}\scripts\Map\{1}\ai\{2}", rootDirectory.TrimEnd(new char[] { '\\' }), dataArray[0], shortPath);

            bool success = true;
            DataBaseManager dataBaseManager = DataBaseManager.GetDataBaseManager();
            customFunctionList.Clear();

            // 初始化参数
            StringBuilder code = new StringBuilder();
            string initialStateName = CodeProviderClass.GetStateString(manager.GetEventData(1)); // 初态的变量名
            List<FlowChartMetaData> metaDataList = manager.GetAllNodes();
            List<FlowChartMetaData> stateList = new List<FlowChartMetaData>();
            List<FlowChartMetaData> actionList = new List<FlowChartMetaData>();
            Hashtable registActions = new Hashtable();

            string sqlString = "SELECT actionid, parms FROM AI_Action_Define";
            actionTable = dataBaseManager.GetDataTable(sqlString, dataBaseManager.Connection_Jx3web);

            foreach (FlowChartMetaData metaData in metaDataList)
            {
                switch (metaData.DataType)
                {
                    case "AIStateNode":
                        {
                            stateList.Add(metaData);
                            break;
                        }
                    case "AIActionNode":
                        {
                            actionList.Add(metaData);
                            string[] information = CodeProviderClass.GetActionInformation(metaData.Data);
                            int actionID = int.Parse(information[2]);

                            if ((actionID == 0) || (actionID > 1000 && registActions[information[2]] == null)) // 注册自定义动作
                            {
                                registActions[information[2]] = metaData;
                            }

                            break;
                        }
                }
            }

            // 添加注释
            code.AppendLine(string.Format("-- 编写者：{0}", Dns.GetHostName()));
            code.AppendLine(string.Format("-- 版本号：{0}", manager.Version));
            code.AppendLine();

            // 引用头文件
            code.AppendLine("Include('scripts/ai/customFuctions.lua')");
            code.AppendLine("Include('scripts/ai/AIParam.lua')");
            code.AppendLine("Include('scripts/ai/argumentStrings.ls')");
            code.AppendLine();
            code.AppendLine("g_AIDebugInfo = {}");
            code.AppendLine();

            // 声明变量
            code.AppendLine("-- 声明状态变量");
            foreach (FlowChartMetaData stateNode in stateList)
            {
                string stateName = stateNode.Code;
                code.AppendLine(string.Format("local {0} = {1} -- {2}", stateName, stateNode.ID, stateNode.DisplayText));
            }
            code.AppendLine();

            // 生成自定义附加脚本
            FlowChartInteractor it = Interactor.GetInteractor().CurrentFlowChartInteractor;
            string customCode = it.CustomText;
            if(customCode != null && customCode != "")
            {
                code.AppendLine("-- 自定义附加脚本");
                code.AppendLine(string.Format("{0}", customCode));
                code.AppendLine();
            }

            // 写主方法
            code.AppendLine("function Setup(ai)");
            code.AppendLine("    local state");
            code.AppendLine("    local action");
            code.AppendLine("    local param = g_AIParam[ai.nAIType]");
            code.AppendLine();

            // 注册自定义动作
            code.AppendLine("    -- 注册自定义动作");
            foreach (object o in registActions.Values)
            {
                FlowChartMetaData actionNode = o as FlowChartMetaData;
                string[] information = CodeProviderClass.GetActionInformation(actionNode.Data);
                code.AppendLine(string.Format("    ai.RegisterUserAction({0}, '{1}')", information[2], information[0]));                
            }            
            code.AppendLine();

            List<FlowChartMetaData> list = new List<FlowChartMetaData>();
            GenerateCode(code, manager.GetEventNode(1), list);

            code.AppendLine(string.Format("    ai.SetInitState({0})", initialStateName));
            code.AppendLine("end");
            code.AppendLine();

            // 调试方法定义
            code.AppendLine("-- 调试方法");
            code.AppendLine("function DebugOnCallAction(action_id)");
            code.AppendLine("    Log('[AI] Call action: ' .. g_AIDebugInfo[action_id])");
            code.AppendLine("end");

            // 写本文件自定义脚本动作定义
            if(customFunctionList.Count > 0)
            {
                code.AppendLine();
                code.AppendLine("-- 本AI图内自定义脚本");

                foreach(string s in customFunctionList)
                {
                    code.AppendLine(string.Format("{0}", s));
                    code.AppendLine();
                }
            }

            // 提取转换中文字符串
            string scriptText = code.ToString();
            scriptText = ConvertScriptText(scriptText);

            // 输出文件
            string fileName = string.Format("{0}.lua", fullPath);
            saveFile(fileName, scriptText);

            string userActionScripts = dataBaseManager.GetUserActionContext();            
            fileName = Path.Combine(rootDirectory, @"scripts\ai\customFuctions.lua");
            saveFile(fileName, userActionScripts);

            fileName = Path.Combine(rootDirectory, @"scripts\ai\argumentStrings.ls");
            string lsContent = ConstructLSFile();
            saveFile(fileName, lsContent);                

            return success;
        }

        /// <summary>
        /// 转换脚本里的字符串
        /// </summary>
        /// <param name="content">脚本内容</param>
        /// <returns>转换后的字符串</returns>
        private string ConvertScriptText(string content)
        {
            CodeConvert codeConvert = CodeConvert.GetCodeConvert();
            codeConvert.ConvertTableName = "ARGUMENT_STRING_TABLE";
            List<string> wordList = codeConvert.GetStringList(content);

            if (wordList.Count > 0) // 需要字符串替换
            {
                DataBaseManager dataBaseManager = DataBaseManager.GetDataBaseManager();
                foreach(string s in wordList)
                {
                    string id = dataBaseManager.InsertIntoLS(s.Replace("'", "''"));
                    content = content.Replace(s, string.Format("{0}[{1}]", codeConvert.ConvertTableName, id));
                }
            }            

            return content;
        }
    }
}
