using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Collections;

using DevComponents.DotNetBar;
using DevComponents.AdvTree;

using DBManager;
using GraphElementLibrary;
using FlowChartEditor.BaseGraphElement;

namespace FlowChartEditor
{
    public partial class InnerChartReferenceForm : Office2007Form
    {
        private const string diagramClass = "AI图"; // 当前绘图类型
        private DataBaseManager dataBaseManager = DataBaseManager.GetDataBaseManager();
        private Dictionary<int, DiagramInfo> diagramInfoDictionary = new Dictionary<int, DiagramInfo>(); // 绘图数据缓存

        /// <summary>
        /// 构造函数
        /// </summary>
        public InnerChartReferenceForm()
        {
            InitializeComponent();
            Init();
        }

        class DiagramInfo
        {
            private int version;
            private List<SlotContainer> nodeList;

            public DiagramInfo(int version, List<SlotContainer> nodeList)
            {
                this.version = version;
                this.nodeList = nodeList;
            }

            public int Version
            {
                set
                {
                    version = value;
                }

                get
                {
                    return version;
                }
            }

            public List<SlotContainer> NodeList
            {
                set
                {
                    nodeList = value;
                }

                get
                {
                    return nodeList;
                }
            }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {            
            // 初始化数据列
            DataGridViewTextBoxColumn pathColumn = new DataGridViewTextBoxColumn();
            pathColumn.Name = "Path";
            pathColumn.HeaderText = "绘图路径";

            DataGridViewTextBoxColumn nodeIDColumn = new DataGridViewTextBoxColumn();
            nodeIDColumn.Name = "NodeID";
            nodeIDColumn.HeaderText = "结点ID";

            diagramInfoView.Columns.Add(pathColumn);
            diagramInfoView.Columns.Add(nodeIDColumn);
        }

        /// <summary>
        /// 刷新数据
        /// </summary>
        public void RefreshData()
        {
            innerChartTree.Nodes.Clear();
            diagramInfoView.Rows.Clear();

            // 初始化分类树
            string sqlString = string.Format("SELECT FullPath FROM {0} WHERE FullPath LIKE '子绘图%' AND Class = '{1}' AND IsFolder = 0",
                                             dataBaseManager.DiagramTableName, diagramClass);
            DataTable innerChartTable = dataBaseManager.GetDataTable(sqlString);

            foreach (DataRow dataRow in innerChartTable.Rows)
            {
                string innerChartName = dataRow["FullPath"].ToString().Replace("子绘图\\", "");
                Node innerChartNode = new Node();
                innerChartNode.Text = innerChartName;
                innerChartTree.Nodes.Add(innerChartNode);
            }
        }

        /// <summary>
        /// 查看子绘图结点
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bViewNode_Click(object sender, EventArgs e)
        {
            if (diagramInfoView.SelectedCells.Count > 0)
            {
                DataGridViewRow currentRow = diagramInfoView.SelectedCells[0].OwningRow;
                string chartID = currentRow.Tag as string;
                string nodeID = currentRow.Cells["NodeID"].Value as string;

                DocumentManager documentManager = DocumentManager.GetDocumentManager();
                documentManager.OpenDiagram(chartID);

                GraphManager graphManager = documentManager.CurrentFlowChartManager.CurrentGraphManager;
                DataManager dataManager = documentManager.CurrentFlowChartManager.CurrentDataManager;
                GraphElement graphElement = dataManager.FindGraphElementByID(int.Parse(nodeID));
                graphManager.SelectGraphElement(graphElement, true);

                this.Focus();
            }
            else
            {
                MessageBox.Show("请先选择要查看的子绘图！", "查看绘图",
                                MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 关闭窗体
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bClose_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            this.Close();
        }

        /// <summary>
        /// 查找引用
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bSearchReference_Click(object sender, EventArgs e)
        {
            Node currentNode = innerChartTree.SelectedNode;

            if (currentNode != null)
            {
                diagramInfoView.Rows.Clear();

                string innerChartName = currentNode.Text;                
                DataTable infoTable = dataBaseManager.GetDiagramInformation(diagramClass);

                DataRow[] rows = infoTable.Select("IsFolder = 0");
                ProgressForm progressForm = new ProgressForm(0, rows.Length);
                progressForm.Show();

                for (int i = 0; i < rows.Length; i++)
                {   
                    DataRow dataRow = rows[i];
                    string id = dataRow["ID"].ToString();
                    string path = dataRow["FullPath"].ToString();
                    string version = dataRow["Version"].ToString();
                    int diagramID = int.Parse(id);

                    progressForm.ShowProgress(i + 1, string.Format("检索绘图 {0} ...", path));

                    List<SlotContainer> nodeList = new List<SlotContainer>();

                    if (diagramInfoDictionary.ContainsKey(diagramID))
                    {
                        DiagramInfo diagramInfo = diagramInfoDictionary[diagramID];

                        if (diagramInfo.Version == int.Parse(version))
                        {
                            nodeList = diagramInfo.NodeList;
                        }
                        else
                        {
                            Hashtable diagramData = dataBaseManager.GetDiagramData(id) as Hashtable;

                            if (diagramData != null)
                            {
                                Hashtable graphData = diagramData["graphData"] as Hashtable;
                                nodeList = graphData["slotContainerList"] as List<SlotContainer>;
                                diagramInfo.Version = int.Parse(version);
                                diagramInfo.NodeList = nodeList;
                            }
                        }
                    }
                    else
                    {
                        Hashtable diagramData = dataBaseManager.GetDiagramData(id) as Hashtable;

                        if (diagramData != null && !string.IsNullOrEmpty(version))
                        {
                            Hashtable graphData = diagramData["graphData"] as Hashtable;
                            nodeList = graphData["slotContainerList"] as List<SlotContainer>;
                            DiagramInfo diagramInfo = new DiagramInfo(int.Parse(version), nodeList);
                            diagramInfoDictionary[diagramID] = diagramInfo;
                        }
                    }

                    foreach (SlotContainer node in nodeList)
                    {
                        if (node is InnerChart && node.Text == innerChartName)
                        {
                            diagramInfoView.Rows.Add(1);
                            DataGridViewRow newRow = diagramInfoView.Rows[diagramInfoView.Rows.Count - 1];
                            newRow.Cells["Path"].Value = path;
                            newRow.Cells["NodeID"].Value = node.ID.ToString();
                            newRow.Tag = id;
                        }
                    }                   
                }
            }
            else
            {
                MessageBox.Show("请先选择要查找引用的子绘图", "查找引用",
                                MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 窗体关闭事件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件发送者</param>
        private void InnerChartReferenceForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            e.Cancel = true;
            this.Visible = false;
        }
    }
}