using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Drawing;
using System.IO;
using System.Data;
using System.Collections;

using DevComponents.DotNetBar;
using DevComponents.DotNetBar.Controls;

using FlowChartEditor;
using Compiler_Sword3;
using DataInteractor;
using FlowChartEditor.BaseGraphElement;
using GraphElementLibrary;
using DataElementLibrary;
using DBManager;
using FlowChartDataLibrary;

namespace FlowChartLogicManager
{
    public class LogicManager:LogicBaseManager
    {
        /// <summary>
        /// 构造函数
        /// </summary>
        public LogicManager()
        {
            
        }

        /// <summary>
        /// 初始化绘图参数
        /// </summary>
        protected override void InitGraphSetting()
        {
            GraphSetting graphSetting = GraphSetting.GetGraphSetting();
            graphSetting.GraphElementTextColor = Color.Yellow;
        }

        /// <summary>
        /// 连接图元
        /// </summary>
        /// <param name="flowChartManager">绘图管理器</param>
        /// <param name="logicData">逻辑数据</param>
        /// <returns>是否操作成功</returns>
        protected override bool LogicConnect(FlowChartManager flowChartManager, object logicData)
        {
            bool executeSuccess = true;
            DataManager dataManager = flowChartManager.CurrentDataManager;

            object[] data = logicData as object[];
            SlotGraphElement slot = data[0] as SlotGraphElement;
            ConnectorGraphElement connector = data[1] as ConnectorGraphElement;
            bool enableConnect = true;
            List<GraphElement> list = new List<GraphElement>(); // 记录遍历过的图元的链表

            if (connector.IsHeadPoint) // 连入插槽容器
            {
                SlotContainer inSlotContainer = connector.Line.InSlotContainer;

                if (inSlotContainer != null)
                {
                    List<DataElement> eventList = dataManager.GetEventList(connector.Line.OutSlotContainer);

                    // 检查事件结点互斥
                    enableConnect = LogicCheck.CheckEventExclusion(eventList);

                    if (enableConnect) // 允许连接事件结点
                    {
                        bool enable = dataManager.IsConnectEventNode(inSlotContainer);
                        ReBindEventNode(dataManager, list, slot.SlotContainer, enable); // 重新绑定当前图元与其连出图元的事件结点
                    }
                    else
                    {
                        slot.UnBind();
                        executeSuccess = false;
                    }
                }
            }
            else // 连出插槽容器
            {
                SlotContainer outSlotContainer = connector.Line.OutSlotContainer;

                if (outSlotContainer != null)
                {
                    List<DataElement> eventList = dataManager.GetEventList(outSlotContainer);

                    if (enableConnect) // 允许连接事件结点
                    {
                        bool enable = dataManager.IsConnectEventNode(slot.SlotContainer);
                        ReBindEventNode(dataManager, list, connector.Line, enable); // 重新绑定当前图元与其连出图元的事件结点 
                    }
                    else
                    {
                        slot.UnBind();
                        executeSuccess = false;
                    }
                }
            }

            return executeSuccess;
        }
        
        /// <summary>
        /// 解除连接图元
        /// </summary>
        /// <param name="flowChartManager">绘图管理器</param>
        /// <param name="logicData">逻辑数据</param>
        /// <returns>是否操作成功</returns>
        protected override bool LogicDisconnect(FlowChartManager flowChartManager, object logicData)
        {
            DataManager dataManager = flowChartManager.CurrentDataManager;

            object[] args = logicData as object[];
            SlotGraphElement slot = args[0] as SlotGraphElement;
            ConnectorGraphElement connector = args[1] as ConnectorGraphElement;
            ConnectorContainer line = connector.Line;
            SlotContainer slotContainer = slot.SlotContainer;

            List<GraphElement> list = new List<GraphElement>(); // 记录遍历的图元的链表

            if (connector.IsHeadPoint) // 移出连接线的头结点
            {
                ReBindEventNode(dataManager, list, slotContainer, false); // 重新绑定当前图元与其连出图元的事件结点
            }
            else // 移出连接线的尾结点
            {
                SlotContainer outSlotContainer = connector.Line.OutSlotContainer;

                if (outSlotContainer != null)
                {
                    ReBindEventNode(dataManager, list, connector.Line, false); // 重新绑定当前图元与其连出图元的事件结点
                }
            }

            return true;
        }

        /// <summary>
        /// 删除图元之后的操作
        /// </summary>
        /// <param name="flowChartManager">绘图管理器</param>
        /// <param name="logicData">逻辑数据</param>
        /// <returns>是否操作成功</returns>
        protected override bool LogicAfterDelete(FlowChartManager flowChartManager, object logicData)
        {
            DataManager dataManager = flowChartManager.CurrentDataManager;
            List<GraphElement> graphElementList = logicData as List<GraphElement>;

            foreach (GraphElement graphElement in graphElementList)
            {
                List<GraphElement> list = new List<GraphElement>(); // 记录遍历过图元的链表
                ReBindEventNode(dataManager, list, graphElement, false); // 重新设定事件结点及图元是否可用

                ConnectorContainer line = graphElement as ConnectorContainer;

                if (line != null)
                {
                    // 连接线恢复可见状态
                    line.Moving = false;
                    line.Visible = true;
                }
            }

            return true;
        }

        /// <summary>
        /// 编辑图元
        /// </summary>
        /// <param name="flowChartManager">绘图管理器</param>
        /// <param name="logicData">逻辑数据</param>
        /// <returns>是否操作成功</returns>
        protected override bool LogicEdit(FlowChartManager flowChartManager, object logicData)
        {
            bool executeResult = true;

            GraphManager graphManager = flowChartManager.CurrentGraphManager;
            DataManager dataManager = flowChartManager.CurrentDataManager;
            DocumentManager documentManager = DocumentManager.GetDocumentManager();
            GraphElement graphElement = logicData as GraphElement;
            DataElement dataElement = dataManager.GetDataElement(graphElement);

            if (CheckGraphElementEditable(graphElement)) // 检查图元是否能够编辑
            {
                object data = dataManager.GetData(graphElement);
                Hashtable information = new Hashtable();

                information["data"] = data;
                information["prev_data"] = dataManager.GetPreviousData(graphElement);
                information["next_data"] = dataManager.GetNextData(graphElement);
                information["neighbor_data"] = dataManager.GetNeighborData(graphElement);
                information["globe_args"] = dataManager.GlobeArgs;
                information["flowchart_name"] = flowChartManager.Name;
                information["map_name"] = flowChartManager.MapName;
                information["client_dir"] = Helper.GetHelper().OutputDir;

                if (graphElement is SlotContainer)
                {
                    SlotContainer slotContainer = graphElement as SlotContainer;

                    // 刷新当前事件结点
                    information["event_data"] = dataManager.GetEventData(slotContainer);
                }

                dataElement.PrintInformation = new DataElement.PrintInfo(documentManager.PrintText);

                try
                {
                    executeResult = dataElement.EditData(information);
                }
                catch (Exception ex)
                {
                    MessageBox.Show("当前图元由于以下原因无法编辑：\n\n" + ex.Message, "图元编辑",
                        MessageBoxButtons.OK, MessageBoxIcon.Information);
                }

                if (executeResult) // 保存图元数据
                {
                    Hashtable previousDataTable = information["prev_data"] as Hashtable;
                    Hashtable nextDataTable = information["next_data"] as Hashtable;
                    GraphElement currentGraphElement;
                    DataElement editDataElement;

                    // 检查是否需要更新图元和数据元的数据
                    foreach (string id in previousDataTable.Keys)
                    {
                        editDataElement = previousDataTable[id] as DataElement;

                        if (editDataElement != null && editDataElement.Data == null)
                        {
                            currentGraphElement = dataManager.FindGraphElementByID(int.Parse(id));
                            currentGraphElement.Text = editDataElement.Text;
                            currentGraphElement.TooltipText = editDataElement.TooltipText;
                            currentGraphElement.ShowText = false;
                        }
                    }

                    foreach (string id in nextDataTable.Keys)
                    {
                        editDataElement = nextDataTable[id] as DataElement;

                        if (editDataElement != null && editDataElement.Data == null)
                        {
                            currentGraphElement = dataManager.FindGraphElementByID(int.Parse(id));
                            currentGraphElement.Text = editDataElement.Text;
                            currentGraphElement.TooltipText = editDataElement.TooltipText;
                            currentGraphElement.ShowText = false;
                        }
                    }

                    graphElement.TooltipText = dataElement.TooltipText;
                    graphElement.Text = dataElement.Text;
                    if (dataElement.Text == "")
                    {
                        graphElement.ShowText = false;
                    }
                    else
                    {
                        graphElement.ShowText = true;
                    }

                    // 调整文本
                    if (graphElement is SlotContainer)
                    {
                        SlotContainer slotContainer = graphElement as SlotContainer;
                        slotContainer.AdjustText();

                        // 根据文本内容调整插槽容器的大小
                        slotContainer.AdjustElementSize();
                    }
                    else if (graphElement is ConnectorContainer)
                    {
                        ConnectorContainer line = graphElement as ConnectorContainer;
                        line.AdjustText();
                    }
                }
            }

            return executeResult;
        }

        /// <summary>
        /// 创建连接线
        /// </summary>
        /// <param name="flowChartManager">绘图管理器</param>
        /// <param name="logicData">逻辑数据</param>
        /// <returns>是否操作成功</returns>
        protected override bool LogicCreateLine(FlowChartManager flowChartManager, object logicData)
        {
            GraphManager graphManager = flowChartManager.CurrentGraphManager;
            DataManager dataManager = flowChartManager.CurrentDataManager;

            object[] args = logicData as object[];
            int id = graphManager.AllocateGraphElementID();
            SlotGraphElement slot = args[0] as SlotGraphElement;
            Point p = (Point)args[1];

            // 创建连接线
            base.LogicCreateLine(flowChartManager, logicData);

            // 绑定连接线的事件
            List<GraphElement> list = new List<GraphElement>();
            bool enable = dataManager.IsConnectEventNode(slot.SlotContainer);
            ReBindEventNode(dataManager, list, slot.SlotContainer, enable); 

            return true;
        }

        /// <summary>
        /// 在指定点创建结点
        /// </summary>
        /// <param name="flowChartManager">绘图管理器</param>
        /// <param name="logicData">逻辑数据</param>
        /// <returns>是否操作成功</returns>
        protected override bool LogicCreateNodeAtPoint(FlowChartManager flowChartManager, object logicData)
        {
            object[] args = logicData as object[];
            int graphType = (int)args[0];
            Point location = (Point)args[1];

            switch (graphType)
            {
                case 1: // 创建事件结点
                    {
                        CreateNode(GraphType.EventNode, location, false);
                        break;
                    }
                case 2: // 创建条件结点
                    {
                        if (flowChartManager.CurrentGraphManager.SelectedGraphElement is SlotContainer) // 必须在有可连接的结点的基础上创建
                        {
                            CreateNode(GraphType.ConditionNode, location, true);                           
                        }

                        break;
                    }
                case 3: // 创建动作结点
                    {
                        if (flowChartManager.CurrentGraphManager.SelectedGraphElement is SlotContainer) // 必须在有可能连接的结点的基础上创建
                        {
                            CreateNode(GraphType.ActionNode, location, true);                            
                        }

                        break;
                    }
            }

            return true;
        }

        /// <summary>
        /// 打开脚本
        /// </summary>
        /// <param name="flowChartManager">绘图管理器</param>
        /// <param name="logicData">逻辑数据</param>
        /// <returns>操作是否成功</returns>
        protected override bool LogicViewCode(FlowChartManager flowChartManager, object logicData)
        {
            Helper helper = Helper.GetHelper();
            string relevatePath = logicData as string;
            string path = Path.Combine(helper.OutputDir, "scripts\\Map");
            path = Path.Combine(path, string.Format("{0}.lua", relevatePath));            

            if (File.Exists(path)) // 文件存在
            {
                try
                {
                    System.Diagnostics.Process.Start(path);
                }
                catch (IOException ex)
                {
                    MessageBox.Show("在打开代码文件时产生IO异常：" + ex.Message, "查看脚本",
                                    MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
            else
            {
                MessageBox.Show(string.Format("文件{0}不存在！", path), "查看脚本",
                                MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

            return true;
        }

        /// <summary>
        /// 查看文件夹
        /// </summary>
        /// <param name="flowChartManager">绘图管理器</param>
        /// <param name="logicData">逻辑数据</param>
        /// <returns>是否操作成功</returns>
        protected override bool LogicViewFolder(FlowChartManager flowChartManager, object logicData)
        {
            DocumentManager documentManager = DocumentManager.GetDocumentManager();
            Helper helper = Helper.GetHelper();
            TreeNode currentNode = logicData as TreeNode;

            string path = Path.Combine(helper.OutputDir, "scripts\\Map");
            path = Path.Combine(path, documentManager.GetNodePath(currentNode));
            
            if (Directory.Exists(path)) // 当前目录存在
            {
                System.Diagnostics.Process.Start("explorer.exe", path);
            }
            else // 当前目录不存在 
            {
                MessageBox.Show("该目录不存在！", "打开文件夹", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

            return true;
        }

        /// <summary>
        /// 检查逻辑
        /// </summary>
        /// <param name="flowChartManager">绘图管理器</param>
        /// <param name="logicData">逻辑数据</param>
        /// <returns>是否操作成功</returns>
        protected override bool LogicCheckLogic(FlowChartManager flowChartManager, object logicData)
        {
            bool executeResult = CheckFlowChartAvail(flowChartManager);

            return executeResult;
        }

        /// <summary>
        /// 管理变量
        /// </summary>
        /// <param name="flowChartManager">绘图管理器</param>
        /// <param name="logicData">逻辑数据</param>
        /// <returns>是否操作成功</returns>
        protected override bool LogicManageArguments(FlowChartManager flowChartManager, object logicData)
        {
            DataBaseManager dataBaseManager = DataBaseManager.GetDataBaseManager();
            VarManager.Program varManager = new VarManager.Program();
            varManager.Show(dataBaseManager.GetJx3webConnection());

            return true;
        }

        /// <summary>
        /// 检查图元是否能够编辑
        /// </summary>
        /// <param name="graphElement">当前图元</param>
        /// <returns>图元是否能够编辑</returns>
        private bool CheckGraphElementEditable(GraphElement graphElement)
        {
            bool result = true;

            if (graphElement is ConnectorContainer)
            {
                ConnectorContainer line = graphElement as ConnectorContainer;
                if (line.InSlotContainer != null && !(line.InSlotContainer is ConditionGraphElement))
                {
                    result = false;
                }
            }

            return result;
        }

        /// <summary>
        /// 获取流程图元数据
        /// </summary>
        /// <param name="dataManager">数据管理器</param>
        /// <param name="slotContainerList">流程图插槽容器的链表</param>
        /// <returns>流程图元数据的链表</returns>
        protected override List<FlowChartMetaData> GetMetaData(DataManager dataManager, List<SlotContainer> slotContainerList)
        {
            List<FlowChartMetaData> metaDataList = new List<FlowChartMetaData>();
            List<GraphElement> graphElementList = new List<GraphElement>();
            Hashtable graphElementTable = new Hashtable();

            foreach (SlotContainer slotContainer in slotContainerList)
            {
                if (slotContainer is EventGraphElement)
                {
                    object data = dataManager.GetData(slotContainer);
                    DataElement dataElement = dataManager.GetDataElement(slotContainer);
                    FlowChartMetaData metaData = new FlowChartMetaData(slotContainer.ID, data, "EventNode");
                    metaData.Code = CodeProvider.CodeProviderClass.ConvertToCode(data, dataElement.DataType);
                    metaData.DisplayText = slotContainer.TooltipText;
                    FindNextMetaData(dataManager, slotContainer, metaData, graphElementList, graphElementTable);
                    metaDataList.Add(metaData);
                }
            }

            return metaDataList;
        }

        /// <summary>
        /// 获取下一个流程图元数据
        /// </summary>
        /// <param name="dataManager">数据管理器</param>
        /// <param name="graphElement">当前图元</param>
        /// <param name="metaData">流程图元数据</param>
        /// <param name="graphElementList">遍历过的图元链表</param>
        /// <param name="graphElementTable">流程图索引哈希表</param>
        private void FindNextMetaData(DataManager dataManager, GraphElement graphElement, FlowChartMetaData metaData, List<GraphElement> graphElementList, Hashtable graphElementTable)
        {
            graphElementList.Add(graphElement);
            graphElementTable[graphElement] = metaData;
            List<GraphElement> list = dataManager.GetNextGraphElements(graphElement, false);

            foreach (GraphElement g in list)
            {
                object data = dataManager.GetData(g);                
                
                if(!graphElementList.Contains(g))
                {
                    DataElement dataElement = dataManager.GetDataElement(g);
                    FlowChartMetaData newMetaData = new FlowChartMetaData(g.ID, data, dataElement.DataType);
                    newMetaData.Code = CodeProvider.CodeProviderClass.ConvertToCode(data, dataElement.DataType);
                    newMetaData.DisplayText = g.TooltipText;
                    newMetaData.AddPreviousMetaData(metaData);
                    metaData.AddNextMetaData(newMetaData);
                    FindNextMetaData(dataManager, g, newMetaData, graphElementList, graphElementTable);
                }
                else
                {
                    FlowChartMetaData newMetaData = graphElementTable[g] as FlowChartMetaData;
                    newMetaData.AddPreviousMetaData(metaData);
                    metaData.AddNextMetaData(newMetaData);
                }
            }

            if (graphElement is ConditionGraphElement) // 条件结点
            {
                bool avail = true; // 是否需要生成else连接线

                foreach(GraphElement g in list)
                {
                    if(dataManager.GetData(g) == null) // 找到else连接线
                    {
                        avail = false;
                        break;
                    }
                }

                if(avail) // 需要生成else连接线
                {
                    FlowChartMetaData lineMetaData = new FlowChartMetaData(0, null, "Line");
                    lineMetaData.Code = "";
                    lineMetaData.AddPreviousMetaData(metaData);
                    metaData.AddNextMetaData(lineMetaData);
                    FlowChartMetaData endMetaData = new FlowChartMetaData(0, null, "EndNode");
                    endMetaData.AddPreviousMetaData(lineMetaData);
                    lineMetaData.AddNextMetaData(endMetaData);
                }
                else // 连接线需要重排序
                {
                    List<GraphElement> lineList = new List<GraphElement>();
                    GraphElement elseLine = null;
                    FlowChartMetaData newMetaData;

                    foreach(GraphElement g in list)
                    {
                        if(dataManager.GetData(g) == null) // else连接线
                        {
                            elseLine = g;
                        }
                        else // 普通连接线
                        {
                            lineList.Add(g);
                        }
                    }

                    metaData.ClearNextMetaDataList();
                    foreach(GraphElement g in lineList)
                    {
                        newMetaData = graphElementTable[g] as FlowChartMetaData;
                        metaData.AddNextMetaData(newMetaData);
                    }
                    newMetaData = graphElementTable[elseLine] as FlowChartMetaData;
                    metaData.AddNextMetaData(newMetaData);
                }
            }
            else
            {
                if(list.Count == 0) // 当前图元没有后续图元
                {
                    FlowChartMetaData lineMetaData = new FlowChartMetaData(0, null, "Line");
                    lineMetaData.AddPreviousMetaData(metaData);
                    metaData.AddNextMetaData(lineMetaData);
                    FlowChartMetaData endMetaData = new FlowChartMetaData(0, null, "EndNode");
                    endMetaData.AddPreviousMetaData(lineMetaData);
                    lineMetaData.AddNextMetaData(endMetaData);
                }
            }                        
        }

        /// <summary>
        /// 设置图元是否可用
        /// </summary>
        /// <param name="dataManager">事件管理器</param>
        /// <param name="list">遍历过的图元链表</param>
        /// <param name="graphElement">当前图元</param>
        /// <param name="enable">图元是否可用</param>
        public void ReBindEventNode(DataManager dataManager, List<GraphElement> list, GraphElement graphElement, bool enable)
        {
            if (!list.Contains(graphElement)) // 还没有遍历过当前图元
            {
                list.Add(graphElement);

                if(enable)
                {
                    graphElement.Enable = enable;
                }
                else
                {
                    if(graphElement is SlotContainer) // 插槽容器
                    {
                        if(!dataManager.IsConnectEventNode(graphElement as SlotContainer))
                        {
                            graphElement.Enable = enable;
                        }
                    }
                    else if(graphElement is ConnectorContainer) // 连接线
                    {
                        if (!dataManager.IsConnectEventNode(graphElement as ConnectorContainer))
                        {
                            graphElement.Enable = enable;
                        }
                    }                    
                }

                foreach (GraphElement g in dataManager.GetNextGraphElements(graphElement, false))
                {
                    ReBindEventNode(dataManager, list, g, enable);
                }
            }
        }

        /// <summary>
        /// 检查流程图的有效性
        /// </summary>
        /// <param name="flowChartManager">绘图管理器</param>        
        /// <returns>流程图是否有效</returns>
        private bool CheckFlowChartAvail(FlowChartManager flowChartManager)
        {
            bool avail = true;

            // 检查连接线的有效性
            avail = CheckLineAvail(flowChartManager);

            // 检查图元的有效性
            if (avail)
            {
                avail = CheckGraphElementAvail(flowChartManager);
            }

            return avail;
        }        

        /// <summary>
        /// 检查图元是否有效
        /// </summary>
        /// <param name="flowChartManager">绘图管理器</param>
        /// <returns>图元是否有效</returns>
        private bool CheckGraphElementAvail(FlowChartManager flowChartManager)
        {
            bool avail = true;
            bool unfinished = false;
            GraphElement invalidGraphElement = null;
            GraphElement unfinishedGraphElement = null;
            DocumentManager documentManager = DocumentManager.GetDocumentManager();
            DataManager dataManager = flowChartManager.CurrentDataManager;

            foreach (GraphElement graphElement in flowChartManager.CurrentGraphManager.CurrentCanvas.GraphElementList)
            {
                if (!graphElement.Enable)
                {
                    avail = false;
                    invalidGraphElement = graphElement;
                    documentManager.PrintText(string.Format("ID为{0}的图元没有正确连接事件结点！", graphElement.ID));                    
                    break;
                }
            }

            if (!avail) // 流程图无效 
            {
                MessageBox.Show("绘图中有不正确连接的图元，请确认所有的图元都已连接事件结点！", "流程图有效性检查", 
                                MessageBoxButtons.OK, MessageBoxIcon.Information);

                // 显示细节
                List<GraphElement> list = new List<GraphElement>();
                list.Add(invalidGraphElement);
                dataManager.CurrentFlowChartManager.CurrentGraphManager.SelectGraphElements(list);
            }

            if(unfinished && avail) // 有未编辑完成的图元
            {
                avail = false;
                MessageBox.Show("绘图中有没有完成编辑的图元，请确认所有的图元都已设置参数！", "流程图有效性检查", 
                                MessageBoxButtons.OK, MessageBoxIcon.Information);

                // 显示细节
                List<GraphElement> list = new List<GraphElement>();
                list.Add(unfinishedGraphElement);
                dataManager.CurrentFlowChartManager.CurrentGraphManager.SelectGraphElements(list);
            }

            return avail;
        }

        /// <summary>
        /// 检查连接线是否有效
        /// </summary>
        /// <param name="flowChartManager">绘图管理器</param>        
        /// <returns>连接线是否有效</returns>
        private bool CheckLineAvail(FlowChartManager flowChartManager)
        {
            bool avail = true;
            ConnectorContainer invalidLine = null;
            DocumentManager documentManager = DocumentManager.GetDocumentManager();

            foreach (ConnectorContainer line in flowChartManager.CurrentGraphManager.ConnectorContainerList)
            {
                if (line.InSlotContainer == null || line.OutSlotContainer == null) // 连接线两端必须连接上结点
                {
                    avail = false;
                    invalidLine = line;
                    documentManager.PrintText(string.Format("ID为{0}的连接线没有正确连接！", line.ID));
                    break;
                }
            }

            if (!avail) // 流程图无效 
            {
                MessageBox.Show("绘图中有不完整的连接，请确认所有的连接是否正确！", "流程图有效性检查", 
                                MessageBoxButtons.OK, MessageBoxIcon.Information);

                // 显示细节
                List<GraphElement> list = new List<GraphElement>();
                list.Add(invalidLine);
                flowChartManager.CurrentGraphManager.SelectGraphElements(list);
            }

            return avail;
        }
    }
}
