using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace AIEnumEditor
{
    public partial class EnumValuesEditor : Form
    {
        private string oldValue; // 旧值
        private string newValue; // 新值

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="oldValue">旧值</param>
        public EnumValuesEditor(string oldValue)
        {
            this.oldValue = oldValue;
            InitializeComponent();
            Init();
        }

        /// <summary>
        /// 新值
        /// </summary>
        public string NewValue
        {
            get
            {
                return newValue;
            }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            // 初始化列数据
            DataGridViewTextBoxColumn chineseNameColumn = new DataGridViewTextBoxColumn();
            chineseNameColumn.Name = "ChineseName";
            chineseNameColumn.HeaderText = "中文名";
            DataGridViewTextBoxColumn englishNameColumn = new DataGridViewTextBoxColumn();
            englishNameColumn.Name = "EnglishName";
            englishNameColumn.HeaderText = "英文名";
            DataGridViewTextBoxColumn valueColumn = new DataGridViewTextBoxColumn();
            valueColumn.Name = "Value";
            valueColumn.HeaderText = "数值";

            dataGridViewX1.Columns.Add(chineseNameColumn);
            dataGridViewX1.Columns.Add(englishNameColumn);
            dataGridViewX1.Columns.Add(valueColumn);

            // 初始化行数据
            if(oldValue != null && oldValue != "")
            {
                string[] datas = oldValue.Split(new char[] { '\n' });
                int index = 0;

                foreach(string s in datas)
                {
                    string value = s.TrimEnd(new char[] { '\r' });
                    string[] values = value.Split(new char[] { ',' });
                    dataGridViewX1.Rows.Add(1);
                    DataGridViewRow newRow = dataGridViewX1.Rows[index];
                    newRow.Cells["ChineseName"].Value = values[0];
                    newRow.Cells["EnglishName"].Value = values[1];
                    newRow.Cells["Value"].Value = values[2];
                    index++;
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
            string value = "";
            bool valid = true;

            foreach(DataGridViewRow row in dataGridViewX1.Rows)
            {
                if(!row.IsNewRow) // 跳过新建的数据行
                {
                    string chineseName = row.Cells["ChineseName"].EditedFormattedValue as string;
                    string englishName = row.Cells["EnglishName"].EditedFormattedValue as string;
                    string values = row.Cells["Value"].EditedFormattedValue as string;

                    if (chineseName == null || chineseName == "" || englishName == null || englishName == "" ||
                        values == null || values == "")
                    {
                        MessageBox.Show("请输入完整的参数！", "枚举值列表编辑",
                            MessageBoxButtons.OK, MessageBoxIcon.Information);
                        valid = false;
                        break;
                    }

                    value += string.Format("{0},{1},{2}\r\n", chineseName, englishName, values);
                }
                
            }

            if(valid) // 输入有效
            {
                newValue = value.TrimEnd(new char[] { '\r', '\n' });
                DialogResult = DialogResult.OK;
                this.Close();
            }                        
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
    }
}