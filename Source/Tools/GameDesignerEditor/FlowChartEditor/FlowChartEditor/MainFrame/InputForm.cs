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
    public partial class InputForm : Office2007Form
    {
        private string inputText; // 输入的文本

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="title">窗体的标题</param>
        /// <param name="inputText">输入的文本</param>
        public InputForm(string title, string inputText)
        {
            this.inputText = inputText;
            InitializeComponent();
            this.Text = title;
            textBoxX1.Text = inputText;
            textBoxX1.Select(0, inputText.Length);
        }

        /// <summary>
        /// 输入的文本属性
        /// </summary>
        public string InputText
        {
            get
            {
                return inputText;
            }
        }

        /// <summary>
        /// 用户选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            inputText = textBoxX1.Text;

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
        /// <param name="e">事件</param>
        private void buttonX2_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            this.Close();
        }
    }
}