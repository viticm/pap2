using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using DevComponents.DotNetBar;

namespace FlowChartEditor
{
    public partial class ListForm : Office2007Form
    {
        private List<string> inputList; // 输入字符串的链表
        private string inputText; // 输入的字符串

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="title">窗体的标题</param>
        /// <param name="inputList">输入字符串的链表</param>
        public ListForm(string title, List<string> inputList)
        {
            this.inputList = inputList;
            InitializeComponent();
            this.Text = title;
            Init();
        }

        /// <summary>
        /// 输入的字符串属性
        /// </summary>
        public string InputText
        {
            set
            {
                inputText = value;
            }

            get
            {
                return inputText;
            }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            foreach(string s in inputList)
            {
                comboBoxEx1.Items.Add(s);
            }
        }

        /// <summary>
        /// 用户选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            inputText = comboBoxEx1.Text;
            if(inputText != "")
            {
                DialogResult = DialogResult.OK;
                this.Close();
            }
            else
            {
                MessageBox.Show("输入不能为空！", "输入有效性检查", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 用户选择取消
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX2_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            this.Close();
        }
    }
}