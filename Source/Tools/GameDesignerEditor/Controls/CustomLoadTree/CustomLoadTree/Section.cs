using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace CustomLoadTree
{
    public partial class Section : Form
    {
        float[] data; // 区间上下限数组

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="data">返回的区间上下限数组</param>
        public Section(float[] data)
        {
            this.data = data;
            InitializeComponent();
        }

        /// <summary>
        /// 检查用户输入的有效性
        /// </summary>
        /// <returns>用户输入是否有效</returns>
        private bool ChechAvail()
        {
            string text1 = textBoxX1.Text;
            string text2 = textBoxX2.Text;
            float upValue = 0.00f; // 上界值
            float downValue = 0.00f; // 下界值

            if(text1 == "" || text2 == "") // 输入为空
            {
                MessageBox.Show("输入不能为空!");
                return false;
            }

            try // 检测数值是否有效
            {
                upValue = float.Parse(text1);
                downValue = float.Parse(text2);
            }
            catch(Exception ex)
            {
                MessageBox.Show("输入的数值无效: " + ex.ToString());
                return false;
            }

            if(upValue < downValue) // 上界小于下界
            {
                MessageBox.Show("上界值不能小于下界值!");
                return false;
            }

            data[0] = upValue;
            data[1] = downValue;
            return true;
        }

        /// <summary>
        /// 确定输入
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            if(ChechAvail()) // 检查输入的有效性
            {
                DialogResult = DialogResult.OK;
                this.Close();
            }      
        }

        /// <summary>
        /// 取消输入
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