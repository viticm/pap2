using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Collections;

using DevComponents.DotNetBar;

using GraphElementLibrary;
using DBManager;
using FlowChartEditor.BaseGraphElement;

namespace FlowChartEditor
{
    public partial class InnerChartForm : Office2007Form
    {
        private DocumentManager documentManager;
        private Hashtable idTable = new Hashtable();
        private Hashtable diagramTable = new Hashtable();
        private string innerChartPath = ""; // 子绘图路径
        private int inSlotCount = 0; // 入口插槽数量
        private int outSlotCount = 0; // 出口插槽数量
        private Hashtable inSlotInfo = new Hashtable(); // 入口插槽信息
        private Hashtable outSlotInfo = new Hashtable(); // 出口插槽信息
        private Hashtable interfaceNodeTable = new Hashtable(); // 接口结点数据表
        private List<GraphElement> graphElementList = new List<GraphElement>(); // 图元链表
        private Hashtable graphTable = new Hashtable(); // 图元数据表        

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="innerChartPath">子绘图路径</param>
        public InnerChartForm(string innerChartPath)
        {
            this.innerChartPath = innerChartPath;
            InitializeComponent();
            Init();
        }

        /// <summary>
        /// 图元数据表
        /// </summary>
        public Hashtable GraphTable
        {
            get
            {
                return graphTable;
            }
        }

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
        /// 接口结点数据表
        /// </summary>
        public Hashtable InterfaceNodeTable
        {
            get
            {
                return interfaceNodeTable;
            }
        }

        /// <summary>
        /// 入口插槽信息
        /// </summary>
        public Hashtable InSlotInfo
        {
            get
            {
                return inSlotInfo;
            }
        }

        /// <summary>
        /// 出口插槽信息
        /// </summary>
        public Hashtable OutSlotInfo
        {
            get
            {
                return outSlotInfo;
            }
        }
        
        /// <summary>
        /// 入口插槽数量
        /// </summary>
        public int InSlotCount
        {
            get
            {
                return inSlotCount;
            }
        }

        /// <summary>
        /// 出口插槽数量
        /// </summary>
        public int OutSlotCount
        {
            get
            {
                return outSlotCount;
            }
        }

        /// <summary>
        /// 子绘图路径
        /// </summary>
        public string InnerChartPath
        {
            get
            {
                return innerChartPath;
            }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            documentManager = DocumentManager.GetDocumentManager();
            TreeView currentTree = documentManager.CurrentTree;
            TreeNode node = currentTree.Nodes[0].Nodes["子绘图"];

            innerChartBox.Items.Clear();

            if (node != null)
            {
                foreach (TreeNode innerChartNode in node.Nodes)
                {
                    innerChartBox.Items.Add(innerChartNode.Text);
                    idTable[innerChartNode.Text] = innerChartNode.Tag;
                }
            }

            innerChartBox.Text = innerChartPath;
        }

        /// <summary>
        /// 选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bOK_Click(object sender, EventArgs e)
        {
            innerChartPath = innerChartBox.Text;

            if (innerChartPath != "")
            {
                string id = idTable[innerChartPath].ToString();
                Hashtable infoTable = diagramTable[id] as Hashtable;
                 
                if (infoTable != null)
                {
                    Hashtable logicData = infoTable["logicData"] as Hashtable;
                    graphTable = logicData["graphTable"] as Hashtable;

                    foreach (GraphElement graphElement in graphTable.Keys)
                    {
                        if (graphElement is InterfaceGraphElement)
                        {
                            InterfaceGraphElement interfaceGraphElement = graphElement as InterfaceGraphElement;
                            InterfaceNodeEditor interfaceEditor = graphTable[graphElement] as InterfaceNodeEditor;
                            
                            if (interfaceEditor.IsInput)
                            {
                                inSlotInfo[interfaceEditor.ID.ToString()] = interfaceEditor.InterfaceName;
                                inSlotCount++;
                            }
                            else
                            {
                                outSlotInfo[interfaceEditor.ID.ToString()] = interfaceEditor.InterfaceName;
                                outSlotCount++;
                            }

                            interfaceNodeTable[interfaceEditor.ID.ToString()] = graphElement;                            
                        }
                        else // 记录所有的非接口结点
                        {
                            graphElementList.Add(graphElement);
                        }
                    }                    
                }

                DialogResult = DialogResult.OK;
                this.Close();
            }
            else
            {
                MessageBox.Show("请先选择子绘图！", "子绘图编辑",
                                MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 选择取消
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bCancel_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            this.Close();
        }

        /// <summary>
        /// 查看绘图
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bView_Click(object sender, EventArgs e)
        {
            innerChartPath = innerChartBox.Text;

            if (innerChartPath != "")
            {
                string nodePath = string.Format(@"子绘图\{0}", innerChartPath);
                documentManager.OpenDiagramInPath(nodePath, "AIChart");
                this.Close();
            }            
        }

        /// <summary>
        /// 选择绘图
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void innerChartBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            innerChartPath = innerChartBox.Text;
            string id = idTable[innerChartPath].ToString();
            Hashtable infoTable = null;
            lDescription.Text = "子绘图描述";

            if (diagramTable[id] == null)
            {
                DataBaseManager dataBaseManager = DataBaseManager.GetDataBaseManager();
                infoTable = dataBaseManager.GetDiagramData(id) as Hashtable;
                diagramTable[id] = infoTable;
            }
            else
            {
                infoTable = diagramTable[id] as Hashtable;
            }
            
            if (infoTable != null)
            {
                Hashtable customData = infoTable["customData"] as Hashtable;

                if (customData != null)
                {
                    string description = customData["description"] as string;

                    if (description != null)
                    {
                        lDescription.Text = description;
                    }
                }                
            }
        }
    }
}