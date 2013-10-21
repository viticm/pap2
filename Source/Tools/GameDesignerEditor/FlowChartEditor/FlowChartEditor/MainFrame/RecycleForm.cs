using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;

using DevComponents.DotNetBar;

using DBManager;

namespace FlowChartEditor
{
    public partial class BackupForm : Office2007Form
    {        
        public BackupForm()
        {
            InitializeComponent();
            Init();
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            // 初始化分类树
            

            // 初始化数据列
            DataGridViewTextBoxColumn editorColumn = new DataGridViewTextBoxColumn();
            editorColumn.Name = "Editor";
            editorColumn.HeaderText = "编辑者";

            DataGridViewTextBoxColumn lastEditTimeColumn = new DataGridViewTextBoxColumn();
            lastEditTimeColumn.Name = "LastEditTime";
            lastEditTimeColumn.HeaderText = "最后编辑时间";

            infoView.Columns.Add(editorColumn);
            infoView.Columns.Add(lastEditTimeColumn);
        }

        /// <summary>
        /// 选择恢复
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bOK_Click(object sender, EventArgs e)
        {

        }

        /// <summary>
        /// 选择取消
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bCancel_Click(object sender, EventArgs e)
        {
            
        }

        /// <summary>
        /// 预览绘图
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bView_Click(object sender, EventArgs e)
        {

        }
    }
}