using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;
using FlowChartEditor.BaseGraphElement;
using GraphElementLibrary;

namespace FlowChartEditor.Command.FlowChartCommand
{
    class FlowChartCopyCommand:FlowChartBaseCommand
    {
        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="data">当前对象</param>
        /// <param name="description">命令的描述</param>
        public FlowChartCopyCommand(object data, string description):base(data, description)
        {

        }

        /// <summary>
        /// 执行命令
        /// </summary>
        /// <param name="o">命令的参数</param>
        /// <returns>是否执行成功</returns>
        public override bool Execute(object o)
        {
            bool success = true;

            // 保存命令执行前的数据
            if (firstCommand) // 只有第一条命令保存执行前的数据
            {
                SaveBeforeExecute(flowChartManager.GetArguments());                
            }

            Hashtable dataTable = new Hashtable();

            if (graphManager.SelectedGraphElementList.Count > 0) // 复制多个图元
            {
                foreach (GraphElement graphElement in graphManager.SelectedGraphElementList)
                {
                    dataTable[graphElement] = dataManager.GetData(graphElement);
                }
            }
            else // 复制单个图元
            {
                GraphElement graphElement = graphManager.SelectedGraphElement;
                dataTable[graphElement] = dataManager.GetData(graphElement);
            }
            
            documentManager.CopyTable = CopyGraphElement(dataTable);

            if (success) // 保存命令执行后的数据
            {
                SaveAfterExecute(flowChartManager.GetArguments());
            }

            return success;
        }
    }
}
