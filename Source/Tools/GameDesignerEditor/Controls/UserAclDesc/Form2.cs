using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Collections;

namespace UserAclDesc
{
    public partial class Form2 : Form
    {
        public ArrayList moduleIndices;
        private Hashtable allData = new Hashtable();
        private int userIndex = -1;

        public Form2(int userindex)
        {
            InitializeComponent();
            userIndex = userindex;
            Init();
        }

        private void Init()
        {
            string sql = string.Format("SELECT * FROM sys_modl_def");
            DataTable tbl_useracl = Helper.GetDataTable(sql, Class1.Conn);
            foreach (DataRow row in tbl_useracl.Rows)
            {
                string modulename = row["modelname_en"].ToString().Trim();
                int moduleid = Convert.ToInt32(row["id"]);
                allData[modulename] = moduleid;
            }

            sql = string.Format("SELECT * FROM sys_modl_def WHERE id NOT IN (SELECT moduleid FROM sys_useracl_desc WHERE userindex = {0})", userIndex);
            DataTable tbl_useracl_not_in = Helper.GetDataTable(sql, Class1.Conn);
            foreach (DataRow row in tbl_useracl_not_in.Rows)
            {
                string modulename = row["modelname_en"].ToString().Trim();
                this.listBox1.Items.Add(modulename);
            }

            sql = string.Format("SELECT * FROM sys_modl_def WHERE id IN (SELECT moduleid FROM sys_useracl_desc WHERE userindex = {0})", userIndex);
            DataTable tbl_useracl_this_user = Helper.GetDataTable(sql, Class1.Conn);
            foreach (DataRow row in tbl_useracl_this_user.Rows)
            {
                string modulename = row["modelname_en"].ToString().Trim();
                this.listBox2.Items.Add(modulename);
            }
        }

        private void button2_Click(object sender, EventArgs e) // 确定
        {
            this.moduleIndices = new ArrayList();
            foreach (object str in this.listBox2.Items)
            {
                moduleIndices.Add(allData[str.ToString()]);
            }
            this.DialogResult = DialogResult.OK;
            this.Close();
        }

        private void button1_Click(object sender, EventArgs e) // 取消
        {
            this.DialogResult = DialogResult.Cancel;
            this.moduleIndices = null;
            this.Close();
        }

        private void button3_Click(object sender, EventArgs e) // >>
        {
            if (this.listBox1.SelectedItem != null)
            {
                this.listBox2.Items.Add(this.listBox1.SelectedItem);
                this.listBox1.Items.Remove(this.listBox1.SelectedItem);
            }
        }

        private void button4_Click(object sender, EventArgs e) // <<
        {
            if (this.listBox2.SelectedItem != null)
            {
                this.listBox1.Items.Add(this.listBox2.SelectedItem);
                this.listBox2.Items.Remove(this.listBox2.SelectedItem);
            }
        }

        private void listBox1_DoubleClick(object sender, EventArgs e) // >>
        {
            button3_Click(this.button3, null);
        }

        private void listBox2_DoubleClick(object sender, EventArgs e) // <<
        {
            button4_Click(this.button4, null);
        }

        private void button5_Click(object sender, EventArgs e)// 全部添加
        {
            for (int i = this.listBox1.Items.Count - 1; i >= 0; i--)
            {
                this.listBox2.Items.Add(this.listBox1.Items[i]);
                this.listBox1.Items.RemoveAt(i);
            }
        }

        private void button6_Click(object sender, EventArgs e)// 全部删除
        {
            for (int i = this.listBox2.Items.Count - 1; i >= 0; i--)
            {
                this.listBox1.Items.Add(this.listBox2.Items[i]);
                this.listBox2.Items.RemoveAt(i);
            }
        }
    }
}