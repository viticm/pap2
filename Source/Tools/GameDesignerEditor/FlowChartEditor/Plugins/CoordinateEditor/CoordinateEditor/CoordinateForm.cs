using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace CoordinateEditor
{
    public partial class CoordinateForm : Form
    {
        string cordinate; // 坐标

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="cordinate">当前坐标</param>
        public CoordinateForm(string cordinate)
        {
            this.cordinate = cordinate;
            InitializeComponent();
            Init();
        }

        /// <summary>
        /// 当前坐标
        /// </summary>
        public string Cordinate
        {
            set
            {
                cordinate = value;
            }

            get
            {
                return cordinate;
            }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            if(cordinate != null && cordinate != "") // 输入不为空
            {
                string[] data = cordinate.Split(new char[] { ',' });
                integerInput1.Value = int.Parse(data[0]);
                integerInput2.Value = int.Parse(data[1]);
                integerInput3.Value = int.Parse(data[2]);
            }
        }

        /// <summary>
        /// 确认输入
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            int x = integerInput1.Value;
            int y = integerInput2.Value;
            int z = integerInput3.Value;            
            cordinate = string.Format("{0},{1},{2}", x, y, z);
            DialogResult = DialogResult.OK;
        }

        /// <summary>
        /// 取消输入
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