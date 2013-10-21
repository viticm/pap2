using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace StandardNpcCreate
{
    public partial class ConfigForm : Form
    {
        public ConfigForm()
        {
            InitializeComponent();
        }

        /// <summary>
        /// 是否更新插槽
        /// </summary>
        public bool UpdateSlot
        {
            get
            {
                return checkBoxX3.Checked;
            }
        }

        /// <summary>
        /// 是否更新动作
        /// </summary>
        public bool UpdateAnimation
        {
            get
            {
                return checkBoxX1.Checked;
            }
        }

        /// <summary>
        /// 是否更新技能
        /// </summary>
        public bool UpdateSkill
        {
            get
            {
                return checkBoxX2.Checked;
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

        /// <summary>
        /// 勾选更新动作
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void checkBoxX1_CheckedChanged(object sender, EventArgs e)
        {
            if (!checkBoxX1.Checked)
            {
                checkBoxX2.Checked = false;
                checkBoxX2.Enabled = false;
            }
            else
            {
                checkBoxX2.Enabled = true;
            }
        }
    }
}