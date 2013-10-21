using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace CustomLoadTree
{
    public partial class ChooseNodeForm : Form
    {
        private TreeNode node;

        public ChooseNodeForm(TreeNode node)
        {
            this.node = node;
            InitializeComponent();
        }

        /// <summary>
        /// 用户选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            //ComputeForm cForm = new ComputeForm();
            //DialogResult result = cForm.ShowDialog();
            //this.Close();
        }

        /// <summary>
        /// 用户选择取消
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX2_Click(object sender, EventArgs e)
        {
            this.Close();
        }
    }
}