using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Windows.Forms;
using System.Collections;
using FlowChartEditor.BaseGraphElement;
using GraphElementLibrary;
using DataElementLibrary;

namespace FlowChartEditor.Command.FlowChartCommand
{
    class FlowChartCreateCommand:FlowChartBaseCommand
    {
        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="data">当前对象</param>
        /// <param name="description">命令的描述</param>
        public FlowChartCreateCommand(object data, string description):
            base(data, description)
        {

        }

        /// <summary>
        /// 初始化图结点
        /// </summary>
        /// <param name="graphElement">结点对象</param>
        /// <param name="id">结点ID</param>
        /// <param name="name">结点名称</param>
        private void InitSlotContainer(SlotContainer slotContainer, int id, string name)
        {
            slotContainer.ID = id;
            slotContainer.Name = name;
            slotContainer.Init();
            graphManager.SlotContainerList.Add(slotContainer);

            description = "创建图元 " + slotContainer.Name;
            dataManager.AddDataElement(slotContainer);            
            graphManager.SelectGraphElement(slotContainer, false);
            graphManager.ReconstructCanvasGraphElementList(); 
        }

        /// <summary>
        /// 执行命令
        /// </summary>
        /// <param name="o">当前对象</param>
        /// <returns>是否执行成功</returns>
        public override bool Execute(object o)
        {
            bool success = true;
            object[] args = o as object[];
            int id = graphManager.AllocateGraphElementID();
            Point p = (Point)args[0];
            string typeString = args[1] as string;
            GraphSetting graphSetting = GraphSetting.GetGraphSetting();

            // 保存命令执行前的数据
            if (firstCommand) // 只有第一条命令保存执行前的数据
            {
                SaveBeforeExecute(flowChartManager.GetArguments());
            }   

            switch (typeString)
            {
                case "ConditionNode": // 条件结点
                    {
                        ConditionGraphElement conditionGraphElement = new ConditionGraphElement(p, graphSetting.ConditionNodeElementSize);                        
                        InitSlotContainer(conditionGraphElement, id, "条件结点");

                        break;
                    }
                case "ActionNode": // 动作结点
                    {
                        ActionGraphElement actionGraphElement = new ActionGraphElement(p, graphSetting.ActionNodeElementSize);
                        InitSlotContainer(actionGraphElement, id, "动作结点");

                        break;
                    }
                case "EventNode": // 事件结点
                    {
                        EventGraphElement eventGraphElement = new EventGraphElement(p, graphSetting.EventNodeElementSize);                        
                        InitSlotContainer(eventGraphElement, id, "事件结点");

                        break;
                    }
                case "AIStateNode": // ai状态结点
                    {
                        AIStateGraphElement aiStateGraphElement = new AIStateGraphElement(p, graphSetting.AIStateNodeElementSize);                        
                        InitSlotContainer(aiStateGraphElement, id, "AI状态结点");

                        break;
                    }
                case "AIActionNode": // ai动作结点
                    {
                        AIActionGraphElement aiActionGraphElement = new AIActionGraphElement(p, graphSetting.AIActionNodeElementSize);                        
                        InitSlotContainer(aiActionGraphElement, id, "AI动作结点");

                        break;
                    }
                case "AIActionsNode": // ai动作组结点
                    {
                        AIActionsGraphElement aiActionsGraphElement = new AIActionsGraphElement(p, graphSetting.AIActionsNodeElementSize);                        
                        InitSlotContainer(aiActionsGraphElement, id, "AI动作组结点");

                        break;
                    }
                case "InnerChart": // 子绘图结点
                    {
                        InnerChart innerChart = new InnerChart(p, graphSetting.InnerChartElementSize);                        
                        InitSlotContainer(innerChart, id, "子绘图结点");

                        break;
                    }
                case "InterfaceNode": // 接口结点
                    {
                        InterfaceGraphElement interfaceGraphElement = new InterfaceGraphElement(p, graphSetting.InterfaceNodeElementSize);                        
                        InitSlotContainer(interfaceGraphElement, id, "接口结点");

                        break;
                    }
            }
            
            if (success) // 保存命令执行后的数据
            {
                flowChartManager.ContentChanged = true;                
                SaveAfterExecute(flowChartManager.GetArguments());
            }

            return success;
        }
    }
}
