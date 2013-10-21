using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using DevComponents.DotNetBar;

namespace GuideConfig
{
    public partial class AddNodeForm : Office2007Form
    {
        private string nodeTitle;
        private string nodeName;
        private string nodeDescription;

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="nodeTitle">标题</param>
        /// <param name="nodeName">名称</param>
        /// <param name="nodeDescription">描述</param>
        public AddNodeForm(string nodeTitle, string nodeName, string nodeDescription)
        {
            this.nodeTitle = nodeTitle;
            this.nodeName = nodeName;
            this.nodeDescription = nodeDescription;
            InitializeComponent();
            Init();
        }

        /// <summary>
        /// 名称
        /// </summary>
        public string NodeName
        {
            get
            {
                return nodeName;
            }
        }

        /// <summary>
        /// 描述
        /// </summary>
        public string NodeDescription
        {
            get
            {
                return nodeDescription;
            }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            this.Text = nodeTitle;
            nameBox.Text = nodeName;
            descriptionBox.Text = nodeDescription;
        }

        /// <summary>
        /// 选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bOK_Click(object sender, EventArgs e)
        {
            nodeName = nameBox.Text;
            nodeDescription = descriptionBox.Text;

            if (nodeName != "")
            {
                DialogResult = DialogResult.OK;
                this.Close();
            }
            else
            {
                MessageBox.Show("名称不能为空！", nodeTitle,
                                MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 选择取消
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bCancel_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            this.Close();
        }
    }
}