using System;
using System.Collections.Generic;
using System.Text;
using FlowChartEditor.BaseGraphElement;
using GraphElementLibrary;

namespace FlowChartEditor.Command.FlowChartCommand
{
    class FlowChartEditCommand:FlowChartBaseCommand
    {
        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="data">当前对象</param>
        /// <param name="description">命令的描述</param>
        public FlowChartEditCommand(object data, string description):
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
            GraphElement graphElement = o as GraphElement;

            // 保存执行前的数据
            description = "编辑图元 " + graphElement.Name;
            if (firstCommand) // 只有第一条命令保存执行前的数据
            {
                SaveBeforeExecute(flowChartManager.GetArguments());                
            }   

            success = graphManager.CurrentFlowChartManager.EditDataElement(graphElement);            

            if(success) // 保存执行后的数据
            {
                flowChartManager.ContentChanged = true;
                SaveAfterExecute(flowChartManager.GetArguments());                
            }                        

            return success;
        }
    }
}
