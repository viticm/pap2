using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Drawing;
using System.Collections;
using FlowChartEditor.BaseGraphElement;
using GraphElementLibrary;

namespace FlowChartEditor.Command.FlowChartCommand
{
    class FlowChartConnectCommand:FlowChartBaseCommand
    {
        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="data">当前对象</param>
        /// <param name="description">命令的描述</param>
        public FlowChartConnectCommand(object data, string description):
            base(data, description)
        {

        }        

        /// <summary>
        /// 执行命令
        /// </summary>
        /// <param name="o">当前对象</param>
        /// <returns>是否执行成功</returns>
        public override bool Execute(object o)
        {
            bool success = true;
            Helper helper = Helper.GetHelper();
            object[] args = o as object[];
            SlotGraphElement  slot = args[0] as SlotGraphElement;
            ConnectorGraphElement connector = args[1] as ConnectorGraphElement;

            // 保存命令执行前的数据
            if (firstCommand) // 只有第一条命令保存执行前的数据
            {
                SaveBeforeExecute(flowChartManager.GetArguments());                
            }            

            description = "连接图元 " + slot.SlotContainer.Name;
            slot.Bind(connector);                        

            // 执行逻辑功能
            LogicBaseManager logicManager = helper.GetLogicManager(flowChartManager.CurrentChartMode);
            success = logicManager.DoLogicOperation(flowChartManager, LogicType.Connect, o);

            if (success) // 保存命令执行后的数据
            {
                flowChartManager.ContentChanged = true;
                SaveAfterExecute(flowChartManager.GetArguments());                
            }

            return success;
        }
    }
}
