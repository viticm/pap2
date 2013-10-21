using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Collections;

namespace GreyHoundEditor
{
    public partial class PreViewForm : Form
    {

        /// <summary>
        /// 构造函数
        /// </summary>
        public PreViewForm()
        {
            InitializeComponent();
            InitData();
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void InitData()
        {
            DataGridViewTextBoxColumn idColumn = new DataGridViewTextBoxColumn();
            idColumn.Name = "idColumn";
            idColumn.HeaderText = "id";

            DataGridViewTextBoxColumn nameColumn = new DataGridViewTextBoxColumn();
            nameColumn.Name = "nameColumn";
            nameColumn.HeaderText = "name";

            DataGridViewTextBoxColumn middleMapColumn = new DataGridViewTextBoxColumn();
            middleMapColumn.Name = "middleMapColumn";
            middleMapColumn.HeaderText = "middlemap";

            DataGridViewTextBoxColumn backgroundMusicColumn = new DataGridViewTextBoxColumn();
            backgroundMusicColumn.Name = "backgroundMusicColumn";
            backgroundMusicColumn.HeaderText = "backgroundMusicColumn";

            dataGridView1.Columns.Add(idColumn);
            dataGridView1.Columns.Add(nameColumn);
            dataGridView1.Columns.Add(middleMapColumn);
            dataGridView1.Columns.Add(backgroundMusicColumn);
        }

        /// <summary>
        /// 显示行数据
        /// </summary>
        /// <param name="dataTable">行数据哈希表</param>
        public void ShowData(Hashtable dataTable)
        {
            string[] data;
            int index = 0;
            foreach(string s in dataTable.Keys)
            {
                data = dataTable[s] as string[];
                if(data != null)
                {
                    dataGridView1.Rows.Add();
                    dataGridView1.Rows[index].Cells["idColumn"].Value = data[0];
                    dataGridView1.Rows[index].Cells["nameColumn"].Value = data[1];
                    dataGridView1.Rows[index].Cells["middleMapColumn"].Value = data[2];
                    dataGridView1.Rows[index].Cells["backgroundMusicColumn"].Value = data[3];
                    index++;
                }  
            }
        }
    }
}