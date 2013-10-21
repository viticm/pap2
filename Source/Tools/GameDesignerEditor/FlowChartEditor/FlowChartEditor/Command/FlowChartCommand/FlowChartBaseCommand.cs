using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;
using System.Windows.Forms;
using FlowChartEditor.BaseGraphElement;
using GraphElementLibrary;

namespace FlowChartEditor.Command.FlowChartCommand
{
    class FlowChartBaseCommand:Command
    {
        protected DocumentManager documentManager; // 文档管理器实例
        protected GraphManager graphManager; // 绘图管理器实例
        protected DataManager dataManager; // 数据管理器实例
        protected FlowChartManager flowChartManager; // 流程图管理器实例

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="data">当前对象</param>
        /// <param name="description">命令的描述</param>
        public FlowChartBaseCommand(object data, string description):
            base(data, description)
        {
            graphManager = data as GraphManager;
            flowChartManager = graphManager.CurrentFlowChartManager;
            dataManager = flowChartManager.CurrentDataManager;
            documentManager = DocumentManager.GetDocumentManager();

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
            Helper helper = Helper.GetHelper();
            GraphManager graphManager = data as GraphManager;
            FlowChartManager flowChartManager = graphManager.CurrentFlowChartManager;
            DataManager dataManager = flowChartManager.CurrentDataManager;            
            bool deleteSuccess = false; // 是否删除成功            

            // 执行逻辑操作
            LogicBaseManager logicManager = helper.GetLogicManager(flowChartManager.CurrentChartMode);            
            logicManager.DoLogicOperation(flowChartManager, LogicType.BeforeDelete, graphElement); 

            if (graphElement is SlotContainer) // 要删除的图元是插槽容器
            {
                SlotContainer slotContainer = graphElement as SlotContainer;
                List<GraphElement> graphElementList = new List<GraphElement>();

                // 解除绑定连接线控制点                
                foreach (ConnectorContainer line in slotContainer.GetConnectedLine())
                {
                    graphElementList.Add(line);
                }

                slotContainer.UnBind();

                // 执行逻辑操作
                logicManager.DoLogicOperation(flowChartManager, LogicType.AfterDelete, graphElementList);

                graphManager.SlotContainerList.Remove(slotContainer); // 从插槽容器索引链表中删除
                deleteSuccess = true;
            }
            else if (graphElement is ConnectorContainer) // 要删除的图元是连接线控制点容器
            {
                ConnectorContainer connectorContainer = graphElement as ConnectorContainer;
                List<GraphElement> graphElementList = new List<GraphElement>();

                SlotContainer inSlotContainer = connectorContainer.InSlotContainer;
                if (inSlotContainer != null)
                {
                    connectorContainer.UnbindInSlotContainer();
                }              

                SlotContainer outSlotContainer = connectorContainer.OutSlotContainer;
                if (outSlotContainer != null)
                {
                    graphElementList.Add(outSlotContainer);
                    connectorContainer.UnbindOutSlotContainer();
                }

                // 执行逻辑操作
                logicManager.DoLogicOperation(flowChartManager, LogicType.AfterDelete, graphElementList);

                graphManager.ConnectorContainerList.Remove(connectorContainer); // 从连接线控制点索引容器中删除
                deleteSuccess = true;
            }
            else if (graphElement is ConnectorGraphElement) // 要删除的图元是连接线控制点
            {
                ConnectorGraphElement connector = graphElement as ConnectorGraphElement;
                ConnectorContainer connectorContainer = connector.Line;
                List<GraphElement> graphElementList = new List<GraphElement>();

                SlotContainer inSlotContainer = connectorContainer.InSlotContainer;
                if (inSlotContainer != null)
                {
                    connectorContainer.UnbindInSlotContainer();
                }

                SlotContainer outSlotContainer = connectorContainer.OutSlotContainer;
                if (outSlotContainer != null)
                {
                    graphElementList.Add(outSlotContainer);
                    connectorContainer.UnbindOutSlotContainer();
                }

                // 执行逻辑操作
                logicManager.DoLogicOperation(flowChartManager, LogicType.AfterDelete, graphElementList);

                graphManager.ConnectorContainerList.Remove(connectorContainer); // 从连接线控制点索引容器中删除
                deleteSuccess = true;
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
            
            SlotContainer slotContainer;
            SlotContainer newSlotContainer;
            ConnectorContainer line;
            ConnectorContainer newLine;
            ICloneable cloneData;

            foreach (object o in dataTable.Keys)
            {
                GraphElement graphElement = o as GraphElement;

                if (graphElement is SlotContainer) // 插槽容器
                {
                    slotContainer = graphElement as SlotContainer;
                    newSlotContainer = slotContainer.Clone();
                    graphTable[slotContainer] = newSlotContainer;
                    cloneData = dataTable[slotContainer] as ICloneable;

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
                int index;
                slotContainer = connectorContainer.InSlotContainer;

                if (slotContainer != null
                    && graphElementList.Contains(slotContainer))
                {
                    index = slotContainer.GetBindedOutSlotIndex(connectorContainer);
                    newSlotContainer = graphTable[slotContainer] as SlotContainer;
                    newLine = graphTable[connectorContainer] as ConnectorContainer;

                    newSlotContainer.BindOutLine(newLine, index); 
                }

                slotContainer = connectorContainer.OutSlotContainer;

                if (slotContainer != null
                    && graphElementList.Contains(slotContainer))
                {
                    index = slotContainer.GetBindedInSlotIndex(connectorContainer);
                    newSlotContainer = graphTable[slotContainer] as SlotContainer;
                    newLine = graphTable[connectorContainer] as ConnectorContainer;

                    newSlotContainer.BindInLine(newLine, index);
                }
            }            

            return copyTable;
        }                

        /// <summary>
        /// 在命令执行前保存对象数据
        /// </summary>
        /// <param name="o">当前对象</param>
        protected override void SaveBeforeExecute(object o)
        {
            if (flowChartManager.SupportCommand) // 优化性能
            {
                dataBeforeExecute.SaveData(o);
            }            
        }

        /// <summary>
        /// 在命令执行后保存对象数据
        /// </summary>
        /// <param name="o">当前对象</param>
        protected override void SaveAfterExecute(object o)
        {
            if (flowChartManager.SupportCommand) // 优化性能
            {
                dataAfterExecute.SaveData(o);
            }            
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
            Hashtable args = o as Hashtable;
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
            Hashtable args = o as Hashtable;
            flowChartManager.SetArguments(args);
        }
    }
}
