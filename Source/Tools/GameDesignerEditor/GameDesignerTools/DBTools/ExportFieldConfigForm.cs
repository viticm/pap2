using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;

namespace GameDesingerTools
{
    public partial class ExportFieldConfigForm : Form
    {
        private List<string> fieldList;
        private string excludeFieldString;
        private bool selectAll = true;

        public ExportFieldConfigForm(List<string> fieldList, string excludeFieldString)
        {
            this.fieldList = fieldList;
            this.excludeFieldString = excludeFieldString;
            InitializeComponent();
            Init();
        }

        /// <summary>
        /// 不导出的字段的配置字符串
        /// </summary>
        public string ExcludeFieldString
        {
            get
            {
                return excludeFieldString;
            }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            List<string> excludeFieldList = new List<string>();

            if (excludeFieldString != "")
            {
                string[] tempArray = excludeFieldString.Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);            
                excludeFieldList.AddRange(tempArray);
            }
            

            for (int i = 0; i < fieldList.Count; i++)
            {
                checkedListBox1.Items.Add(fieldList[i]);

                if (excludeFieldList.Contains(fieldList[i]))
                {
                    checkedListBox1.SetItemChecked(i, true);
                }
            }
        }

        /// <summary>
        /// 选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            StringBuilder configString = new StringBuilder();

            for (int i = 0; i < checkedListBox1.Items.Count; i++)
            {
                if (checkedListBox1.GetItemChecked(i))
                {
                    configString.Append(string.Format("{0},", checkedListBox1.Items[i].ToString()));
                }
            }

            excludeFieldString = configString.ToString().TrimEnd(new char[] { ',' });

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
            DialogResult = DialogResult.OK;
            this.Close();
        }

        /// <summary>
        /// 全部选中/全部取消
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX3_Click(object sender, EventArgs e)
        {            
            for (int i = 0; i < checkedListBox1.Items.Count; i++)
            {
                checkedListBox1.SetItemChecked(i, selectAll);
            }

            selectAll = !selectAll;

            if (selectAll)
            {               
                buttonX3.Text = "全部选中";
            }
            else
            {
                buttonX3.Text = "全部取消";
            }
        }
    }
}