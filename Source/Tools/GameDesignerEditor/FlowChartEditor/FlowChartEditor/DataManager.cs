using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;
using System.Windows.Forms;
using System.Reflection;
using System.IO;

using FlowChartEditor.BaseGraphElement;
using GraphElementLibrary;
using DataElementLibrary;

namespace FlowChartEditor
{
    public class DataManager
    {
        private Hashtable graphTable = new Hashtable(); // 图元哈希表
        private Hashtable compileTable = new Hashtable(); // 编译数据表
        private FlowChartManager flowChartManager; // 流程图管理器
        private Hashtable globeArgs = new Hashtable(); // 全局变量
        private DataElement initializedDataElement; // 初始化的数据元
        private string requestReloadID = ""; // 需要重新编辑的图元ID

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="flowChartManager">流程图管理器</param>
        public DataManager(FlowChartManager flowChartManager)
        {
            this.flowChartManager = flowChartManager;
        }

        /// <summary>
        /// 需要重新编辑的图元ID
        /// </summary>
        public string RequestReloadID
        {
            set
            {
                requestReloadID = value;
            }

            get
            {
                return requestReloadID;
            }
        }

        /// <summary>
        /// 初始化的数据元
        /// </summary>
        public DataElement InitializedDataElement
        {
            set
            {
                initializedDataElement = value;
            }
        }

        /// <summary>
        /// 全局变量
        /// </summary>
        public Hashtable GlobeArgs
        {
            get
            {
                return globeArgs;
            }
        }

        /// <summary>
        /// 逻辑图元哈希表
        /// </summary>
        public Hashtable LogicGraphTable
        {
            get
            {
                return graphTable;
            }
        }

        /// <summary>
        /// 当前流程图管理器
        /// </summary>
        public FlowChartManager CurrentFlowChartManager
        {
            set
            {
                flowChartManager = value;
            }

            get
            {
                return flowChartManager;
            }
        }

        /// <summary>
        /// 重新分配数据元的ID
        /// </summary>
        public void ResetDataElementID()
        {
            foreach (GraphElement graphElement in graphTable.Keys)
            {
                DataElement dataElement = graphTable[graphElement] as DataElement;
                dataElement.ID = graphElement.ID;
            }
        }

        /// <summary>
        /// 初始化编译数据表
        /// </summary>
        public void InitCompileTable()
        {
            compileTable = new Hashtable();

            foreach (GraphElement graphElement in graphTable.Keys)
            {
                compileTable[graphElement] = graphTable[graphElement];
            }
        }
        
        /// <summary>
        /// 添加编译数据
        /// </summary>
        /// <param name="graphElement">图元</param>
        /// <param name="dataElement">数据元</param>
        public void AddCompileData(GraphElement graphElement, DataElement dataElement)
        {
            compileTable[graphElement] = dataElement;
        }

        /// <summary>
        /// 获取编译数据元
        /// </summary>
        /// <param name="graphElement">索引的图元</param>
        /// <returns>获取的数据元</returns>
        public DataElement GetCompileDataElement(GraphElement graphElement)
        {
            DataElement dataElement = null;

            if (graphElement != null)
            {
                dataElement = compileTable[graphElement] as DataElement;
            }

            return dataElement;
        }

        /// <summary>
        /// 获取编译数据
        /// </summary>
        /// <param name="graphElement">索引的图元</param>
        /// <returns>获取的数据</returns>
        public object GetCompileData(GraphElement graphElement)
        {
            object data = null;
            DataElement dataElement = compileTable[graphElement] as DataElement;

            if (dataElement != null)
            {
                data = dataElement.Data;
            }

            return data;
        }

        /// <summary>
        /// 初始化数据元
        /// </summary>
        /// <param name="graphElement">索引的图元</param>
        /// <returns>数据元</returns>
        public DataElement InitDataElement(GraphElement graphElement)
        {
            // 执行逻辑操作            
            LogicBaseManager logicManager = Helper.GetHelper().GetLogicManager(flowChartManager.CurrentChartMode);
            logicManager.DoLogicOperation(flowChartManager, LogicType.InitializeDataElement, graphElement);

            return initializedDataElement;
        }

        /// <summary>
        /// 增加数据元
        /// </summary>
        /// <param name="graphElement">索引的图元</param>
        /// <returns>是否增加成功</returns>
        public bool AddDataElement(GraphElement graphElement)
        {
            bool success = false;

            DataElement dataElement = InitDataElement(graphElement);

            if (dataElement != null)
            {
                dataElement.ID = graphElement.ID;
                graphTable[graphElement] = dataElement;
                success = true;
            }            

            return success;
        }

        /// <summary>
        /// 获取数据元
        /// </summary>
        /// <param name="graphElement">索引的图元</param>
        /// <returns>获取的数据元</returns>
        public DataElement GetDataElement(GraphElement graphElement)
        {
            DataElement dataElement = null;

            if(graphElement != null)
            {
                dataElement = graphTable[graphElement] as DataElement;
            }            

            return dataElement;
        }

        /// <summary>
        /// 获取数据
        /// </summary>
        /// <param name="graphElement">索引的图元</param>
        /// <returns>获取的数据</returns>
        public object GetData(GraphElement graphElement)
        {            
            object data = null;
            DataElement dataElement = graphTable[graphElement] as DataElement;
            
            if (dataElement != null)
            {
                data = dataElement.Data;
            }

            return data;
        }

        /// <summary>
        /// 设置图元对应的数据
        /// </summary>
        /// <param name="graphElement">索引的图元</param>
        /// <param name="data">图元的数据</param>
        public void SetData(GraphElement graphElement, object data)
        {
            DataElement dataElement = graphTable[graphElement] as DataElement;

            if (dataElement != null)
            {
                dataElement.Text = graphElement.Text;
                dataElement.TooltipText = graphElement.TooltipText;
                dataElement.Data = data;
            }            
        }     
        
        /// <summary>
        /// 刷新图元的数据
        /// </summary>
        /// <param name="graphElement">索引的图元</param>
        public void ReloadData(GraphElement graphElement)
        {
            Hashtable table = new Hashtable();
            table["globe_args"] = globeArgs;
            DataElement dataElement = graphTable[graphElement] as DataElement;

            try
            {
                dataElement.ReloadData(table);
            }
            catch (Exception ex)
            {
                DocumentManager.GetDocumentManager().PrintText(string.Format("刷新ID为{0}的图元数据时产生异常：{1}",
                                                                             dataElement.ID, ex.Message));
            }
        }

        /// <summary>
        /// 获取指定ID的图元
        /// </summary>
        /// <param name="id">图元的id</param>
        /// <returns>图元</returns>
        public GraphElement FindGraphElementByID(int id)
        {
            GraphElement graphElement = null;

            foreach (GraphElement g in graphTable.Keys)
            {
                if (g.ID == id)
                {
                    graphElement = g;
                    break;
                }
            }

            return graphElement;
        }

        /// <summary>
        /// 获取指定ID的数据元
        /// </summary>
        /// <param name="id">数据元的id</param>
        /// <returns>数据元</returns>
        public DataElement FindDataElementByID(int id)
        {
            DataElement dataElement = null;

            foreach(DataElement dElement in graphTable.Values)
            {
                if (dElement.ID == id)
                {
                    dataElement = dElement;
                    break;
                }
            }

            return dataElement;
        }

        /// <summary>
        /// 编辑数据元
        /// </summary>
        /// <param name="graphElement">索引的图元</param>
        /// <returns>是否编辑成功</returns>
        public bool EditData(GraphElement graphElement)
        {
            bool success = false;

            DataElement dataElement = graphTable[graphElement] as DataElement;
            if(dataElement != null) // 找到该图元绑定的数据元
            {
                // 执行逻辑操作
                LogicBaseManager logicManager = Helper.GetHelper().GetLogicManager(flowChartManager.CurrentChartMode);
                success = logicManager.DoLogicOperation(flowChartManager, LogicType.Edit, graphElement);
            }
            else
            {
                MessageBox.Show("该图元没有绑定数据元！", "数据元编辑", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }            

            return success;
        }                   

        /// <summary>
        /// 删除数据元
        /// </summary>
        /// <param name="graphElement">索引的图元</param>
        /// <returns>是否删除成功</returns>
        public bool DeleteData(GraphElement graphElement)
        {
            if (graphElement != null)
            {
                graphTable.Remove(graphElement);
            }

            return true;
        }

        /// <summary>
        /// 获取流程图的数据参数
        /// </summary>
        /// <returns>流程图的数据参数哈希表</returns>
        public Hashtable GetArguments()
        {
            Hashtable args = new Hashtable();
            args["graphTable"] = graphTable;

            return args;
        }

        /// <summary>
        /// 保存前保证绘图的有效性
        /// </summary>
        public void ValidateBeforeSave()
        {
            List<GraphElement> deleteGraphElementList = new List<GraphElement>();
            List<GraphElement> validGraphElementList = flowChartManager.CurrentGraphManager.CurrentCanvas.GraphElementList;

            foreach (GraphElement graphElement in graphTable.Keys)
            {
                if (!validGraphElementList.Contains(graphElement))
                {
                    deleteGraphElementList.Add(graphElement);
                }
            }

            foreach (GraphElement graphElement in deleteGraphElementList)
            {
                DeleteData(graphElement);
            }
        }

        /// <summary>
        /// 设置流程图的数据参数
        /// </summary>
        /// <param name="args">流程图的数据参数哈希表</param>
        public void SetArguments(Hashtable args)
        {
            DocumentManager documentManager = DocumentManager.GetDocumentManager();
            graphTable = args["graphTable"] as Hashtable;
           
            Hashtable table = new Hashtable();
            table["globe_args"] = globeArgs;
            table["requestReloadID"] = requestReloadID;
            table["forceReload"] = "0";

            foreach (GraphElement graphElement in graphTable.Keys)
            {
                DataElement dataElement = graphTable[graphElement] as DataElement;

                if (dataElement != null)
                {
                    dataElement.ReloadData(table);
                    graphElement.TooltipText = dataElement.TooltipText;
                         
                    if (graphElement.Text != dataElement.Text && !string.IsNullOrEmpty(dataElement.Text))
                    {
                        graphElement.Text = dataElement.Text;

                        // 调整文本
                        if (graphElement is SlotContainer)
                        {
                            (graphElement as SlotContainer).AdjustText();
                        }
                        else if (graphElement is ConnectorContainer)
                        {
                            (graphElement as ConnectorContainer).AdjustText();
                        }
                    }
                }
            }

            requestReloadID = table["requestReloadID"] as string;
        }        

        /// <summary>
        /// 获取连入的图元的数据
        /// </summary>
        /// <param name="graphElement">当前图元</param>
        /// <returns>连入的图元的数据</returns>
        public Hashtable GetPreviousData(GraphElement graphElement)
        {
            Hashtable table = new Hashtable();

            foreach (GraphElement g in GetPreviousGraphElements(graphElement))
            {
                table[g.ID.ToString()] = GetDataElement(g);
            }

            return table;
        }

        /// <summary>
        /// 获取兄弟连接线的数据
        /// </summary>
        /// <param name="graphElement">当前图元</param>
        /// <returns>兄弟连接线的数据</returns>
        public Hashtable GetNeighborData(GraphElement graphElement)
        {
            Hashtable table = new Hashtable();

            if(graphElement is ConnectorContainer)
            {
                ConnectorContainer line = graphElement as ConnectorContainer;
                if(line.InSlotContainer != null)
                {
                    List<GraphElement> list = GetNextGraphElements(line.InSlotContainer, false);
                    foreach (GraphElement g in list)
                    {
                        if(g != graphElement)
                        {
                            table[g.ID.ToString()] = GetDataElement(g);
                        }
                    }
                }
            }

            return table;
        }

        /// <summary>
        /// 获取连出的图元的数据
        /// </summary>
        /// <param name="graphElement">当前图元</param>
        /// <returns>连出的图元的数据</returns>
        public Hashtable GetNextData(GraphElement graphElement)
        {
            Hashtable table = new Hashtable();

            foreach(GraphElement g in GetNextGraphElements(graphElement, false))
            {
                table[g.ID.ToString()] = GetDataElement(g);
            }

            return table;
        }

        /// <summary>
        /// 获取连入的图元链表
        /// </summary>
        /// <param name="graphElement">当前图元</param>
        /// <returns>连入的图元链表</returns>
        public List<GraphElement> GetPreviousGraphElements(GraphElement graphElement)
        {
            List<GraphElement> list = new List<GraphElement>();

            if (graphElement is SlotContainer) // 当前图元是插槽容器
            {
                SlotContainer slotContaienr = graphElement as SlotContainer;

                foreach (SlotGraphElement slot in slotContaienr.GetInSlotList())
                {
                    if (slot.Binded)
                    {
                        list.Add(slot.BindingConnector.Line);
                    }
                }
            }
            else if (graphElement is ConnectorContainer) // 当前图元是连接线控制点容器
            {                
                ConnectorContainer line = graphElement as ConnectorContainer;

                if (line.InSlotContainer != null)
                {                    
                    list.Add(line.InSlotContainer);                   
                }
            }

            return list;
        }

        /// <summary>
        /// 检查两个图元是否连接
        /// </summary>
        /// <param name="fromGraphElement">起始图元</param>
        /// <param name="toGraphElement">终结图元</param>
        /// <returns>两个图元是否连接</returns>
        public bool IsConnect(GraphElement fromGraphElement, GraphElement toGraphElement)
        {
            List<GraphElement> list = new List<GraphElement>(); // 记录当前遍历过的图元，避免死循环
            IsConnect(fromGraphElement, toGraphElement, list);
            bool result = list.Contains(toGraphElement);

            return result;
        }

        /// <summary>
        /// 检查两个图元是否连接的辅助函数
        /// </summary>
        /// <param name="fromGraphElement">起始图元</param>
        /// <param name="toGraphElement">终结图元</param>
        /// <param name="list">当前遍历过的图元链表</param>
        private void IsConnect(GraphElement fromGraphElement, GraphElement toGraphElement, List<GraphElement> list)
        {
            if (fromGraphElement == toGraphElement)
            {
                list.Add(toGraphElement);
            }
            else
            {
                if (!list.Contains(fromGraphElement))
                {
                    list.Add(fromGraphElement);

                    foreach (GraphElement graphElement in GetNextGraphElements(fromGraphElement, false))
                    {
                        IsConnect(graphElement, toGraphElement, list);
                    }
                }
            }
        }

        /// <summary>
        /// 获取连出的图元链表
        /// </summary>
        /// <param name="graphElement">当前图元</param>
        /// <param name="hasInnerChart">是否处理子绘图</param>
        /// <returns>连出的图元链表</returns>
        public List<GraphElement> GetNextGraphElements(GraphElement graphElement, bool dealInnerChart)
        {
            List<GraphElement> list = new List<GraphElement>();

            if (graphElement is SlotContainer) // 当前图元是插槽容器
            {
                SlotContainer slotContainer = graphElement as SlotContainer;

                foreach (SlotGraphElement slot in slotContainer.GetOutSlotList())
                {
                    if (slot.Binded)
                    {
                        list.Add(slot.BindingConnector.Line);
                    }
                }
            }
            else if (graphElement is ConnectorContainer) // 当前图元是连接线控制点容器
            {
                ConnectorContainer line = graphElement as ConnectorContainer;

                if (line.OutSlotContainer != null)
                {
                    if (dealInnerChart)
                    {
                        SlotContainer slotContainer = line.OutSlotContainer;

                        if (slotContainer is InnerChart)
                        {
                            SlotGraphElement slot = slotContainer.GetBindedInSlot(line);
                            InnerChartEditor innerChartEditor = GetDataElement(slotContainer) as InnerChartEditor;

                            if (slot != null)
                            {
                                InterfaceGraphElement interfaceNode = innerChartEditor.GetInterfaceNode(slot.Tag.ToString());

                                if (interfaceNode != null)
                                {
                                    GraphElement connectedGraphElement = interfaceNode.GetConnectedOutGraphElement();

                                    if (connectedGraphElement != null)
                                    {
                                        list.Add(connectedGraphElement);
                                    }
                                }
                            }
                        }
                        else if (slotContainer is InterfaceGraphElement)
                        {
                            InnerChart innerChart = FindGraphElementByID(int.Parse(slotContainer.Tag as string)) as InnerChart;

                            foreach (SlotGraphElement slot in innerChart.GetOutSlotList())
                            {
                                if (slot.Binded)
                                {
                                    ConnectorContainer connectedLine = slot.BindingConnector.Line;
                                    string lineData = GetData(connectedLine) as string;

                                    if (!string.IsNullOrEmpty(lineData))
                                    {
                                        string[] dataArray = lineData.Split(new char[] { ':' }, StringSplitOptions.RemoveEmptyEntries);

                                        if (dataArray[0] == slotContainer.ID.ToString())
                                        {
                                            list.AddRange(GetNextGraphElements(slot.BindingConnector.Line, true));
                                            break;
                                        }
                                    }

                                    /*
                                    if (!string.IsNullOrEmpty(connectedLine.Text))
                                    {
                                        string[] dataArray = connectedLine.Text.Split(new char[] { ':' }, StringSplitOptions.RemoveEmptyEntries);

                                        if (dataArray[0] == slotContainer.ID.ToString())
                                        {
                                            list.AddRange(GetNextGraphElements(slot.BindingConnector.Line, true));
                                            break;
                                        }
                                    }
                                    */ 
                                }
                            }

                            /*
                            foreach (SlotGraphElement slot in innerChart.GetOutSlotList())
                            {
                                if (slot.Tag.ToString() == slotContainer.ID.ToString())
                                {
                                    slotGraphElement = slot;
                                    break;
                                }
                            }

                            if (slotGraphElement != null)
                            {
                                if (slotGraphElement.Binded)
                                {
                                    list.AddRange(GetNextGraphElements(slotGraphElement.BindingConnector.Line, true));
                                }
                            }
                            */
                        }
                        else
                        {
                            list.Add(slotContainer);
                        }
                    }
                    else
                    {
                        list.Add(line.OutSlotContainer);
                    }                    
                }
            }            

            return list;
        }

        /// <summary>
        /// 获取连出的所有插槽容器的链表
        /// </summary>
        /// <param name="slotContainer">当前插槽容器</param>
        /// <returns>连出的所有插槽容器的链表</returns>
        public List<SlotContainer> GetAllNextSlotContainers(SlotContainer slotContainer)
        {
            List<SlotContainer> list = new List<SlotContainer>();

            GetAllNextSlotContainers(slotContainer, list);

            return list;
        }

        /// <summary>
        /// 获取连入的所有插槽容器的链表
        /// </summary>
        /// <param name="slotContainer">当前插槽容器</param>
        /// <returns>连入的所有插槽容器的链表</returns>
        public List<SlotContainer> GetAllPreviousSlotContainers(SlotContainer slotContainer)
        {
            List<SlotContainer> list = new List<SlotContainer>();

            GetAllPreviousSlotContainers(slotContainer, list);

            return list;
        }

        /// <summary>
        /// 获取连出的所有插槽容器的链表（辅助函数）
        /// </summary>
        /// <param name="slotContainer">当前插槽容器</param>
        /// <param name="list">当前搜索的所有插槽容器的链表</param>
        public void GetAllNextSlotContainers(SlotContainer slotContainer, List<SlotContainer> list)
        {
            if(!list.Contains(slotContainer)) // 避免出现死循环
            {
                list.Add(slotContainer);

                foreach (SlotGraphElement slot in slotContainer.GetOutSlotList())
                {
                    if (slot.Binded && slot.BindingConnector.Line.OutSlotContainer != null)
                    {
                        GetAllNextSlotContainers(slot.BindingConnector.Line.OutSlotContainer, list);
                    }
                }
            }
        }

        /// <summary>
        /// 获取连入的所有插槽容器的链表（辅助函数）
        /// </summary>
        /// <param name="slotContainer">当前插槽容器</param>
        /// <param name="list">当前搜索的所有插槽容器的练表</param>
        public void GetAllPreviousSlotContainers(SlotContainer slotContainer, List<SlotContainer> list)
        {
            if(!list.Contains(slotContainer)) // 避免出现死循环
            {
                list.Add(slotContainer);

                foreach(SlotGraphElement slot in slotContainer.GetInSlotList())
                {
                    if(slot.Binded && slot.BindingConnector.Line.InSlotContainer != null)
                    {
                        GetAllPreviousSlotContainers(slot.BindingConnector.Line.InSlotContainer, list);
                    }
                }
            }
        }

        /// <summary>
        /// 获取连出的插槽容器链表
        /// </summary>
        /// <param name="slotContainer">当前插槽容器</param>
        /// <returns>连出的插槽容器链表</returns>
        public List<SlotContainer> GetNextSlotContainers(SlotContainer slotContainer)
        {
            List<SlotContainer> list = new List<SlotContainer>();

            foreach(SlotGraphElement slot in slotContainer.GetOutSlotList())
            {
                if(slot.Binded && slot.BindingConnector.Line.OutSlotContainer != null
                    && !list.Contains(slot.BindingConnector.Line.OutSlotContainer))
                {
                    list.Add(slot.BindingConnector.Line.OutSlotContainer);
                }
            }

            return list;
        }

        /// <summary>
        /// 获取插槽容器对应的事件结点的数据
        /// </summary>
        /// <param name="slotContainer">当前插槽容器</param>
        /// <returns>事件结点的数据</returns>
        public object GetEventData(SlotContainer slotContainer)
        {
            object data = null;

            List<DataElement> eventList = GetEventList(slotContainer);
            if(eventList.Count > 0)
            {
                data = eventList[0].Data;
            }

            return data;
        }

        /// <summary>
        /// 获取插槽容器对应的事件结点数据链表
        /// </summary>
        /// <param name="slotContainer">当前插槽容器</param>
        /// <returns>插槽容器对应的事件结点数据链表</returns>
        public List<DataElement> GetEventList(SlotContainer slotContainer)
        {
            List<DataElement> list = new List<DataElement>();
            List<SlotContainer> slotContainerList = GetAllPreviousSlotContainers(slotContainer);
            foreach(SlotContainer s in slotContainerList)
            {
                if(s is EventGraphElement)
                {
                    DataElement dataElement = graphTable[s] as DataElement;
                    if(dataElement != null)
                    {
                        list.Add(dataElement);
                    }
                }
            }

            return list;
        }

        /// <summary>
        /// 获取连接线对应的事件结点数据链表
        /// </summary>
        /// <param name="connectorContainer">当前连接线</param>
        /// <returns>连接线对应的事件结点数据链表</returns>
        public List<DataElement> GetEventList(ConnectorContainer line)
        {
            List<DataElement> list = new List<DataElement>();
            SlotContainer slotContainer = line.InSlotContainer;

            if(slotContainer != null)
            {
                List<SlotContainer> slotContainerList = GetAllPreviousSlotContainers(slotContainer);
                foreach (SlotContainer s in slotContainerList)
                {
                    if (s is EventGraphElement)
                    {
                        DataElement dataElement = graphTable[s] as DataElement;
                        if (dataElement != null)
                        {
                            list.Add(dataElement);
                        }
                    }
                }
            }

            return list;
        }

        /// <summary>
        /// 检查是否连接有事件结点
        /// </summary>
        /// <param name="slotContainer">当前插槽容器</param>
        /// <returns>是否连接有事件结点</returns>
        public bool IsConnectEventNode(SlotContainer slotContainer)
        {
            bool result = false;
            
            if (slotContainer.RequireEvent) // 需要入口事件
            {
                if (GetEventList(slotContainer).Count > 0)
                {
                    result = true;
                }
            }
            else
            {
                result = true;
            }                                    

            return result;
        }

        /// <summary>
        /// 检查是否连接有事件结点
        /// </summary>
        /// <param name="line">当前连接线</param>
        /// <returns>是否连接有事件结点</returns>
        public bool IsConnectEventNode(ConnectorContainer line)
        {
            bool result = false;

            if (line.RequireEvent) // 需要入口事件
            {
                if (GetEventList(line).Count > 0)
                {
                    result = true;
                }
            }
            else
            {
                result = true;
            }            

            return result;
        }
    }
}
