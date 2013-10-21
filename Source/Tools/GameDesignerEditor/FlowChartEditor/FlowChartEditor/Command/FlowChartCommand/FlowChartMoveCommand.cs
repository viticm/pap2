using System;
using System.Collections.Generic;
using System.Collections;
using System.Text;
using System.Drawing;
using FlowChartEditor.BaseGraphElement;
using GraphElementLibrary;

namespace FlowChartEditor.Command.FlowChartCommand
{
    class FlowChartMoveCommand:FlowChartBaseCommand
    {
        private Point location; // 当前图元的位置

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="data">当前对象</param>
        /// <param name="description">命令的描述</param>
        public FlowChartMoveCommand(object data, string description):
            base(data, description)
        {

        }

        /// <summary>
        /// 保存当前的状态
        /// </summary>
        public void SaveCurrentState(object o)
        {            
            location = (Point)o;

            // 保存命令执行前的数据
            SaveBeforeExecute(flowChartManager.GetArguments());            
        }

        /// <summary>
        /// 执行命令
        /// </summary>
        /// <param name="o">当前对象</param>
        /// <returns>是否执行成功</returns>
        public override bool Execute(object o)
        {
            bool success = false;            
            object[] args = o as object[];
            GraphElement graphElement = args[0] as GraphElement;
            Point newLocation = (Point)args[1];

            if(location != newLocation) // 图元被移动过
            {
                description = "移动图元 " + graphElement.Name;
                if(graphElement is ResizeControler)
                {
                    description = "放缩图元 " + (graphElement as ResizeControler).Owner.Name;
                }
                // 保存命令执行后的数据
                flowChartManager.ContentChanged = true;
                SaveAfterExecute(flowChartManager.GetArguments());                
                success = true;
            }            

            return success;
        } 
    }
}
