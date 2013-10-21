using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace StandardNpcCreate
{
    public partial class NotifyForm : Form
    {
        /// <summary>
        /// 构造函数
        /// </summary>
        public NotifyForm()
        {
            InitializeComponent();
        }

        /// <summary>
        /// 是否更新已有动作
        /// </summary>
        public bool UpdateExistAnimation
        {
            get
            {
                return checkBoxX2.Checked;
            }
        }

        /// <summary>
        /// 是否记录当前选择
        /// </summary>
        public bool RecordSelection
        {
            get
            {
                return checkBoxX1.Checked;
            }
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