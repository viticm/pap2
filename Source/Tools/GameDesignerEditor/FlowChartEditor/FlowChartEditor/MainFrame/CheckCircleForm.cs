using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using DevComponents.DotNetBar;

using FlowChartEditor.BaseGraphElement;
using GraphElementLibrary;

namespace FlowChartEditor
{
    public partial class CheckCircleForm : Office2007Form
    {
        private GraphManager graphManager; // 绘图管理器
        private List<List<SlotContainer>> circleList; // 当前循环的圈


        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="graphManager">绘图管理器</param>
        /// <param name="circleList">当前循环的圈</param>
        public CheckCircleForm(GraphManager graphManager, List<List<SlotContainer>> circleList)
        {
            this.graphManager = graphManager;
            this.circleList = circleList;
            InitializeComponent();
            Init();
        }

        /// <summary>
        /// 初始化
        /// </summary>
        private void Init()
        {
            this.Text = string.Format("循环有效性检测 有{0}个循环可能无法正常结束", circleList.Count);
            string circleString = string.Empty;
            foreach(List<SlotContainer> list in circleList)
            {
                circleString = string.Empty;
                foreach(SlotContainer slotContainer in list)
                {
                    circleString += slotContainer.ID + " -> ";
                }
                circleString = circleString + list[0].ID;
                listBox1.Items.Add(circleString);
            }
        }

        /// <summary>
        /// 关闭窗口
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX2_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.No;
            this.Close();
        }

        /// <summary>
        /// 窗体关闭事件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void CheckCircleForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            foreach(List<SlotContainer> list in circleList)
            {
                foreach(SlotContainer slotContainer in list)
                {
                    slotContainer.ShowID = false;
                }
            }
        }

        /// <summary>
        /// 选择查看循环
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void listBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            int index = listBox1.SelectedIndex;
            if (index != -1)
            {
                List<SlotContainer> currentList = circleList[index];
                List<GraphElement> list = new List<GraphElement>();
                foreach (SlotContainer slotContainer in currentList)
                {
                    slotContainer.ShowID = true;
                    list.Add(slotContainer);
                }

                graphManager.SelectGraphElements(list);
            }
        }
    }
}