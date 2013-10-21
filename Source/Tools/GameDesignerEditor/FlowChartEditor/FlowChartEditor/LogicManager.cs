using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Drawing;
using System.IO;
using FlowChartEditor.BaseGraphElement;
using DevComponents.DotNetBar;
using DevComponents.DotNetBar.Controls;
using GraphElementLibrary;
using Compiler_Sword3;
using DBManager;
using FlowChartDataLibrary;

namespace FlowChartEditor
{
    class LogicManager
    {
        private static LogicManager logicManager; // 逻辑管理器

        /// <summary>
        /// 私有构造函数，单例模式
        /// </summary>
        private LogicManager()
        {

        }

        /// <summary>
        /// 获取逻辑管理器实例
        /// </summary>
        /// <returns>逻辑管理器实例</returns>
        public static LogicManager GetLogicManager()
        {
            if(logicManager == null)
            {
                logicManager = new LogicManager();
            }

            return logicManager;
        }

        /// <summary>
        /// 检查是否更新事件结点
        /// </summary>
        /// <param name="graphManager">图元管理器</param>
        /// <param name="graphElement">当前图元</param>
        /// <param name="eventNode">事件结点</param>
        /// <returns>是否需要更新</returns>
        private bool CheckCanBindEventNode(GraphManager graphManager, GraphElement graphElement, EventGraphElement eventNode)
        {
            bool avail = true;            
            DataManager dataManager = graphManager.CurrentFlowChartManager.CurrentDataManager;
            IComparable com1;
            IComparable com2;

            if (graphElement is SlotContainer) // 插槽容器
            {
                SlotContainer slotContainer = graphElement as SlotContainer;
                if (slotContainer.EventNode != null && slotContainer.EventNode != eventNode)
                {
                    com1 = dataManager.GetData(slotContainer.EventNode) as IComparable;
                    com2 = dataManager.GetData(eventNode) as IComparable;
                    avail = (com1.CompareTo(com2) == 0);
                }
            }
            else if (graphElement is ConnectorContainer) // 连接线
            {
                ConnectorContainer line = graphElement as ConnectorContainer;
                if (line.EventNode != null && line.EventNode != eventNode)
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
        /// <param name="graphManager">图元管理器</param>
        /// <param name="list">遍历过的图元链表</param>
        /// <param name="graphElement">当前图元</param>
        /// <param name="eventNode">事件结点</param>
        /// <param name="enable">图元是否可用</param>
        public void ReBindEventNode(GraphManager graphManager, List<GraphElement> list, GraphElement graphElement, EventGraphElement eventNode, bool enable)
        {
            if (!list.Contains(graphElement)) // 还没有遍历过当前图元
            {
                DataManager dataManager = graphManager.CurrentFlowChartManager.CurrentDataManager;

                if (graphElement is SlotContainer) // 该图元是插槽容器
                {
                    SlotContainer slotContainer = graphElement as SlotContainer;

                    if (!enable && slotContainer.EventNode != null && dataManager.IsConnect(slotContainer.EventNode, slotContainer)) // 不需要更新当前图元的事件结点
                    {

                    }
                    else
                    {
                        list.Add(slotContainer);
                        slotContainer.Enable = enable;
                        slotContainer.EventNode = eventNode;

                        foreach (GraphElement g in dataManager.GetNextGraphElements(graphElement))
                        {
                            ReBindEventNode(graphManager, list, g, eventNode, enable);
                        }
                    }
                }
                else if (graphElement is ConnectorContainer) // 该图元是连接线控制点容器
                {
                    ConnectorContainer line = graphElement as ConnectorContainer;

                    if (!enable && line.EventNode != null && dataManager.IsConnect(line.EventNode, line)) // 不需要更新当前图元的事件结点
                    {

                    }
                    else
                    {
                        list.Add(line);
                        line.Enable = enable;
                        line.EventNode = eventNode;

                        foreach (GraphElement g in dataManager.GetNextGraphElements(graphElement))
                        {
                            ReBindEventNode(graphManager, list, g, eventNode, enable);
                        }
                    }
                }
            }
        }

        /// <summary>
        /// 检查插槽容器的所有连出的线是否有效
        /// </summary>
        /// <param name="slotContainer">插槽容器</param>
        private void CheckLineValid(DataManager dataManager, SlotContainer slotContainer)
        {
            int invalidLineCount = 0;
            List<ConnectorContainer> list = new List<ConnectorContainer>();

            foreach (SlotGraphElement slot in slotContainer.GetOutSlotList())
            {
                if (slot.Binded)
                {
                    object data = dataManager.GetData(slot.BindingConnector.Line);
                    if (data == null)
                    {
                        list.Add(slot.BindingConnector.Line);
                        invalidLineCount++;
                    }
                    else
                    {
                        slot.BindingConnector.Line.Invalid = false;
                    }
                }
            }

            if (invalidLineCount == 1) // 无效连接线数等于1
            {
                foreach (ConnectorContainer line in list)
                {
                    line.Invalid = false;
                    line.Text = "else";
                    line.ShowText = true;
                    line.AdjustText();
                }
            }
            else if (invalidLineCount > 1) // 无效连接线数大于1
            {
                foreach (ConnectorContainer line in list)
                {
                    line.Invalid = true;
                    line.Text = "×";
                    line.ShowText = true;
                    line.AdjustText();
                }
            }
            else
            {
                foreach (ConnectorContainer line in list)
                {
                    line.Invalid = false;
                    line.ShowText = false;
                }
            }
        }

        /// <summary>
        /// 检查流程图的有效性
        /// </summary>
        /// <param name="dataManager">数据管理器</param>
        /// <param name="name">流程图的名称</param>
        /// <param name="slotContainerList">插槽容器链表</param>
        /// <param name="connectorContainerList">连接线链表</param>
        /// <param name="logBox">输入日志窗体</param>
        /// <param name="showDetail">是否显示细节</param>
        /// <returns>流程图是否有效</returns>
        private bool CheckFlowChartAvail(DataManager dataManager, string name, List<SlotContainer> slotContainerList, List<ConnectorContainer> connectorContainerList,
            TextBoxX logBox, bool showDetail)
        {
            bool avail = true;

            // 检查连接线的有效性
            avail = CheckLineAvail(dataManager, name, connectorContainerList, logBox, showDetail);

            // 检查图元的有效性
            if (avail)
            {
                List<GraphElement> list = new List<GraphElement>();
                foreach (SlotContainer slotContainer in slotContainerList)
                {
                    list.Add(slotContainer);
                }

                foreach (ConnectorContainer line in connectorContainerList)
                {
                    list.Add(line);
                }

                avail = CheckGraphElementAvail(dataManager, name, list, logBox, showDetail);
            }

            // 检查循环的有效性
            if (avail)
            {
                avail = CheckCircleAvail(dataManager, name, slotContainerList, logBox, showDetail);
            }

            return avail;
        }

        /// <summary>
        /// 检查图元是否有效
        /// </summary>
        /// <param name="dataManager">数据管理器</param>
        /// <param name="name">流程图的名称</param>
        /// <param name="graphElementList">图元链表</param>
        /// <param name="showDetail">是否显示细节</param>
        /// <param name="logBox">输出日志窗体</param>
        /// <returns>图元是否有效</returns>
        private bool CheckGraphElementAvail(DataManager dataManager, string name, List<GraphElement> graphElementList, TextBoxX logBox, bool showDetail)
        {
            bool avail = true;
            GraphElement invalidGraphElement = null;

            foreach (GraphElement graphElement in graphElementList)
            {
                if (!graphElement.Enable)
                {
                    avail = false;
                    invalidGraphElement = graphElement;
                    logBox.AppendText(string.Format("ID为{0}的图元没有正确连接事件结点！", graphElement.ID));
                    break;
                }
            }

            if (!avail) // 流程图无效 
            {
                MessageBox.Show(string.Format("流程图 {0} 中有不正确连接的图元，请确认所有的图元都已连接事件结点！", name),
                        "流程图有效性检查", MessageBoxButtons.OK, MessageBoxIcon.Information);

                if (showDetail) // 显示细节
                {
                    List<GraphElement> list = new List<GraphElement>();
                    list.Add(invalidGraphElement);
                    dataManager.CurrentFlowChartManager.CurrentGraphManager.SelectGraphElements(list);
                }
            }

            return avail;
        }

        /// <summary>
        /// 检查连接线是否有效
        /// </summary>
        /// <param name="dataManager">数据管理器</param>
        /// <param name="name">流程图的名称</param>
        /// <param name="connectorContainerList">连接线链表</param>
        /// <param name="showDetail">是否显示细节</param>
        /// <param name="logBox">输出日志窗体</param>
        /// <returns>连接线是否有效</returns>
        private bool CheckLineAvail(DataManager dataManager, string name, List<ConnectorContainer> connectorContainerList, TextBoxX logBox, bool showDetail)
        {
            bool avail = true;
            ConnectorContainer invalidLine = null;

            foreach (ConnectorContainer line in connectorContainerList)
            {
                if (line.InSlotContainer == null || line.OutSlotContainer == null) // 连接线两端必须连接上结点
                {
                    avail = false;
                    invalidLine = line;
                    logBox.AppendText(string.Format("ID为{0}的连接线没有正确连接！\r\n", line.ID));
                    break;
                }
                else if (line.Invalid) // 连接线必须有效
                {
                    avail = false;
                    invalidLine = line;
                    logBox.AppendText(string.Format("ID为{0}的连接线没有设置条件！\r\n", line.ID));
                    break;
                }
            }

            if (!avail) // 流程图无效 
            {
                MessageBox.Show(string.Format("流程图 {0} 中有不完整的连接，请确认所有的连接是否正确！", name),
                    "流程图有效性检查", MessageBoxButtons.OK, MessageBoxIcon.Information);

                if (showDetail) // 显示细节
                {
                    List<GraphElement> list = new List<GraphElement>();
                    list.Add(invalidLine);
                    dataManager.CurrentFlowChartManager.CurrentGraphManager.SelectGraphElements(list);
                }
            }

            return avail;
        }

        /// <summary>
        /// 检测循环圈是否有效
        /// </summary>
        /// <param name="dataManager">数据管理器</param>
        /// <param name="name">流程图的名称</param>
        /// <param name="slotContainerList">插槽容器链表</param>
        /// <param name="showDetail">是否显示细节</param>
        /// <param name="logBox">输出日志窗体</param>
        /// <return>循环圈是否有效</return>
        private bool CheckCircleAvail(DataManager dataManager, string name, List<SlotContainer> slotContainerList, TextBoxX logBox, bool showDetail)
        {
            bool avail = true;
            List<SlotContainer> list = new List<SlotContainer>();
            List<List<SlotContainer>> circleLists = new List<List<SlotContainer>>();
            List<SlotContainer> circleNodeList = new List<SlotContainer>();
            List<List<SlotContainer>> markList = new List<List<SlotContainer>>();

            foreach (SlotContainer slotContainer in slotContainerList)
            {
                if (slotContainer is EventGraphElement)
                {
                    dataManager.FindNextNode(slotContainer, list, circleLists, circleNodeList);
                }
            }

            if (circleLists.Count > 0)
            {
                foreach (List<SlotContainer> circleList in circleLists)
                {
                    avail = false;

                    foreach (SlotContainer node in circleList)
                    {
                        if (node is ConditionGraphElement) // 找到所有的条件结点
                        {
                            // 检查当前圈中的连接线是设置了条件的条件连接线
                            foreach (GraphElement graphElement in dataManager.GetNextGraphElements(node))
                            {
                                ConnectorContainer line = graphElement as ConnectorContainer;
                                if (dataManager.GetData(line) != null && circleList.Contains(line.OutSlotContainer))
                                {
                                    avail = true;
                                    break;
                                }
                            }

                            if (avail)
                            {
                                break;
                            }

                            // 检查是否有通过条件结点连出圈的结点
                            foreach (SlotContainer nextNode in dataManager.GetSpecifyNextSlotContainers(node, circleList))
                            {
                                if (!circleNodeList.Contains(nextNode)) // 有连出圈的结点
                                {
                                    avail = true;
                                    break;
                                }
                            }

                            if (avail)
                            {
                                break;
                            }
                        }
                    }

                    if (!avail)
                    {
                        markList.Add(circleList);
                        string circleString = "";
                        for (int i = 0; i < circleList.Count; i++)
                        {
                            circleString += circleList[i].ID + " -> ";
                        }
                        circleString += circleList[0].ID;
                        logBox.AppendText(string.Format("检测出循环圈{0}！\r\n", circleString));
                    }
                }

                if (markList.Count > 0) // 检测到无效的循环
                {
                    avail = false;
                    MessageBox.Show(string.Format("流程图 {0} 中有可能无效的循环，请确认所有的循环是否设置了有效出口！", name),
                        "流程图有效性检查", MessageBoxButtons.OK, MessageBoxIcon.Information);

                    if (showDetail) // 显示细节
                    {
                        CheckCircleForm cForm = new CheckCircleForm(dataManager.CurrentFlowChartManager.CurrentGraphManager, markList);
                        cForm.ShowDialog();
                    }
                }
            }

            return avail;
        }

        /// <summary>
        /// 执行逻辑功能
        /// </summary>
        /// <param name="flowChartManager">流程图管理器</param>
        /// <param name="logicType">逻辑类型</param>
        /// <param name="logicData">逻辑数据</param>
        /// <return>执行结果</return>
        public bool DoLogicFunction(FlowChartManager flowChartManager, LogicType logicType, object logicData)
        {
            bool result = true;
            DataBaseManager dataBaseManager = DataBaseManager.GetDataBaseManager();
            GraphManager graphManager = null;
            DataManager dataManager = null;
            if(flowChartManager != null)
            {
                graphManager = flowChartManager.CurrentGraphManager;
                dataManager = flowChartManager.CurrentDataManager;
            }

            switch(logicType)
            {
                case LogicType.Connect: // 连接操作
                    {                        
                        object[] data = logicData as object[];
                        SlotGraphElement slot = data[0] as SlotGraphElement;
                        ConnectorGraphElement connector = data[1] as ConnectorGraphElement;                        

                        switch (flowChartManager.CurrentChartMode)
                        {
                            case ChartMode.FlowChart: // 编辑流程图模式
                                {
                                    List<GraphElement> list = new List<GraphElement>(); // 记录遍历过的图元的链表

                                    if (connector.IsHeadPoint) // 连入插槽容器
                                    {
                                        SlotContainer inSlotContainer = connector.Line.InSlotContainer;
                                        if (inSlotContainer != null)
                                        {
                                            slot.SlotContainer.EventNode = inSlotContainer.EventNode;
                                            bool enable = (inSlotContainer.EventNode != null && dataManager.IsConnect(inSlotContainer.EventNode, inSlotContainer));

                                            // 检查是否可以更新绑定事件
                                            if (CheckCanBindEventNode(graphManager, slot.SlotContainer, inSlotContainer.EventNode))
                                            {
                                                ReBindEventNode(graphManager, list, slot.SlotContainer, inSlotContainer.EventNode, enable); // 重新绑定当前图元与其连出图元的事件结点
                                            }
                                            else
                                            {
                                                slot.UnBind();
                                                MessageBox.Show("表达式参数与事件类型不匹配！\r\n\r\n请检查连入事件结点的类型是否正确",
                                                    "连接检查", MessageBoxButtons.OK, MessageBoxIcon.Information);
                                                result = false;
                                            }
                                        }
                                    }
                                    else // 连出插槽容器
                                    {
                                        SlotContainer outSlotContainer = connector.Line.OutSlotContainer;
                                        if (outSlotContainer != null)
                                        {
                                            bool enable = (slot.SlotContainer.EventNode != null && dataManager.IsConnect(slot.SlotContainer.EventNode, slot.SlotContainer));

                                            // 检查是否可以更新绑定事件
                                            if (CheckCanBindEventNode(graphManager, connector.Line, slot.SlotContainer.EventNode))
                                            {
                                                ReBindEventNode(graphManager, list, connector.Line, slot.SlotContainer.EventNode, enable); // 重新绑定当前图元与其连出图元的事件结点
                                            }
                                            else
                                            {
                                                slot.UnBind();
                                                MessageBox.Show("表达式参数与事件类型不匹配！\r\n\r\n请检查连入事件结点的类型是否正确",
                                                    "连接检查", MessageBoxButtons.OK, MessageBoxIcon.Information);
                                                result = false;
                                            }
                                        }
                                    }

                                    if (slot.IsOutSlot && slot.SlotContainer is ConditionGraphElement) //  对条件结点连出的线要进行有效性检查
                                    {
                                        CheckLineValid(dataManager, slot.SlotContainer);
                                    }

                                    break;
                                }
                            case ChartMode.AIChart: // 编辑AI图模式
                                {
                                    break;
                                }
                        }

                        break;
                    }
                case LogicType.Disconnect: // 解除连接操作
                    {
                        object[] args = logicData as object[];
                        SlotGraphElement slot = args[0] as SlotGraphElement;                        
                        ConnectorGraphElement connector = args[1] as ConnectorGraphElement;
                        ConnectorContainer line = connector.Line;
                        SlotContainer slotContainer = slot.SlotContainer;

                        switch (flowChartManager.CurrentChartMode)
                        {
                            case ChartMode.FlowChart: // 编辑流程图模式
                                {
                                    List<GraphElement> list = new List<GraphElement>(); // 记录遍历的图元的链表
                                    if (connector.IsHeadPoint) // 移出连接线的头结点
                                    {
                                        ReBindEventNode(graphManager, list, slotContainer, null, false); // 重新绑定当前图元与其连出图元的事件结点
                                    }
                                    else // 移出连接线的尾结点
                                    {
                                        SlotContainer outSlotContainer = connector.Line.OutSlotContainer;
                                        if (outSlotContainer != null)
                                        {
                                            ReBindEventNode(graphManager, list, connector.Line, null, false); // 重新绑定当前图元与其连出图元的事件结点
                                        }
                                    }

                                    if (slot.IsOutSlot && slotContainer is ConditionGraphElement) //  对条件结点连出的线要进行有效性检查
                                    {
                                        CheckLineValid(dataManager, slotContainer);
                                        if (line.Text == "×") // 连接线无效
                                        {
                                            line.Invalid = false;
                                            line.ShowText = false;
                                        }
                                    }

                                    break;
                                }
                            case ChartMode.AIChart: // 编辑AI图模式
                                {
                                    break;
                                }
                        } 

                        break;
                    }
                case LogicType.BeforeDelete: // 删除之前的操作
                    {
                        GraphElement graphElement = logicData as GraphElement;

                        switch(flowChartManager.CurrentChartMode)
                        {
                            case ChartMode.FlowChart: // 编辑流程图模式
                                {
                                    if(graphElement is SlotContainer) // 当前图元是插槽容器
                                    {
                                        SlotContainer slotContainer = graphElement as SlotContainer;                                                                                

                                        if(slotContainer is ConditionGraphElement) // 对条件结点连出的线要进行有效性检查
                                        {
                                            foreach(SlotGraphElement slot in slotContainer.GetOutSlotList())
                                            {
                                                if(slot.Binded)
                                                {
                                                    ConnectorContainer line = slot.BindingConnector.Line;
                                                    if (line.Invalid)
                                                    {
                                                        line.Invalid = false;
                                                        line.ShowText = false;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if(graphElement is ConnectorContainer) // 当前图元是连接线
                                    {
                                        ConnectorContainer line = graphElement as ConnectorContainer;

                                        if (line.InSlotContainer is ConditionGraphElement) // 对条件结点连出的线要进行有效性检查
                                        {
                                            CheckLineValid(dataManager, line.InSlotContainer);
                                        }
                                    }

                                    break;
                                }
                            case ChartMode.AIChart: // 编辑AI图模式
                                {
                                    break;
                                }
                        }

                        break;
                    }
                case LogicType.AfterDelete: // 删除之后的操作
                    {
                        List<GraphElement> graphElementList = logicData as List<GraphElement>;

                        foreach(GraphElement graphElement in graphElementList)
                        {
                            List<GraphElement> list = new List<GraphElement>(); // 记录遍历过图元的链表
                            ReBindEventNode(graphManager, list, graphElement, null, false); // 重新设定事件结点及图元是否可用
                        }

                        break;
                    }
                case LogicType.Edit: // 编辑操作
                    {
                        switch(flowChartManager.CurrentChartMode)
                        {
                            case ChartMode.FlowChart: // 流程图编辑模式
                                {
                                    GraphElement graphElement = logicData as GraphElement;

                                    if (graphElement is SlotContainer)
                                    {
                                        SlotContainer slotContainer = graphElement as SlotContainer;
                                        if (graphElement is ConditionGraphElement)
                                        {
                                            CheckLineValid(dataManager, slotContainer);
                                        }

                                        // 根据文本内容调整插槽容器的大小
                                        slotContainer.AdjustElementSize();
                                    }
                                    else if (graphElement is ConnectorContainer)
                                    {
                                        ConnectorContainer line = graphElement as ConnectorContainer;

                                        if (line.InSlotContainer != null && line.InSlotContainer is ConditionGraphElement) // 对条件结点的连接线要特殊检查
                                        {
                                            CheckLineValid(dataManager, line.InSlotContainer);
                                        }
                                    }

                                    break;
                                }
                            case ChartMode.AIChart: // AI图编辑模式
                                {
                                    break;
                                }
                        }

                        break;
                    }
                case LogicType.CheckEditable:
                    {
                        GraphElement graphElement = logicData as GraphElement;

                        switch(flowChartManager.CurrentChartMode)
                        {
                            case ChartMode.FlowChart: // 编辑流程图模式
                                {
                                    if (graphElement is ConnectorContainer)
                                    {
                                        ConnectorContainer line = graphElement as ConnectorContainer;
                                        if (line.InSlotContainer != null && !(line.InSlotContainer is ConditionGraphElement))
                                        {
                                            result = false;
                                        }
                                    }

                                    break;
                                }
                            case ChartMode.AIChart: // 编辑AI图模式
                                {
                                    break;
                                }
                        }

                        break;
                    }
                case LogicType.CreateLine: // 创建连接线操作
                    {
                        object[] args = logicData as object[];
                        int id = (int)args[0];
                        SlotGraphElement slot = args[1] as SlotGraphElement;
                        Point p = (Point)args[2];
                        Helper helper = Helper.GetHelper();

                        switch(flowChartManager.CurrentChartMode)
                        {
                            case ChartMode.FlowChart:
                                {
                                    LineGraphElement lineGraphElement = new LineGraphElement(slot.Location, p);
                                    lineGraphElement.ID = id;
                                    lineGraphElement.Name = "连接线";
                                    lineGraphElement.TextFont = helper.TextFont;
                                    lineGraphElement.InvalidColor = Color.Brown;
                                    lineGraphElement.Init();
                                    slot.Bind(lineGraphElement.GetConnectorList()[0]);
                                    lineGraphElement.Refresh();
                                    graphManager.SelectedGraphElement = lineGraphElement;

                                    break;
                                }
                            case ChartMode.AIChart:
                                {
                                    AILineGraphElement aiLineGraphElement = new AILineGraphElement(slot.Location, p);
                                    aiLineGraphElement.ID = id;
                                    aiLineGraphElement.Name = "AI连接线";
                                    aiLineGraphElement.TextFont = helper.TextFont;
                                    aiLineGraphElement.InvalidColor = Color.Brown;
                                    aiLineGraphElement.Init();
                                    slot.Bind(aiLineGraphElement.GetConnectorList()[0]);
                                    aiLineGraphElement.Refresh();
                                    graphManager.SelectedGraphElement = aiLineGraphElement;

                                    break;
                                }
                        }

                        break;
                    }
                case LogicType.CreateAndConnectLine: // 创建并连接连接线
                    {
                        Helper helper = Helper.GetHelper();
                        object[] args = logicData as object[];
                        int id = (int)args[0];
                        SlotGraphElement outSlot = args[1] as SlotGraphElement;
                        SlotGraphElement inSlot = args[2] as SlotGraphElement;

                        switch(flowChartManager.CurrentChartMode) 
                        {
                            case ChartMode.FlowChart: // 流程图编辑模式
                                {
                                    LineGraphElement lineGraphElement = new LineGraphElement(outSlot.Location, inSlot.Location);
                                    lineGraphElement.ID = id;
                                    lineGraphElement.Name = "连接线";
                                    lineGraphElement.TextFont = helper.TextFont;
                                    lineGraphElement.InvalidColor = Color.Brown;
                                    lineGraphElement.Init();
                                    lineGraphElement.Refresh();
                                    graphManager.SelectedGraphElement = lineGraphElement;

                                    break;
                                }
                            case ChartMode.AIChart: // AI图编辑模式
                                {
                                    AILineGraphElement aiLineGraphElement = new AILineGraphElement(outSlot.Location, inSlot.Location);
                                    aiLineGraphElement.ID = id;
                                    aiLineGraphElement.Name = "AI连接线";
                                    aiLineGraphElement.TextFont = helper.TextFont;
                                    aiLineGraphElement.InvalidColor = Color.Brown;
                                    aiLineGraphElement.Init();
                                    aiLineGraphElement.Refresh();
                                    graphManager.SelectedGraphElement = aiLineGraphElement;

                                    break;
                                }
                        }                        

                        break;
                    }
                case LogicType.PopUpContextMenu: // 弹出右键菜单
                    {
                        ButtonItem contextMenu = logicData as ButtonItem;

                        switch(flowChartManager.CurrentChartMode)
                        {
                            case ChartMode.FlowChart: // 编辑流程图模式
                                {
                                    if (graphManager.SelectedGraphElement != null) // 有图元被选中
                                    {
                                        contextMenu.SubItems["bAddConditionFlowChart"].Text = "创建并连接 条件结点 (<u>2</u>)";
                                        contextMenu.SubItems["bAddActionFlowChart"].Text = "创建并连接 动作结点 (<u>3</u>)";
                                    }
                                    else
                                    {
                                        contextMenu.SubItems["bCreateEventFlowChart"].Text = "创建事件结点 (<u>1</u>)";
                                    }

                                    break;
                                }
                            case ChartMode.AIChart: // 编辑AI图模式
                                {
                                    if (graphManager.SelectedGraphElement != null) // 有图元被选中
                                    {
                                        contextMenu.SubItems["bAddConditionFlowChart"].Text = "创建并连接 AI分支结点 (<u>2</u>)";
                                        contextMenu.SubItems["bAddActionFlowChart"].Text = "创建并连接 AI动作结点 (<u>3</u>)";
                                    }
                                    else
                                    {
                                        contextMenu.SubItems["bCreateEventFlowChart"].Text = "创建AI状态结点 (<u>1</u>)";
                                    }

                                    break;
                                }
                        }

                        break;
                    }
                case LogicType.Create: // 右键菜单中创建图元
                    {
                        GraphType graphType = (GraphType)logicData;
                        DocumentManager documentManager = DocumentManager.GetDocumentManager();

                        switch(flowChartManager.CurrentChartMode)
                        {
                            case ChartMode.FlowChart: // 编辑流程图模式
                                {
                                    switch(graphType)
                                    {
                                        case GraphType.EventNode: // 创建事件结点
                                            {
                                                flowChartManager.CurrentGraphManager.CreateAbbreviateGraphElement(GraphType.EventNode, false);
                                                documentManager.AddGraphType = GraphType.EventNode;
                                                documentManager.AddGraphElement = true;

                                                break;
                                            }
                                        case GraphType.ConditionNode: // 创建条件结点
                                            {
                                                flowChartManager.CurrentGraphManager.CreateAbbreviateGraphElement(GraphType.ConditionNode, true);
                                                documentManager.AddGraphType = GraphType.ConditionNode;
                                                documentManager.AddGraphElement = true;

                                                break;
                                            }
                                        case GraphType.ActionNode: // 创建动作结点
                                            {
                                                flowChartManager.CurrentGraphManager.CreateAbbreviateGraphElement(GraphType.ActionNode, true);
                                                documentManager.AddGraphType = GraphType.ActionNode;
                                                documentManager.AddGraphElement = true;

                                                break;
                                            }
                                    }

                                    break;
                                }
                            case ChartMode.AIChart: // 编辑AI图模式
                                {
                                    switch (graphType)
                                    {
                                        case GraphType.EventNode: // 创建AI状态结点
                                            {
                                                if(flowChartManager.CurrentGraphManager.SelectedGraphElement != null)
                                                {
                                                    flowChartManager.CurrentGraphManager.CreateAbbreviateGraphElement(GraphType.AIStateNode, true);
                                                }
                                                else
                                                {
                                                    flowChartManager.CurrentGraphManager.CreateAbbreviateGraphElement(GraphType.AIStateNode, false);
                                                }
                                                
                                                documentManager.AddGraphType = GraphType.AIStateNode;
                                                documentManager.AddGraphElement = true;

                                                break;
                                            }
                                        case GraphType.ConditionNode: // 创建AI动作结点
                                            {
                                                flowChartManager.CurrentGraphManager.CreateAbbreviateGraphElement(GraphType.AIActionNode, true);
                                                documentManager.AddGraphType = GraphType.AIActionNode;
                                                documentManager.AddGraphElement = true;

                                                break;
                                            }
                                        case GraphType.ActionNode: // 创建AI动作组结点
                                            {
                                                flowChartManager.CurrentGraphManager.CreateAbbreviateGraphElement(GraphType.AIActionsNode, true);
                                                documentManager.AddGraphType = GraphType.AIActionsNode;
                                                documentManager.AddGraphElement = true;

                                                break;
                                            }
                                    }

                                    break;
                                }
                        }

                        break;
                    }
                case LogicType.CreateAtPoint: // 在指定点创建
                    {
                        DocumentManager documentManager = DocumentManager.GetDocumentManager();
                        object[] args = logicData as object[];
                        int graphType = (int)args[0];
                        Point location = (Point)args[1];

                        switch(flowChartManager.CurrentChartMode)
                        {
                            case ChartMode.FlowChart: // 编辑流程图模式
                                {
                                    switch(graphType)
                                    {
                                        case 1: // 创建事件结点
                                            {
                                                flowChartManager.CurrentGraphManager.CreateAbbreviateGraphElement(GraphType.EventNode, location, false);
                                                documentManager.AddGraphType = GraphType.EventNode;
                                                documentManager.AddGraphElement = true;

                                                break;
                                            }
                                        case 2: // 创建条件结点
                                            {
                                                flowChartManager.CurrentGraphManager.CreateAbbreviateGraphElement(GraphType.ConditionNode, location, true);
                                                documentManager.AddGraphType = GraphType.ConditionNode;
                                                documentManager.AddGraphElement = true;

                                                break;
                                            }
                                        case 3: // 创建动作结点
                                            {
                                                flowChartManager.CurrentGraphManager.CreateAbbreviateGraphElement(GraphType.ActionNode, location, true);
                                                documentManager.AddGraphType = GraphType.ActionNode;
                                                documentManager.AddGraphElement = true;

                                                break;
                                            }
                                    }

                                    break;
                                }
                            case ChartMode.AIChart: // 编辑AI图模式
                                {
                                    switch(graphType)
                                    {
                                        case 1: // 创建AI状态结点
                                            {
                                                if(flowChartManager.CurrentGraphManager.SelectedGraphElement != null)
                                                {
                                                    flowChartManager.CurrentGraphManager.CreateAbbreviateGraphElement(GraphType.AIStateNode, location, true);
                                                }
                                                else
                                                {
                                                    flowChartManager.CurrentGraphManager.CreateAbbreviateGraphElement(GraphType.AIStateNode, location, false);
                                                }
                                                
                                                documentManager.AddGraphType = GraphType.AIStateNode;
                                                documentManager.AddGraphElement = true;

                                                break;
                                            }
                                        case 2: // 创建AI动作结点
                                            {
                                                if(graphManager.SelectedGraphElement is SlotContainer)
                                                {
                                                    flowChartManager.CurrentGraphManager.CreateAbbreviateGraphElement(GraphType.AIActionNode, location, true);
                                                    documentManager.AddGraphType = GraphType.AIActionNode;
                                                    documentManager.AddGraphElement = true;
                                                }                                                

                                                break;
                                            }
                                        case 3: // 创建AI动作组结点
                                            {
                                                if(graphManager.SelectedGraphElement is SlotContainer)
                                                {
                                                    flowChartManager.CurrentGraphManager.CreateAbbreviateGraphElement(GraphType.AIActionsNode, location, true);
                                                    documentManager.AddGraphType = GraphType.AIActionsNode;
                                                    documentManager.AddGraphElement = true;
                                                }                                                

                                                break;
                                            }
                                    }                                    

                                    break;
                                }
                        }

                        break;
                    }
                case LogicType.ViewCode: // 查看代码
                    {
                        DocumentManager documentManager = DocumentManager.GetDocumentManager();
                        Helper helper = Helper.GetHelper();
                        string relevatePath = logicData as string;

                        switch(documentManager.CurrentChartMode)
                        {
                            case ChartMode.FlowChart: // 编辑流程图模式
                                {
                                    string path = Path.Combine(helper.OutputDir, "scripts\\Map");
                                    path = Path.Combine(path, string.Format("{0}.lua", relevatePath));
                                    FileInfo fi = new FileInfo(path);
                                    if(fi.Exists) // 文件存在
                                    {
                                        try
                                        {
                                            System.Diagnostics.Process.Start(path);
                                        }
                                        catch (Exception ex)
                                        {
                                            MessageBox.Show("在查看代码文件时产生IO异常：" + ex.ToString());	
                                        }
                                    }
                                    else
                                    {
                                        MessageBox.Show(string.Format("文件{0}不存在！", path), "查看代码",
                                            MessageBoxButtons.OK, MessageBoxIcon.Information);
                                    }

                                    break;
                                }
                            case ChartMode.AIChart: // 编辑AI图模式
                                {
                                    break;
                                }
                        }

                        break;
                    }
                case LogicType.CompileCode: // 生成代码
                    {                       

                        break;
                    }
                case LogicType.CompileCodes: // 生成代码
                    {
                        break;
                    }
                case LogicType.CheckLogic: // 检查逻辑
                    {
                        object[] args = logicData as object[];
                        dataManager = args[0] as DataManager;
                        string name = args[1] as string;
                        List<SlotContainer> slotContainerList = args[2] as List<SlotContainer>;
                        List<ConnectorContainer> connectorContainerList = args[3] as List<ConnectorContainer>;
                        TextBoxX logBox = args[4] as TextBoxX;
                        bool showDetail = (bool)args[5];

                        switch(flowChartManager.CurrentChartMode)
                        {
                            case ChartMode.FlowChart: // 编辑流程图模式
                                {
                                    result = CheckFlowChartAvail(dataManager, name, slotContainerList, connectorContainerList, logBox,
                                        showDetail);
                                    break;
                                }
                            case ChartMode.AIChart: // 编辑AI图模式
                                {
                                    break;
                                }
                        }

                        break;
                    }
                case LogicType.ManageArguments: // 管理变量
                    {
                        DocumentManager documentManager = DocumentManager.GetDocumentManager();

                        switch(documentManager.CurrentChartMode)
                        {
                            case ChartMode.FlowChart: // 编辑流程图模式
                                {
                                    VarManager.Program varManager = new VarManager.Program();
                                    varManager.Show(dataBaseManager.GetJx3webConnection());

                                    break;
                                }
                            case ChartMode.AIChart: // 编辑AI图模式
                                {
                                    flowChartManager = documentManager.CurrentFlowChartManager;

                                    if(flowChartManager == null)
                                    {
                                        MessageBox.Show("当前没有流程图被激活！", "变量管理", MessageBoxButtons.OK,
                                            MessageBoxIcon.Information);
                                        result = false;
                                    }
                                    else
                                    {

                                    }

                                    break;
                                }
                        }

                        break;
                    }
                case LogicType.SetCompileArgs: // 设置代码生成的参数
                    {
                        FlowChartMetaDataManager flowChartMetaDataManager = logicData as FlowChartMetaDataManager;
                        Helper helper = Helper.GetHelper();

                        switch(flowChartManager.CurrentChartMode)
                        {
                            case ChartMode.FlowChart: // 编辑流程图模式
                                {                                    
                                    string fullPath = Path.Combine(helper.OutputDir, "scripts\\Map");
                                    fullPath = Path.Combine(fullPath, flowChartManager.CurrentPath);
                                    string[] data = flowChartManager.CurrentPath.Split(new char[] { '\\' });

                                    flowChartMetaDataManager.RootPath = helper.OutputDir;
                                    flowChartMetaDataManager.FlowChartFullPath = fullPath;
                                    flowChartMetaDataManager.FlowChartShortPath = flowChartManager.CurrentPath;
                                    flowChartMetaDataManager.FlowChartName = data[data.Length - 1];
                                    flowChartMetaDataManager.MapName = data[0];

                                    break;
                                }
                            case ChartMode.AIChart: // 编辑AI图模式
                                {
                                    string fullPath = Path.Combine(helper.OutputDir, "scripts\\ai");
                                    fullPath = Path.Combine(fullPath, flowChartManager.CurrentPath);
                                    string[] data = flowChartManager.CurrentPath.Split(new char[] { '\\' });

                                    flowChartMetaDataManager.RootPath = helper.OutputDir;
                                    flowChartMetaDataManager.FlowChartFullPath = fullPath;
                                    flowChartMetaDataManager.FlowChartShortPath = flowChartManager.CurrentPath;
                                    flowChartMetaDataManager.FlowChartName = data[data.Length - 1];
                                    flowChartMetaDataManager.MapName = data[0];

                                    break;
                                }
                        }

                        break;
                    }
            }

            return result;
        }
    }
}
