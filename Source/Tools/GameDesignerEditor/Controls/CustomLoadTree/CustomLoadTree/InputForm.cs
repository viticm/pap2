using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace CustomLoadTree
{
    public partial class InputForm : Form
    {
        private string titleText; // 标题文本
        private string inputText; // 输入文本

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="titleText">标题文本</param>
        /// <param name="inputText">输入文本</param>
        public InputForm(string titleText, string inputText)
        {
            this.titleText = titleText;
            this.inputText = inputText;
            InitializeComponent();
            Init();
        }

        /// <summary>
        /// 输入的文本
        /// </summary>
        public string InputText
        {
            get
            {
                return inputText;
            }
        }

        /// <summary>
        /// 初始化
        /// </summary>
        private void Init()
        {
            this.Text = titleText;
            textBoxX1.Text = inputText;
            textBoxX1.SelectionStart = 0;
            textBoxX1.SelectionLength = inputText.Length;
            textBoxX1.Focus();
        }

        /// <summary>
        /// 选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            inputText = textBoxX1.Text;
            if (inputText != "")
            {
                DialogResult = DialogResult.OK;
                this.Close();
            }
            else
            {
                MessageBox.Show("输入不能为空！", "文本输入", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 选择取消
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