using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace ChooseItem
{
    public partial class ComputeDropRate : Form
    {
        private float finalDropRate = 0; // 最终掉落率
        private int luckyRate; // 幸运权值
        private int dropValue; // 掉落值

        public ComputeDropRate()
        {
            InitializeComponent();
        }

        /// <summary>
        /// 最终掉落率属性
        /// </summary>
        public float FinalDropRate
        {
            get
            {
                return finalDropRate;
            }
        }

        /// <summary>
        /// 幸运权值属性
        /// </summary>
        public int LuckyRate
        {
            get
            {
                return luckyRate;
            }

            set
            {
                luckyRate = value;
            }
        }

        /// <summary>
        /// 掉落值属性
        /// </summary>
        public int DropValue
        {
            get 
            {
                return dropValue;
            }

            set
            {
                dropValue = value;
            }
        }

        /// <summary>
        /// 计算最终掉落率
        /// </summary>
        private void ComputeFinalDropRate()
        {
            int luckyValue = 0; // 幸运值

            if(textBoxX1.Text == "")
            {
                MessageBox.Show("幸运值不能为空!");
                return;
            }

            try
            {
                luckyValue = int.Parse(textBoxX1.Text);
                // 最终掉落率 ＝(掉落值 + (幸运权值 × 幸运值)) / 掉落总值
                finalDropRate = (dropValue + ((float)luckyRate * luckyValue)) / 1000000;
            }
            catch(Exception ex)
            {
                MessageBox.Show("计算最终掉落率时产生异常: " + ex.ToString());
            }
        }

        /// <summary>
        /// 计算最终掉落率
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            ComputeFinalDropRate();
            textBoxX2.Text = finalDropRate.ToString();
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