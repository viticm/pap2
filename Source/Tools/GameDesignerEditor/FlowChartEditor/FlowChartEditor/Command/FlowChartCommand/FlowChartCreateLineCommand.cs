using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using FlowChartEditor.BaseGraphElement;

namespace FlowChartEditor.Command.FlowChartCommand
{
    class FlowChartCreateLineCommand:FlowChartBaseCommand
    {
        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="data">当前对象</param>
        /// <param name="description">命令的描述</param>
        public FlowChartCreateLineCommand(object data, string description):
            base(data, description)
        {

        }

        /// <summary>
        /// 执行命令
        /// </summary>
        /// <param name="o">当前对象</param>
        /// <returns>是否编辑成功</returns>
        public override bool Execute(object o)
        {
            bool success = true;
            Helper helper = Helper.GetHelper();
            object[] args = o as object[];
            SlotGraphElement slot = args[0] as SlotGraphElement;

            // 保存命令执行前的数据
            if (firstCommand) // 只有第一条命令保存执行前的数据
            {                
                SaveBeforeExecute(flowChartManager.GetArguments());
            }   
            
            // 执行逻辑操作
            LogicBaseManager logicManager = helper.GetLogicManager(flowChartManager.CurrentChartMode);
            logicManager.DoLogicOperation(flowChartManager, LogicType.CreateLine, o);

            ConnectorContainer line = graphManager.SelectedGraphElement as ConnectorContainer;
            graphManager.ConnectorContainerList.Add(line);
            description = "创建图元 " + line.Name;
            dataManager.AddDataElement(line);
            graphManager.ReconstructCanvasGraphElementList();

            // 执行逻辑操作
            object[] logicData = new object[] { slot, slot.BindingConnector };
            logicManager.DoLogicOperation(flowChartManager, LogicType.Connect, logicData);

            if(success) // 保存命令执行后的数据
            {
                flowChartManager.ContentChanged = true;
                SaveAfterExecute(flowChartManager.GetArguments());                
            }                        

            return success;
        }
    }
}
