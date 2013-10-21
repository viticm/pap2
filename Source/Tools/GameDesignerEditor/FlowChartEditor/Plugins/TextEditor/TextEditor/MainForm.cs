using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using DevComponents.DotNetBar;

namespace TextEditor
{
    public partial class MainForm : Office2007Form
    {
        private string content; // 文本内容

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="content">文本内容</param>
        public MainForm(string content)
        {
            this.content = content;
            InitializeComponent();
            textBoxX1.Text = content;
            textBoxX1.Select(0, content.Length);
        }

        /// <summary>
        /// 文本内容
        /// </summary>
        public string Content
        {
            get
            {
                return content;
            }
        }

        /// <summary>
        /// 用户选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            content = textBoxX1.Text;

            if(content != "")
            {                
                DialogResult = DialogResult.OK;
                this.Close();
            }
            else
            {
                MessageBox.Show("输入的内容不能为空！", "输入检查", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 用户选择取消
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX2_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.No;
            this.Close();
        }

        /// <summary>
        /// 鼠标点击文本框
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void textBoxX1_MouseClick(object sender, MouseEventArgs e)
        {
            if (textBoxX1.Text == "请输入编辑内容") // 自动清空文本内容
            {
                textBoxX1.Text = "";
            }
        }
    }
}