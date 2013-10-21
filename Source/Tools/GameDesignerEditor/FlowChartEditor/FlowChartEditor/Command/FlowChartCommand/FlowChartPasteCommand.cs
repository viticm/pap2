using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;
using System.Drawing;
using FlowChartEditor.BaseGraphElement;
using GraphElementLibrary;

namespace FlowChartEditor.Command.FlowChartCommand
{
    class FlowChartPasteCommand:FlowChartBaseCommand
    {
        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="data">当前对象</param>
        /// <param name="description">命令的描述</param>
        public FlowChartPasteCommand(object data, string description):base(data, description)
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
            Point currentPoint = (Point)o;
            Size moveSize = Size.Empty;

            // 保存命令执行前的数据
            if (firstCommand) // 只有第一条命令保存执行前的数据
            {                
                SaveBeforeExecute(flowChartManager.GetArguments());
            }

            description = "粘贴图元";
            Hashtable table = CopyGraphElement(documentManager.CopyTable);

            // 计算图元粘贴位置偏移
            List<GraphElement> graphElementList = new List<GraphElement>();
            foreach(object key in table.Keys)
            {
                graphElementList.Add(key as GraphElement);
            }
            moveSize = InitMoveSize(graphElementList, currentPoint);

            // 加入到绘图容器中
            foreach (object key in table.Keys)
            {
                if (key is SlotContainer) // 插槽容器
                {
                    SlotContainer slotContainer = key as SlotContainer;                    
                    slotContainer.Move(moveSize);
                    graphManager.SlotContainerList.Add(slotContainer);

                    // 分配新的图元ID
                    slotContainer.ID = graphManager.AllocateGraphElementID();

                    // 复制数据
                    dataManager.AddDataElement(slotContainer);
                    dataManager.SetData(slotContainer, table[key]);
                    dataManager.ReloadData(slotContainer);
                }
                else if (key is ConnectorContainer) // 连接线
                {
                    ConnectorContainer line = key as ConnectorContainer;                    

                    foreach (ConnectorGraphElement connector in line.GetConnectorList()) // 移动连接线控制点
                    {
                        if (!connector.Binded) // 连接线控制点不是连接线的头结点和尾结点
                        {
                            connector.Move(moveSize);
                        }
                    }

                    graphManager.ConnectorContainerList.Add(line);                                

                    // 分配新的图元ID
                    line.ID = graphManager.AllocateGraphElementID();

                    // 复制数据                    
                    dataManager.AddDataElement(line);
                    dataManager.SetData(line, table[key]);
                    dataManager.ReloadData(line);
                }
            }

            // 检查事件结点是否有效
            foreach (object key in table.Keys)
            {
                if (key is SlotContainer) // 插槽容器
                {
                    SlotContainer slotContainer = key as SlotContainer;
                    slotContainer.Enable = dataManager.IsConnectEventNode(slotContainer);
                }
                else if(key is ConnectorContainer) // 连接线
                {
                    ConnectorContainer line = key as ConnectorContainer;
                    line.Enable = dataManager.IsConnectEventNode(line);
                }
            }

            flowChartManager.PasteTable = table; 
            
            if(success) // 保存命令执行后的数据
            {
                flowChartManager.ContentChanged = true;
                SaveAfterExecute(flowChartManager.GetArguments());                
            }
            
            return success;
        }

        /// <summary>
        /// 计算图元移动的偏移
        /// </summary>
        /// <param name="graphElementList">图元链表</param>
        /// <param name="currentPoint">当前点所在位置</param>
        /// <returns>图元移动的偏移</returns>
        private Size InitMoveSize(List<GraphElement> graphElementList, Point currentPoint)
        {
            Size moveSize = Size.Empty;
            Point newPosition = Point.Empty;

            foreach (GraphElement graphElement in graphElementList)
            {
                if (moveSize == Size.Empty)
                {
                    moveSize.Width = currentPoint.X - graphElement.Location.X;
                    moveSize.Height = currentPoint.Y - graphElement.Location.Y;                    
                }

                newPosition = graphElement.Location + moveSize;

                // 定义图元粘贴后的位置的下限
                if (newPosition.X < 50)
                {
                    moveSize.Width = 50 - graphElement.Location.X;
                }

                if (newPosition.Y < 50)
                {
                    moveSize.Height = 50 - graphElement.Location.Y;
                }
            }

            return moveSize;
        }
    }
}
