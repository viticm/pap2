using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace ShopItemIO
{
    public partial class ProgressForm : Form
    {
        private int minValue; // 进度条最小值
        private int maxValue; // 进度条最大值

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="minValue">进度条最小值</param>
        /// <param name="maxValue">进度条最大值</param>
        public ProgressForm(int minValue, int maxValue)
        {
            this.minValue = minValue;
            this.maxValue = maxValue;
            InitializeComponent();
            Init();
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            progressBarX1.Minimum = minValue;
            progressBarX1.Maximum = maxValue;
        }

        /// <summary>
        /// 显示进度
        /// </summary>
        /// <param name="progressValue">进度值</param>
        /// <param name="progressText">进度文本</param>
        public void ShowProgress(int progressValue, string progressText)
        {
            progressBarX1.Value = progressValue;
            labelX1.Text = progressText;
            Application.DoEvents();

            if (progressValue >= maxValue)
            {
                this.Close();
            }
        }
    }
}