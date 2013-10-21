using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Collections;
using GraphElementLibrary;
using DataElementLibrary;

namespace FlowChartEditor.Command.FlowChartCommand
{
    class FlowChartMultiDeleteCommand:FlowChartBaseCommand
    {
        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="data">当前对象</param>
        /// <param name="description">命令的描述</param>
        public FlowChartMultiDeleteCommand(object data, string description):
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
            List<GraphElement> graphElementList = o as List<GraphElement>;

            DialogResult result = MessageBox.Show("是否确定删除选中的图元？", "删除操作确认", MessageBoxButtons.YesNo, MessageBoxIcon.Question);
            if (result == DialogResult.Yes) // 用户确认删除
            {
                graphManager.CurrentCanvas.PaintCanvas = false; // 避免绘制时使用已经释放的资源

                // 保存命令执行前的数据
                if (firstCommand) // 只有第一条命令保存执行前的数据
                {
                    SaveBeforeExecute(flowChartManager.GetArguments());                    
                }   

                description = "批量删除图元";
                foreach(GraphElement graphElement in graphElementList)
                {
                    success = DeleteGraphElement(graphElement);
                    if(!success)
                    {
                        MessageBox.Show(string.Format("ID为{0}的图元删除失败！", graphElement.ID), "删除图元", 
                            MessageBoxButtons.OK, MessageBoxIcon.Information);
                        break;
                    }
                }

                graphElementList.Clear();                

                // 保存命令执行后的数据
                if(success)
                {
                    flowChartManager.ContentChanged = true;
                    SaveAfterExecute(flowChartManager.GetArguments());                    
                }                
            }

            return success;
        }
    }
}
