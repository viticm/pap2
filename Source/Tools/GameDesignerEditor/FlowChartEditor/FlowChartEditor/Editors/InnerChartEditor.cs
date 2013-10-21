using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;
using System.Windows.Forms;

using DataElementLibrary;
using GraphElementLibrary;
using FlowChartEditor.BaseGraphElement;
using DBManager;

namespace FlowChartEditor
{
    [Serializable]
    public class InnerChartEditor : DataElement
    {
        private string innerChartPath = "";

        [NonSerialized]
        private List<GraphElement> graphElementList; // 图元链表

        [NonSerialized]
        private Hashtable interfaceNodeTable; // 接口结点数据表

        [NonSerialized]
        private Hashtable graphTable; // 图元数据表

        /// <summary>
        /// 图元链表
        /// </summary>
        public List<GraphElement> GraphElementList
        {
            get
            {
                return graphElementList;
            }
        }

        /// <summary>
        /// 获取接口结点
        /// </summary>
        /// <param name="interfaceID">结点ID</param>
        /// <returns>接口结点</returns>
        public InterfaceGraphElement GetInterfaceNode(string interfaceID)
        {
            InterfaceGraphElement interfaceGraphElement = null;

            if (interfaceNodeTable != null)
            {
                interfaceGraphElement = interfaceNodeTable[interfaceID] as InterfaceGraphElement;
            }

            return interfaceGraphElement;
        }

        /// <summary>
        /// 获取数据元
        /// </summary>
        /// <param name="graphElement">图元</param>
        /// <returns>数据元</returns>
        public DataElement GetDataElement(GraphElement graphElement)
        {
            DataElement dataElement = null;

            if (graphTable != null)
            {
                dataElement = graphTable[graphElement] as DataElement;
            }

            return dataElement;
        }

        /// <summary>
        /// 编辑数据
        /// </summary>
        /// <param name="o">数据元的数据</param>
        /// <return>是否编辑成功</return>
        public override bool EditData(Hashtable table)
        {
            bool editSuccess = false;

            InnerChartForm innerChartForm = new InnerChartForm(innerChartPath);
            
            if (innerChartForm.ShowDialog() == DialogResult.OK)
            {                
                innerChartPath = innerChartForm.InnerChartPath;
                graphElementList = innerChartForm.GraphElementList;
                graphTable = innerChartForm.GraphTable;
                interfaceNodeTable = innerChartForm.InterfaceNodeTable;
                this.text = innerChartPath;      
          
                // 更新数据
                UpdateData(innerChartForm.InSlotInfo, innerChartForm.OutSlotInfo);
                
                InnerChart innerChart = DocumentManager.GetDocumentManager().CurrentFlowChartManager.CurrentDataManager.FindGraphElementByID(id) as InnerChart;

                // 更新插槽数量
                UpdateSlotCount(innerChart, innerChartForm.InSlotCount, innerChartForm.OutSlotCount, false);

                List<int> inIDList = new List<int>();
                List<int> outIDList = new List<int>();

                foreach (string nodeID in innerChartForm.InSlotInfo.Keys)
                {                  
                    inIDList.Add(int.Parse(nodeID));
                }            

                foreach (string nodeID in innerChartForm.OutSlotInfo.Keys)
                {                    
                    outIDList.Add(int.Parse(nodeID));                    
                }            
    
                foreach (InterfaceGraphElement interfaceNode in interfaceNodeTable.Values)
                {
                    interfaceNode.Tag = id.ToString();
                }

                // 刷新插槽信息
                RefreshSlotInfo(innerChart, inIDList, outIDList, innerChartForm.InSlotInfo, innerChartForm.OutSlotInfo);                

                editSuccess = true;
            }

            return editSuccess;
        }

        /// <summary>
        /// 更新数据
        /// </summary>
        /// <param name="inSlotInfo">入口插槽数据</param>
        /// <param name="outSlotInfo">出口插槽数据</param>
        private void UpdateData(Hashtable inSlotInfo, Hashtable outSlotInfo)
        {
            Hashtable infoTable = new Hashtable();
            infoTable["inSlotInfo"] = inSlotInfo;
            infoTable["outSlotInfo"] = outSlotInfo;
            infoTable["innerChartPath"] = innerChartPath;
            data = infoTable;
        }

        /// <summary>
        /// 更新插槽数量
        /// </summary>
        /// <param name="innerChart">子绘图结点</param>
        /// <param name="inSlotCount">入口插槽数量</param>
        /// <param name="outSlotCount">出口插槽数量</param>
        /// <param name="showMessage">是否显示消息</param>
        private void UpdateSlotCount(InnerChart innerChart, int inSlotCount, int outSlotCount, bool showMessage)
        {
            // 更新插槽信息
            if (innerChart.InSlotCount != inSlotCount || innerChart.OutSlotCount != outSlotCount)
            {
                innerChart.InSlotCount = inSlotCount;
                innerChart.OutSlotCount = outSlotCount;
                innerChart.LockInSlot();
                innerChart.LockOutSlot();

                if (showMessage)
                {
                    MessageBox.Show(string.Format("子绘图结点{0} (ID:{1})的插槽参数更新，需要重新连接！", innerChart.Text, innerChart.ID), 
                                    "子绘图更新", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
            }
        }

        /// <summary>
        /// 刷新插槽信息
        /// </summary>
        /// <param name="innerChart">子绘图结点</param>
        /// <param name="inIDList">入口插槽ID链表</param>
        /// <param name="outIDList">出口插槽ID链表</param>
        /// <param name="inSlotInfo">入口插槽信息</param>
        /// <param name="outSlotInfo">出口插槽信息</param>
        private void RefreshSlotInfo(InnerChart innerChart, List<int> inIDList, List<int> outIDList, Hashtable inSlotInfo, Hashtable outSlotInfo)
        {
            StringBuilder descriptionInfo = new StringBuilder();

            descriptionInfo.AppendLine("<bold>入口插槽");            

            // 刷新插槽信息
            inIDList.Sort();
            List<SlotGraphElement> slotList = innerChart.GetInSlotList();

            for (int i = 0; i < inIDList.Count; i++)
            {
                string nodeID = inIDList[i].ToString();
                string nodeName = inSlotInfo[nodeID] as string;
                slotList[i].Tag = nodeID;
                slotList[i].TooltipText = nodeName;

                descriptionInfo.AppendLine(string.Format("{0}：{1}", i, nodeName));                
            }

            outIDList.Sort();
            slotList = innerChart.GetOutSlotList();

            descriptionInfo.AppendLine("<split>");
            descriptionInfo.AppendLine("<bold>出口插槽");            

            for (int i = 0; i < outIDList.Count; i++)
            {
                string nodeID = outIDList[i].ToString();
                string nodeName = outSlotInfo[nodeID] as string;
                slotList[i].Tag = nodeID;

                descriptionInfo.AppendLine(string.Format("{0}：{1}", i, nodeName));                
            }

            // 刷新提示信息
            tooltipText = descriptionInfo.ToString();
        }

        /// <summary>
        /// 初始化子绘图数据表
        /// </summary>        
        private void InitGraphTable()
        {
            if (!string.IsNullOrEmpty(innerChartPath))
            {
                DataBaseManager dataBaseManager = DataBaseManager.GetDataBaseManager();
                string fullPath = string.Format(@"子绘图\{0}", innerChartPath);
                Hashtable infoTable = dataBaseManager.GetDiagramDataByPath(fullPath, "AI图") as Hashtable;

                if (infoTable != null)
                {
                    Hashtable logicData = infoTable["logicData"] as Hashtable;
                    graphTable = logicData["graphTable"] as Hashtable;
                }     
            }                   
        }

        /// <summary>
        /// 重读数据
        /// </summary>
        /// <param name="table">当前数据</param>
        /// <return>是否重读成功</return>
        public override bool ReloadData(Hashtable table)
        {            
            Hashtable dataInfo = data as Hashtable;

            if (dataInfo != null)
            {
                innerChartPath = dataInfo["innerChartPath"] as string;

                // 初始化绘图数据表
                InitGraphTable();

                if (graphTable != null)
                {
                    int inSlotCount = 0;
                    int outSlotCount = 0;
                    Hashtable inSlotInfo = new Hashtable();
                    Hashtable outSlotInfo = new Hashtable();
                    graphElementList = new List<GraphElement>();
                    interfaceNodeTable = new Hashtable();

                    List<int> inIDList = new List<int>();
                    List<int> outIDList = new List<int>();

                    foreach (GraphElement graphElement in graphTable.Keys)
                    {
                        if (graphElement is InterfaceGraphElement)
                        {
                            InterfaceGraphElement interfaceGraphElement = graphElement as InterfaceGraphElement;
                            InterfaceNodeEditor interfaceEditor = graphTable[graphElement] as InterfaceNodeEditor;

                            if (interfaceEditor.IsInput)
                            {                                
                                inSlotInfo[interfaceEditor.ID.ToString()] = interfaceEditor.InterfaceName;
                                inIDList.Add(interfaceGraphElement.ID);
                                inSlotCount++;
                            }
                            else
                            {
                                outSlotInfo[interfaceEditor.ID.ToString()] = interfaceEditor.InterfaceName;
                                outIDList.Add(interfaceGraphElement.ID);
                                outSlotCount++;
                            }

                            interfaceGraphElement.Tag = id.ToString();
                            interfaceNodeTable[interfaceEditor.ID.ToString()] = graphElement;
                        }
                        else
                        {
                            graphElementList.Add(graphElement);
                        }
                    }                    

                    InnerChart innerChart = DocumentManager.GetDocumentManager().CurrentFlowChartManager.CurrentDataManager.FindGraphElementByID(id) as InnerChart;
                    
                    // 更新数据
                    UpdateData(inSlotInfo, outSlotInfo);

                    // 更新插槽数量
                    UpdateSlotCount(innerChart, inSlotCount, outSlotCount, true);

                    // 更新插槽信息
                    RefreshSlotInfo(innerChart, inIDList, outIDList, inSlotInfo, outSlotInfo);
                }
            }                                       

            return true;
        }

        /// <summary>
        /// 获取输出的参数
        /// </summary>
        /// <returns>输出的参数</returns>
        public override Hashtable GetOutputArgs()
        {
            return null;
        }
    }
}
