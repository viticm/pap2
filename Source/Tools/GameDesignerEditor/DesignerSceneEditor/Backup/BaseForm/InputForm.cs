using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace DesignerSceneEditor
{
	public partial class InputForm: Form
	{
        private string inputText;

		public InputForm(string inputText, string titleText)
		{
			InitializeComponent();
            textBoxX1.Text = inputText;
            textBoxX1.SelectAll();
            this.Text = titleText;
            textBoxX1.Focus();
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
        /// 选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
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
                MessageBox.Show("输入不能为空！", "文本输入",
                    MessageBoxButtons.OK, MessageBoxIcon.Information);
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