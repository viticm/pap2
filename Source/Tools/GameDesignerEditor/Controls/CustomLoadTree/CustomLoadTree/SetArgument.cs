using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace CustomLoadTree
{
    public partial class SetArgument : Form
    {
        private float maxFlow; // 实际流量上限
        private float minFlow; // 实际流量下限

        public SetArgument()
        {
            InitializeComponent();
        }

        /// <summary>
        /// 流量上限属性
        /// </summary>
        public float MaxFlow
        {
            get
            {
                return maxFlow;
            }
        }

        /// <summary>
        /// 流量下限属性
        /// </summary>
        public float MinFlow
        {
            get
            {
                return minFlow;
            }
        }

        /// <summary>
        /// 允许字体变化属性
        /// </summary>
        public bool FontChangable
        {
            get
            {
                return checkBoxX1.Checked;
            }
        }

        /// <summary>
        /// 检查参数的有效值
        /// </summary>
        /// <returns>参数是否有效</returns>
        private bool CheckAvail()
        {
            string maxF = textBoxX1.Text;
            string minF = textBoxX2.Text;

            if(maxF == "") // 流量上限为空
            {
                MessageBox.Show("流量上限不能为空!");
                return false;
            }

            if(minF == "") //流量下限为空
            {
                MessageBox.Show("流量下限不能为空!");
                return false;
            }

            try
            {
                float maxFlow = float.Parse(maxF);
                float minFlow = float.Parse(minF);

                if(maxFlow < 0) // 流量上限为负
                {
                    MessageBox.Show("流量上限不能为负!");
                    return false;
                }

                if(minFlow < 0) // 流量下限为负
                {
                    MessageBox.Show("流量下限不能为负!");
                    return false;
                }

                if(maxFlow < minFlow) // 流量上限小于下限
                {
                    MessageBox.Show("流量上限不能小于下限!");
                    return false;
                }
            }
            catch(Exception ex)
            {
                MessageBox.Show("检查计算参数设置有效值时产生异常: " + ex.ToString());
                return false;
            }

            return true;
        }

        /// <summary>
        /// 用户选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            if(CheckAvail()) // 检查用户输入的有效性
            {
                maxFlow = float.Parse(textBoxX1.Text);
                minFlow = float.Parse(textBoxX2.Text);
                DialogResult = DialogResult.OK;
                this.Close();
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
        /// 用户选择允许字体变化
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void checkBoxX1_CheckedChanged(object sender, EventArgs e)
        {
            if(checkBoxX1.Checked)
            {
                MessageBox.Show("注意，对单元格字体的设置可能会消耗较多的处理时间！");
            }
        }
    }
}