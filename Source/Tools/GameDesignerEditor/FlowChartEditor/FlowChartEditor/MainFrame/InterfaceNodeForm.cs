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
    public partial class InterfaceNodeForm : Office2007Form
    {
        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="interfaceName">接口名称</param>
        /// <param name="isInput">是否是输入类型</param>
        public InterfaceNodeForm(string interfaceName, bool isInput)
        {
            InitializeComponent();

            // 初始化控件显示
            nameBox.Text = interfaceName;
            bInput.Checked = isInput;
            bOutput.Checked = !isInput;
        }        

        /// <summary>
        /// 接口名称
        /// </summary>
        public string InterfaceName
        {
            get
            {
                return nameBox.Text;
            }
        }

        /// <summary>
        /// 是否是输入类型
        /// </summary>
        public bool IsInput
        {
            get
            {
                return bInput.Checked;
            }
        }

        /// <summary>
        /// 选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {            
            if (nameBox.Text != "" && (bInput.Checked || bOutput.Checked)) // 输入有效
            {
                DialogResult = DialogResult.OK;
                this.Close();
            }
            else
            {
                MessageBox.Show("参数不能为空！", "接口编辑",
                                MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 选择取消
        /// </summary>
        /// <param name="sender">事件发送和则</param>
        /// <param name="e">事件参数</param>
        private void buttonX2_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            this.Close();
        }
    }
}