using System;
using System.Collections.Generic;
using System.Text;
using GraphElementLibrary;

namespace FlowChartEditor
{
    class FlowChartEditCommand:FlowChartCommand
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
            bool success = false;
            GraphManager graphManager = data as GraphManager;
            DataManager dataManager = graphManager.CurrentFlowChartManager.CurrentDataManager;
            GraphElement graphElement = o as GraphElement;

            // 保存执行前的数据
            description = "编辑图元 " + graphElement.Name;
            dataBeforeExecute.SaveData(dataManager.GetArguments());

            success = graphManager.CurrentFlowChartManager.EditDataElement(graphElement);

            // 保存执行后的数据
            dataAfterExecute.SaveData(dataManager.GetArguments());

            return success;
        }
    }
}
