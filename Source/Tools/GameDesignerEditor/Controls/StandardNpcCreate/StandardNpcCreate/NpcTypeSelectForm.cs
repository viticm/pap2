using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace StandardNpcCreate
{
    public partial class NpcTypeSelectForm : Form
    {
        private List<string> detailTypeList; // npc类型链表

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="detailTypeList">npc类型链表</param>
        /// <param name="titleText">窗体标题</param>
        public NpcTypeSelectForm(List<string> detailTypeList, string titleText)
        {
            this.detailTypeList = detailTypeList;
            InitializeComponent();
            Init();
            this.Text = titleText;
        }

        /// <summary>
        /// 选择编号
        /// </summary>
        public int SelectionIndex
        {
            get
            {
                return comboBoxEx1.SelectedIndex;
            }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            foreach(string s in detailTypeList)
            {
                comboBoxEx1.Items.Add(s);
            }

            comboBoxEx1.SelectedIndex = 0;
        }

        /// <summary>
        /// 选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.OK;
            this.Close();
        }
    }
}