using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;
using FlowChartEditor.BaseGraphElement;
using GraphElementLibrary;

namespace FlowChartEditor.Command.FlowChartCommand
{
    class FlowChartCutCommand:FlowChartBaseCommand
    {
        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="data">当前对象</param>
        /// <param name="description">命令的描述</param>
        public FlowChartCutCommand(object data, string description):base(data, description)
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
            List<GraphElement> list = new List<GraphElement>();

            // 保存命令执行前的数据
            if (firstCommand) // 只有第一条命令保存执行前的数据
            {
                SaveBeforeExecute(flowChartManager.GetArguments());                
            }            

            if (graphManager.SelectedGraphElementList.Count > 0) // 剪切多个图元
            {                
                List<GraphElement> graphElementList = graphManager.SelectedGraphElementList;
                description = "剪切图元组";

                foreach(GraphElement graphElement in graphElementList)
                {
                    if (graphElement is SlotContainer) // 图元是插槽容器
                    {
                        SlotContainer slotContainer = graphElement as SlotContainer;
                        
                        foreach (SlotGraphElement slot in slotContainer.GetSlotList())
                        {
                            if(slot.Binded && !graphElementList.Contains(slot.BindingConnector.Line))
                            {
                                slot.UnBind();
                            }
                        }

                        graphManager.SlotContainerList.Remove(slotContainer);
                    }
                    else if (graphElement is ConnectorContainer) // 图元是连接线
                    {
                        ConnectorContainer line = graphElement as ConnectorContainer;
                        List<SlotGraphElement> deleteSlotList = new List<SlotGraphElement>();

                        if(line.InSlotContainer != null && !graphElementList.Contains(line.InSlotContainer))
                        {
                            line.UnbindInSlotContainer();
                        }

                        graphManager.ConnectorContainerList.Remove(line);
                    }

                    list.Add(graphElement);
                }
            }
            else // 剪切单个图元
            {                
                GraphElement graphElement = graphManager.SelectedGraphElement;
                description = "剪切图元 " + graphElement.Name;

                if(graphElement is SlotContainer) // 图元是插槽容器
                {
                    SlotContainer slotContainer = graphElement as SlotContainer;                    

                    slotContainer.UnBind();
                    graphManager.SlotContainerList.Remove(slotContainer);
                }
                else if(graphElement is ConnectorContainer) // 图元是连接线
                {
                    ConnectorContainer line = graphElement as ConnectorContainer;

                    if(line.InSlotContainer != null)
                    {
                        line.UnbindInSlotContainer();
                    }

                    if(line.OutSlotContainer != null)
                    {
                        line.UnbindOutSlotContainer();
                    }

                    graphManager.ConnectorContainerList.Remove(line);
                }

                list.Add(graphElement);
            }

            Hashtable cutTable = new Hashtable();
            foreach(GraphElement g in list)
            {
                cutTable[g] = dataManager.GetData(g);
            }
            documentManager.CopyTable = cutTable;
            
            if(success) // 保存命令执行后的数据
            {
                flowChartManager.ContentChanged = true;
                SaveAfterExecute(flowChartManager.GetArguments());                
            }            

            return success;
        }
    }
}
