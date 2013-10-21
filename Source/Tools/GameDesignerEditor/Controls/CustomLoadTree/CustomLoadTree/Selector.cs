using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace CustomLoadTree
{
    public partial class Selector : Form
    {
        float[] data; // 程序数据
        private List<string> selectionList = new List<string>(); // 选项列表
        private List<KeyValuePair<string, string>> valueList = new List<KeyValuePair<string,string>>(); // 选项键值对列表

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="data">数据链表</param>
        public Selector(float[] data)
        {
            this.data = data;
            InitializeComponent();
            FillList();
        }

        /// <summary>
        /// 填充链表
        /// </summary>
        private void FillList()
        {
            selectionList.Add("普通");
            selectionList.Add("高级");
            selectionList.Add("精英");
            valueList.Add(new KeyValuePair<string, string>("普通", "1.00"));
            valueList.Add(new KeyValuePair<string, string>("高级", "1.30"));
            valueList.Add(new KeyValuePair<string, string>("精英", "1.50"));            
        }

        /// <summary>
        /// 选择取消
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX2_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.No;
            this.Close();
        }

        /// <summary>
        /// 选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            string value = comboBoxEx1.Text;

            if(value == "") // 输入为空
            {
                MessageBox.Show("输入不能为空!");
                return;
            }

            if(selectionList.Contains(value)) // 输入是选项中的数据
            {
                foreach(KeyValuePair<string, string> k in valueList)
                {
                    if(value == k.Key)
                    {
                        value = k.Value;
                    }
                }
            }

            try
            {
                float result = float.Parse(value);
                data[0] = result;
                DialogResult = DialogResult.OK;
                this.Close();
            }
            catch(Exception ex)
            {
                MessageBox.Show("输入值无效: " + ex.ToString());
            }
        }
    }
}