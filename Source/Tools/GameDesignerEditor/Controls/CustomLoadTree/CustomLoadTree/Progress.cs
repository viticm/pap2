using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace CustomLoadTree
{
    /// <summary>
    /// 进度条显示窗口
    /// </summary>
    public partial class Progress : Form
    {
        private int maxValue = 0; // 进度条的最大值
        private string titleText = "树结点遍历中";

        /// <summary>
        /// 构造函数
        /// </summary>
        public Progress()
        {
            InitializeComponent();
            this.StartPosition = FormStartPosition.CenterScreen; // 将窗口设置为屏幕正中位置
        }

        /// <summary>
        /// 标题文本属性
        /// </summary>
        public string TitleText
        {
            get
            {
                return titleText;
            }

            set
            {
                titleText = value;
            }
        }

        /// <summary>
        /// 设置进度条的最大值
        /// </summary>
        /// <param name="n">最大值</param>
        public void SetMaxValue(int n)
        {
            maxValue = n;
            progressBarX1.Minimum = 0;
            progressBarX1.Maximum = n;
        }

        /// <summary>
        /// 设置进度条的当前值
        /// </summary>
        /// <param name="n">当前值</param>
        public void SetValue(int n)
        {
            this.Text = string.Format("{2} {0}/{1}", n, maxValue, titleText);
            progressBarX1.Value = n;
            if (maxValue == n)
            {
                this.Close();
            }
        }

        /// <summary>
        /// 关闭窗口事件处理
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Progress_FormClosing(object sender, FormClosingEventArgs e)
        {
            // 隐藏窗口
            this.Visible = false;
            e.Cancel = true;
        }
    }
}