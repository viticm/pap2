using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using FlowChartEditor.BaseGraphElement;
using GraphElementLibrary;

namespace FlowChartEditor.Command.FlowChartCommand
{
    class FlowChartCreateAndConnectLineCommand:FlowChartBaseCommand
    {
        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="data">当前对象</param>
        /// <param name="description">命令的描述</param>
        public FlowChartCreateAndConnectLineCommand(object data, string description):
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
            SlotGraphElement outSlot = args[0] as SlotGraphElement;
            SlotGraphElement inSlot = args[1] as SlotGraphElement;

            // 保存命令执行前的数据
            if (firstCommand) // 只有第一条命令保存执行前的数据
            {                
                SaveBeforeExecute(flowChartManager.GetArguments());
            }

            // 执行逻辑操作
            LogicBaseManager logicManager = helper.GetLogicManager(flowChartManager.CurrentChartMode);
            logicManager.DoLogicOperation(flowChartManager, LogicType.CreateAndConnectLine, o);

            ConnectorContainer line = graphManager.SelectedGraphElement as ConnectorContainer;
            outSlot.Bind(line.GetConnectorList()[0]);
            inSlot.Bind(line.GetConnectorList()[1]);
            
            graphManager.ConnectorContainerList.Add(line);            
            graphManager.SelectGraphElement(line, false);
            description = "创建并连接图元 " + outSlot.SlotContainer.Name + " - " + inSlot.SlotContainer.Name;
            dataManager.AddDataElement(line);
            graphManager.ReconstructCanvasGraphElementList();

            // 执行逻辑操作
            object[] logicData = new object[]{ outSlot, outSlot.BindingConnector };
            logicManager.DoLogicOperation(flowChartManager, LogicType.Connect, logicData);

            if (success) // 保存命令执行后的数据
            {
                flowChartManager.ContentChanged = true;                
                SaveAfterExecute(flowChartManager.GetArguments());
            }            

            return success;
        }
    }
}
