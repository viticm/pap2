using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;
using System.Windows.Forms;
using GraphElementLibrary;

namespace FlowChartEditor
{
    class FlowChartCommand:Command
    {
        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="data">当前对象</param>
        /// <param name="description">命令的描述</param>
        public FlowChartCommand(object data, string description):
            base(data, description)
        {
            dataBeforeExecute = new SerialMemento();
            dataAfterExecute = new SerialMemento();
        }

        /// <summary>
        /// 删除图元
        /// </summary>
        /// <param name="graphElement">要删除的图元</param>
        /// <return>是否删除成功</return>
        protected bool DeleteGraphElement(GraphElement graphElement)
        {
            GraphManager graphManager = data as GraphManager;
            DataManager dataManager = graphManager.CurrentFlowChartManager.CurrentDataManager;
            List<GraphElement> list = new List<GraphElement>(); // 记录遍历过图元的链表
            bool deleteSuccess = false; // 是否删除成功

            if (graphElement is SlotContainer) // 要删除的图元是插槽容器
            {
                SlotContainer slotContainer = graphElement as SlotContainer;
                ReBindEventNode(list, slotContainer, null, false); // 重新设定事件结点及图元是否可用

                foreach (SlotGraphElement slot in slotContainer.GetSlotList())
                {
                    if (slot.Binded) // 解除绑定连接线控制点
                    {
                        if (slot.IsOutSlot && slotContainer is ConditionGraphElement) // 对条件结点连出的线要进行有效性检查
                        {
                            ConnectorContainer line = slot.BindingConnector.Line;
                            if (line.Text == "×")
                            {
                                line.Invalid = false;
                                line.ShowText = false;
                            }
                        }
                        slot.UnBind();
                    }
                }

                graphManager.SlotContainerList.Remove(slotContainer); // 从插槽容器索引链表中删除
                deleteSuccess = true;
            }
            else if (graphElement is ConnectorContainer) // 要删除的图元是连接线控制点容器
            {
                ConnectorContainer connectorContainer = graphElement as ConnectorContainer;

                SlotContainer inSlotContainer = connectorContainer.InSlotContainer;
                if (inSlotContainer != null)
                {
                    connectorContainer.UnbindInSlotContainer();
                }

                ReBindEventNode(list, connectorContainer, null, false); // 重新设定事件结点及图元是否可用

                SlotContainer outSlotContainer = connectorContainer.OutSlotContainer;
                if (outSlotContainer != null)
                {
                    connectorContainer.UnbindOutSlotContainer();
                }

                if (inSlotContainer is ConditionGraphElement) // 对条件结点连出的线要进行有效性检查
                {
                    dataManager.CheckLineValid(inSlotContainer);
                }

                graphManager.ConnectorContainerList.Remove(connectorContainer); // 从连接线控制点索引容器中删除
                deleteSuccess = true;
            }
            else if (graphElement is ConnectorGraphElement) // 要删除的图元是连接线控制点
            {
                ConnectorGraphElement connector = graphElement as ConnectorGraphElement;
                if (connector.IsHeadPoint || connector.IsTailPoint) // 不允许删除头结点和尾结点
                {
                    MessageBox.Show("不能删除头连接线控制点和尾连接线控制点！", "图元删除", MessageBoxButtons.OK, MessageBoxIcon.Information);

                }
                else
                {
                    connector.Line.RemoveConnector(connector);
                    deleteSuccess = true;
                }
            }
            else if (graphElement is SlotGraphElement) // 要删除的图元是插槽
            {
                SlotGraphElement slot = graphElement as SlotGraphElement;

                if (slot.CanDelete && (slot.IsInSlot || slot.SlotContainer.OutSlotCount > 1)) // 插槽可以删除
                {
                    SlotContainer slotContainer = slot.SlotContainer;
                    slotContainer.RemoveSlot(slot);                    
                    deleteSuccess = true;
                }
                else
                {
                    MessageBox.Show("该插槽不能删除", "图元删除", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
            }
            else if (graphElement is RemarkGraphElement) // 要删除的图元是注释结点
            {
                RemarkGraphElement remarkGraphElement = graphElement as RemarkGraphElement;
                SlotContainer slotContainer = remarkGraphElement.Owner as SlotContainer;

                slotContainer.RemarkNode = null;
                remarkGraphElement.Owner = null;
                remarkGraphElement = null;
                deleteSuccess = true;
            }

            if (deleteSuccess) // 释放图元所占的资源
            {
                dataManager.DeleteData(graphElement);
                graphElement.Release();
            }

            return deleteSuccess;
        }

        /// <summary>
        /// 复制图元及其信息
        /// </summary>
        /// <param name="dataTable">保存图元信息的哈希表</param>
        /// <returns>复制的数据</returns>
        protected Hashtable CopyGraphElement(Hashtable dataTable)
        {
            GraphManager graphManager = data as GraphManager;
            FlowChartManager flowChartManager = graphManager.CurrentFlowChartManager;
            DataManager dataManager = flowChartManager.CurrentDataManager;
            Hashtable copyTable = new Hashtable();
            Hashtable graphTable = new Hashtable();
            List<ConnectorContainer> lineList = new List<ConnectorContainer>();
            List<GraphElement> graphElementList = new List<GraphElement>();

            if (dataTable.Keys.Count > 1) // 复制多个图元
            {
                SlotContainer slotContainer;
                SlotContainer newSlotContainer;
                ConnectorContainer line;
                ConnectorContainer newLine;
                SlotGraphElement slot;
                ICloneable cloneData;
                EventGraphElement newEventNode;
                List<EventGraphElement> eventList = new List<EventGraphElement>();

                foreach (object o in dataTable.Keys)
                {
                    GraphElement graphElement = o as GraphElement;

                    if (graphElement is SlotContainer) // 插槽容器
                    {
                        slotContainer = graphElement as SlotContainer;
                        newSlotContainer = slotContainer.Clone();
                        graphTable[slotContainer] = newSlotContainer;
                        cloneData = dataTable[slotContainer] as ICloneable;

                        if (slotContainer is EventGraphElement)
                        {
                            eventList.Add(slotContainer as EventGraphElement);
                        }

                        if (cloneData != null)
                        {
                            copyTable[newSlotContainer] = cloneData.Clone();
                        }
                        else
                        {
                            copyTable[newSlotContainer] = null;
                        }

                        graphElementList.Add(slotContainer);
                    }
                    else if (graphElement is ConnectorContainer) // 连接线
                    {
                        line = graphElement as ConnectorContainer;
                        newLine = line.Clone();
                        graphTable[line] = newLine;
                        lineList.Add(line);
                        cloneData = dataTable[line] as ICloneable;

                        if (cloneData != null)
                        {
                            copyTable[newLine] = cloneData.Clone();
                        }
                        else
                        {
                            copyTable[newLine] = null;
                        }

                        graphElementList.Add(line);
                    }
                }

                // 重新构建复制图元的连接关系
                foreach (ConnectorContainer connectorContainer in lineList)
                {
                    if (connectorContainer.InSlotContainer != null
                        && graphElementList.Contains(connectorContainer.InSlotContainer))
                    {
                        newSlotContainer = graphTable[connectorContainer.InSlotContainer] as SlotContainer;
                        newLine = graphTable[connectorContainer] as ConnectorContainer;

                        slot = newSlotContainer.GetOutSlot();
                        slot.Bind(newLine.TailConnector);
                    }

                    if (connectorContainer.OutSlotContainer != null
                        && graphElementList.Contains(connectorContainer.OutSlotContainer))
                    {
                        newSlotContainer = graphTable[connectorContainer.OutSlotContainer] as SlotContainer;
                        newLine = graphTable[connectorContainer] as ConnectorContainer;

                        slot = newSlotContainer.GetInSlot();
                        slot.Bind(newLine.HeadConnector);
                    }
                }

                // 重设图元对应的事件结点
                if (eventList.Count > 0)
                {
                    foreach (object o in dataTable.Keys)
                    {
                        GraphElement graphElement = o as GraphElement;

                        if (graphElement is SlotContainer) // 插槽容器
                        {
                            slotContainer = graphElement as SlotContainer;
                            if (slotContainer.EventNode != null)
                            {
                                newSlotContainer = graphTable[slotContainer] as SlotContainer;

                                if (eventList.Contains(slotContainer.EventNode))
                                {
                                    newEventNode = graphTable[slotContainer.EventNode] as EventGraphElement;
                                    newSlotContainer.EventNode = newEventNode;
                                }
                                else
                                {
                                    newSlotContainer.EventNode = slotContainer.EventNode;
                                }
                            }
                        }
                        else if (graphElement is ConnectorContainer) // 连接线
                        {
                            line = graphElement as ConnectorContainer;
                            if (line.EventNode != null)
                            {
                                newLine = graphTable[line] as ConnectorContainer;

                                if (eventList.Contains(line.EventNode))
                                {
                                    newEventNode = graphTable[line.EventNode] as EventGraphElement;
                                    newLine.EventNode = newEventNode;
                                }
                                else
                                {
                                    newLine.EventNode = line.EventNode;
                                }
                            }
                        }
                    }
                }
            }
            else // 复制单个图元
            {
                GraphElement graphElement = null;
                foreach(object o in dataTable.Keys)
                {
                    graphElement = o as GraphElement;
                }               
                ICloneable cloneData;

                if (graphElement is SlotContainer) // 插槽容器
                {
                    SlotContainer slotContainer = graphElement as SlotContainer;
                    SlotContainer newSlotContainer = slotContainer.Clone();
                    cloneData = dataTable[slotContainer] as ICloneable;

                    if (cloneData != null)
                    {
                        copyTable[newSlotContainer] = cloneData.Clone();
                    }
                    else
                    {
                        copyTable[newSlotContainer] = null;
                    }
                }
                else if (graphElement is ConnectorContainer) // 连接线
                {
                    ConnectorContainer line = graphElement as ConnectorContainer;
                    ConnectorContainer newLine = line.Clone();
                    cloneData = dataTable[line] as ICloneable;

                    if (cloneData != null)
                    {
                        copyTable[newLine] = cloneData.Clone();
                    }
                    else
                    {
                        copyTable[newLine] = null;
                    }
                }
            }

            return copyTable;
        }

        /// <summary>
        /// 检查是否更新事件结点
        /// </summary>
        /// <param name="graphElement">当前图元</param>
        /// <param name="eventNode">事件结点</param>
        /// <returns>是否需要更新</returns>
        protected virtual bool CheckCanBindEventNode(GraphElement graphElement, EventGraphElement eventNode)
        {
            bool avail = true;
            GraphManager graphManager = data as GraphManager;
            DataManager dataManager = graphManager.CurrentFlowChartManager.CurrentDataManager;
            IComparable com1;
            IComparable com2;

            if(graphElement is SlotContainer) // 插槽容器
            {
                SlotContainer slotContainer = graphElement as SlotContainer;
                if(slotContainer.EventNode != null && slotContainer.EventNode != eventNode)
                {
                    com1 = dataManager.GetData(slotContainer.EventNode) as IComparable;
                    com2 = dataManager.GetData(eventNode) as IComparable;
                    avail = (com1.CompareTo(com2) == 0);
                }
            }
            else if(graphElement is ConnectorContainer) // 连接线
            {
                ConnectorContainer line = graphElement as ConnectorContainer;
                if(line.EventNode != null && line.EventNode != eventNode)
                {
                    com1 = dataManager.GetData(line.EventNode) as IComparable;
                    com2 = dataManager.GetData(eventNode) as IComparable;
                    avail = (com1.CompareTo(com2) == 0);
                }
            }

            return avail;
        }

        /// <summary>
        /// 设置图元是否可用
        /// </summary>
        /// <param name="list">遍历过的图元链表</param>
        /// <param name="graphElement">当前图元</param>
        /// <param name="eventNode">事件结点</param>
        /// <param name="enable">图元是否可用</param>
        protected virtual void ReBindEventNode(List<GraphElement> list, GraphElement graphElement, EventGraphElement eventNode, bool enable)
        {
            if(!list.Contains(graphElement)) // 还没有遍历过当前图元
            {
                GraphManager graphManager = data as GraphManager;
                DataManager dataManager = graphManager.CurrentFlowChartManager.CurrentDataManager;               

                if(graphElement is SlotContainer) // 该图元是插槽容器
                {
                    SlotContainer slotContainer = graphElement as SlotContainer;

                    if(!enable && slotContainer.EventNode != null && dataManager.IsConnect(slotContainer.EventNode, slotContainer)) // 不需要更新当前图元的事件结点
                    {

                    }
                    else
                    {
                        list.Add(slotContainer);
                        slotContainer.Enable = enable;
                        slotContainer.EventNode = eventNode;

                        foreach (GraphElement g in dataManager.GetNextGraphElements(graphElement))
                        {
                            ReBindEventNode(list, g, eventNode, enable);
                        }
                    }
                }
                else if(graphElement is ConnectorContainer) // 该图元是连接线控制点容器
                {
                    ConnectorContainer line = graphElement as ConnectorContainer;

                    if(!enable && line.EventNode != null && dataManager.IsConnect(line.EventNode, line)) // 不需要更新当前图元的事件结点
                    {                            
                        
                    } 
                    else
                    {
                        list.Add(line);
                        line.Enable = enable;
                        line.EventNode = eventNode;

                        foreach (GraphElement g in dataManager.GetNextGraphElements(graphElement))
                        {
                            ReBindEventNode(list, g, eventNode, enable);
                        }
                    }
                }                
            }
        }

        /// <summary>
        /// 在命令执行前保存对象数据
        /// </summary>
        /// <param name="o">当前对象</param>
        protected override void SaveBeforeExecute(object o)
        {
            dataBeforeExecute.SaveData(o);
        }

        /// <summary>
        /// 在命令执行后保存对象数据
        /// </summary>
        /// <param name="o">当前对象</param>
        protected override void SaveAfterExecute(object o)
        {
            dataAfterExecute.SaveData(o);
        }

        /// <summary>
        /// 执行命令
        /// </summary>
        /// <param name="o">当前对象</param>
        /// <returns>是否执行成功</returns>
        public override bool Execute(object o)
        {
            return false;
        }

        /// <summary>
        /// 执行命令
        /// </summary>
        public override void Execute()
        {
            object o = dataAfterExecute.LoadData();
            GraphManager graphManager = data as GraphManager;
            FlowChartManager flowChartManager = graphManager.CurrentFlowChartManager;
            object[] args = o as object[];
            flowChartManager.SetArguments(args);
        }

        /// <summary>
        /// 撤销命令
        /// </summary>
        public override void Unexecute()
        {
            object o = dataBeforeExecute.LoadData();
            GraphManager graphManager = data as GraphManager;
            FlowChartManager flowChartManager = graphManager.CurrentFlowChartManager;
            object[] args = o as object[];
            flowChartManager.SetArguments(args);
        }
    }
}
