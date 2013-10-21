using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using DevComponents.DotNetBar;
using DevComponents.AdvTree;
using System.Threading;
using DataElementLibrary;
using System.Collections;
using DataInteractor;

namespace AIDiagramPlugin
{
    public partial class StateNodeForm : Office2007Form
    {
        private AI_State m_state = null; // 当前状态
        private FlowChartInteractor flowChartInteractor; // 绘图中间对象
        private bool editSuccess = false; // 是否编辑成功
        private int graphElementID; // 当前编辑图元的ID

        /// <summary>
        /// 构造函数
        /// </summary>
        public StateNodeForm()
        {
            InitializeComponent();            
        }

        /// <summary>
        /// 当前编辑图元的ID
        /// </summary>
        public int GraphElementID
        {
            set
            {
                graphElementID = value;
            }
        }

        /// <summary>
        /// 是否编辑成功
        /// </summary>
        public bool EditSuccess
        {
            get
            {
                return editSuccess;
            }
        }

        /// <summary>
        /// 刷新数据
        /// </summary>
        public void Reload(AI_State state)
        {
            editSuccess = false;
            allEventTree.Nodes.Clear();
            registedEventTree.Nodes.Clear();

            this.m_state = state;
            flowChartInteractor = Interactor.GetInteractor().CurrentFlowChartInteractor;

            // 初始化名称
            if (string.IsNullOrEmpty(state.Ename))
            {
                state.Ename = string.Format("State{0}", graphElementID.ToString());
            }

            txtName.Text = state.Name;

            // 初始化所有事件
            foreach (AI_Event ai_event in EventTable.EventItems)
            {
                // 取消已注册的事件
                if (!state.EventList.Contains(ai_event))
                {
                    Node node = new Node();
                    node.Text = ai_event.ToString();
                    node.Tag = ai_event;
                    if (int.Parse(ai_event.eventid) < 1000) // 系统事件
                    {
                        node.Image = imageList1.Images[0];
                    }
                    else
                    {
                        node.Image = imageList1.Images[2];
                    }
                    allEventTree.Nodes.Add(node);
                }
            }

            // 初始化自定义事件
            List<AI_Event> customEventList = flowChartInteractor.CustomTable["CustomEventList"] as List<AI_Event>;

            if (customEventList != null)
            {
                foreach (AI_Event aiEvent in customEventList)
                {
                    // 去掉已注册的事件
                    if (!state.EventList.Contains(aiEvent))
                    {
                        Node node = new Node();
                        node.Text = aiEvent.ToString();
                        node.Tag = aiEvent;
                        node.Image = imageList1.Images[2];
                        allEventTree.Nodes.Add(node);
                    }
                }
            }

            // 初始化选中的事件         
            foreach (AI_Event aiEvent in state.EventList)
            {
                Node node = new Node();
                node.Text = aiEvent.ToString();
                node.Tag = aiEvent;

                if (int.Parse(aiEvent.eventid) < 1000) // 系统事件
                {
                    node.Image = imageList1.Images[0];
                }
                else
                {
                    node.Image = imageList1.Images[2];
                }

                registedEventTree.Nodes.Add(node);
            }

            // 切换焦点
            txtName.Focus();
        }

        /// <summary>
        /// 选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            // 完整性检查
            if (txtName.Text != "")
            {
                // 修改名称
                this.m_state.Name = txtName.Text;

                // 更新可用事件
                this.m_state.EventList.Clear();
                foreach (Node node in registedEventTree.Nodes)
                {
                    this.m_state.EventList.Add(node.Tag as AI_Event);
                }

                editSuccess = true;
                this.DialogResult = DialogResult.OK;
                this.Close();
            }
            else
            {
                MessageBox.Show("状态名不能为空！", "编辑状态结点", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }                                   
        }

        /// <summary>
        /// 选择取消
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX2_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.Cancel;
            this.Close();
        }

        /// <summary>
        /// 添加事件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void btnAdd_Click(object sender, EventArgs e)
        {
            Node node = allEventTree.SelectedNode;

            if (node != null)
            {
                allEventTree.Nodes.Remove(node);
                registedEventTree.Nodes.Add(node);
            }
        }

        /// <summary>
        /// 删除事件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void btnDel_Click(object sender, EventArgs e)
        {
            Node node = registedEventTree.SelectedNode;

            if (node != null)
            {
                registedEventTree.Nodes.Remove(node);
                allEventTree.Nodes.Add(node);
            }
        }        

        /// <summary>
        /// 新建自定义事件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX3_Click(object sender, EventArgs e)
        {
            string eventName = textBoxX1.Text;

            if (eventName != "")
            {
                List<AI_Event> customEventList = flowChartInteractor.CustomTable["CustomEventList"] as List<AI_Event>;

                if (customEventList == null)
                {
                    customEventList = new List<AI_Event>();
                }

                bool nameExist = false;

                foreach (AI_Event aiEvent in customEventList)
                {
                    if (aiEvent.Name == eventName)
                    {
                        nameExist = true;
                        break;
                    }
                }

                if (nameExist) // 事件名已经存在
                {
                    MessageBox.Show("该名称的事件已经存在！", "新建自定义事件", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
                else
                {
                    string customEventIndexString = flowChartInteractor.CustomTable["CustomEventIndex"] as string;
                    int customEventIndex = 1;
                    if(customEventIndexString != null)
                    {
                        customEventIndex = int.Parse(customEventIndexString);
                    }
                    int eventID = customEventIndex + 2000; // ID2000以上的为脚本内自定义事件

                    AI_Event aiEvent = new AI_Event();
                    aiEvent.Name = eventName;
                    aiEvent.Ename = "CustomEvent";
                    aiEvent.DBID = "0";
                    aiEvent.eventid = eventID.ToString();
                    customEventList.Add(aiEvent);
                    customEventIndex++;

                    Node node = new Node();
                    node.Text = aiEvent.ToString();
                    node.Tag = aiEvent;
                    node.Image = imageList1.Images[0];
                    allEventTree.Nodes.Add(node);
                    allEventTree.SelectedNode = node;

                    flowChartInteractor.CustomTable["CustomEventList"] = customEventList;
                    flowChartInteractor.CustomTable["CustomEventIndex"] = customEventIndex.ToString(); 
                }
            }
            else
            {
                MessageBox.Show("请先输入自定义事件的名称！", "新建自定义事件", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 删除自定义事件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX4_Click(object sender, EventArgs e)
        {
            Node currentNode = allEventTree.SelectedNode;

            if (currentNode != null)
            {
                AI_Event aiEvent = currentNode.Tag as AI_Event;

                if (int.Parse(aiEvent.eventid) > 2000) // 检查是否是绘图级的自定义事件
                {
                    List<AI_Event> customEventList = flowChartInteractor.CustomTable["CustomEventList"] as List<AI_Event>;
                    customEventList.Remove(aiEvent);
                    allEventTree.Nodes.Remove(currentNode);
                }
                else
                {
                    MessageBox.Show("只能删除本图内创建的自定义事件！", "删除自定义事件", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
            }
            else
            {
                MessageBox.Show("请先选择要删除的自定义事件！", "删除自定义事件", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 窗体关闭事件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void StateNodeForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            e.Cancel = true;
            this.Visible = false;
        }

        /// <summary>
        /// 所有事件切换
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void advTree1_AfterNodeSelect(object sender, AdvTreeNodeEventArgs e)
        {
            Node currentNode = allEventTree.SelectedNode;

            if (currentNode != null)
            {
                AI_Event aiEvent = currentNode.Tag as AI_Event;
                string description = aiEvent.Info;

                if (string.IsNullOrEmpty(description))
                {
                    description = aiEvent.Name;
                }

                informationBox.Text = description;
            }
        }

        /// <summary>
        /// 注册事件切换
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void advTree2_AfterNodeSelect(object sender, AdvTreeNodeEventArgs e)
        {
            Node currentNode = registedEventTree.SelectedNode;

            if (currentNode != null)
            {
                AI_Event aiEvent = currentNode.Tag as AI_Event;
                string description = aiEvent.Info;

                if (string.IsNullOrEmpty(description))
                {
                    description = aiEvent.Name;
                }

                informationBox.Text = description;
            }
        }

        /// <summary>
        /// 重命名自定义事件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX5_Click(object sender, EventArgs e)
        {
            Node eventNode = allEventTree.SelectedNode;
            
            if (eventNode != null)
            {
                string eventName = textBoxX1.Text;  
 
                if (eventName != "")
                {
                    AI_Event aiEvent = eventNode.Tag as AI_Event;

                    if (aiEvent != null && int.Parse(aiEvent.eventid) > 2000)
                    {
                        aiEvent.Name = eventName;
                        eventNode.Text = aiEvent.ToString();
                    }
                    else
                    {
                        MessageBox.Show("只能重命名本图内创建的自定义事件！", "重命名自定义事件", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    }
                }
                else
                {
                    MessageBox.Show("请先输入自定义事件的名称！", "重命名自定义事件", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }                                
            }
            else
            {                
                MessageBox.Show("请先选择要重命名的自定义事件！", "重命名自定义事件", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }
    }
}