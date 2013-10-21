using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace GameDesingerTools.DBTools
{
    public partial class TypeForm : Form
    {
        private string typeName; // 类型名称

        public TypeForm()
        {
            InitializeComponent();
        }

        /// <summary>
        /// 类型名称属性
        /// </summary>
        public string TypeName 
        {
            get
            {
                return typeName;
            }
        }

        /// <summary>
        /// 用户选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            if(textBoxX1.Text == "")
            {
                MessageBox.Show("类型名称不能为空!");
                return;
            }
            typeName = textBoxX1.Text;
            DialogResult = DialogResult.OK;
            this.Close();
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
    }
}