using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Collections;
using GraphElementLibrary;
using DataElementLibrary;

namespace FlowChartEditor.Command.FlowChartCommand
{
    class FlowChartDeleteCommand:FlowChartBaseCommand
    {
        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="data">当前对象</param>
        /// <param name="description">命令的描述</param>
        public FlowChartDeleteCommand(object data, string description):
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
            GraphElement graphElement = o as GraphElement;           

            DialogResult result = MessageBox.Show("是否确定删除选中的图元？", "删除操作确认", MessageBoxButtons.YesNo, MessageBoxIcon.Question);
            if (result == DialogResult.Yes) // 用户确认删除
            {                
                graphManager.CurrentCanvas.PaintCanvas = false; // 避免绘制时使用已经释放的资源

                // 保存命令执行前的数据
                if (firstCommand) // 只有第一条命令保存执行前的数据
                {                    
                    SaveBeforeExecute(flowChartManager.GetArguments());
                }   

                description = "删除图元 " + graphElement.Name;
                success = DeleteGraphElement(graphElement);                                                                
            }
            else
            {
                success = false;
            }

            // 保存命令执行后的数据
            if(success)
            {
                flowChartManager.ContentChanged = true;
                SaveAfterExecute(flowChartManager.GetArguments());                
            }

            return success;
        }
    }
}
