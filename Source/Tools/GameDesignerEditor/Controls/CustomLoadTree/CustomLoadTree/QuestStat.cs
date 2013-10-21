using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;

namespace CustomLoadTree
{
    public partial class QuestStatForm : Form
    {
        protected DataTable table1 = new DataTable();
       
        public QuestStatForm()
        {           
            InitializeComponent();            
        }
        public QuestStatForm(TreeView treeview1)
        {
            InitializeComponent();

            InitTable1();
            FillTable(treeview1);

            dataGridView1.DataSource = table1;
        }
       

        protected void InitTable1()
        {
            table1.Columns.Add("SiteName", typeof(string));
            table1.Columns.Add("QType0", typeof(Int32));
            table1.Columns.Add("QType1", typeof(Int32));
            table1.Columns.Add("QType2", typeof(Int32));
            table1.Columns.Add("QType3", typeof(Int32));
            table1.Columns.Add("QType4", typeof(Int32));
            table1.Columns.Add("QType5", typeof(Int32));
            table1.Columns.Add("QType6", typeof(Int32));
            table1.Columns.Add("QType7", typeof(Int32));
            table1.Columns.Add("QType8", typeof(Int32));
            table1.Columns.Add("QType10", typeof(Int32));
            table1.Columns.Add("Total", typeof(Int32));            
        }

        protected void FillTable(TreeView treeview1)
        {
            foreach (TreeNode node1 in treeview1.Nodes)
            {
                int[] qtCnt = new int[11];//初始化为0
                if (node1.Parent == null && 9 == node1.ImageIndex)
                {
                    StatQuestType(node1, qtCnt);
                    AddTableRow(table1, node1.Text, qtCnt);
                }
            }
            AddLastRow(table1);

        }

        //(为了统计各场景任务数量）datatable添加一行
        protected void AddTableRow(DataTable table1, string sitename, int[] qtCnt)
        {
            DataRow addRow = table1.NewRow();
            addRow["SiteName"] = sitename;
            addRow["QType0"] = qtCnt[0];
            addRow["QType1"] = qtCnt[1];
            addRow["QType2"] = qtCnt[2];
            addRow["QType3"] = qtCnt[3];
            addRow["QType4"] = qtCnt[4];
            addRow["QType5"] = qtCnt[5];
            addRow["QType6"] = qtCnt[6];
            addRow["QType7"] = qtCnt[7];
            addRow["QType8"] = qtCnt[8];
            addRow["QType10"] = qtCnt[10];
            addRow["Total"] = qtCnt[0] + qtCnt[1] + qtCnt[2] + qtCnt[3] + qtCnt[4] + qtCnt[5] + qtCnt[6] + qtCnt[7] + qtCnt[8] + qtCnt[10];
            table1.Rows.Add(addRow);
        }
        protected void AddLastRow(DataTable table1)
        {
            DataRow addRow = table1.NewRow();
            addRow["SiteName"] = "总统计";
            int i = 0, columnsum = 0;
            for (i = 1; i < table1.Columns.Count; i++)
            {
                columnsum = 0;
                foreach (DataRow dr in table1.Rows)
                {
                    columnsum += (int)dr[i];
                }
                addRow[i] = columnsum;
            }
            table1.Rows.Add(addRow);
        }



        protected void StatQuestType(TreeNode node, int[] na)
        {
            foreach (TreeNode node1 in node.Nodes)
            {
                na[node1.ImageIndex]++;
                StatQuestType(node1, na);
            }
        }     

       
    }
}