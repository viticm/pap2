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
    public partial class Form1 : Form
    {
        public int userIndex = -1;

        private Hashtable userData = new Hashtable();

        public Form1()
        {
            InitializeComponent();
            Init();
        }

        private void Init()
        {
            DataTable tbl_useracl = Helper.GetDataTable("SELECT * FROM sys_useracl", Class1.Conn);
            foreach (DataRow row in tbl_useracl.Rows)
            {
                string username = row["userid"].ToString().Trim();
                int userindex = Convert.ToInt32(row["_index"]);
                userData[username] = userindex;

                this.comboBox1.Items.Add(username);
            }
        }

        private void button2_Click(object sender, EventArgs e) // 确定
        {
            string username = this.comboBox1.Text;
            if (username != string.Empty)
            {
                this.DialogResult = DialogResult.OK;
                this.userIndex = Convert.ToInt32(userData[username]);
                this.Close();
            }
        }

        private void button1_Click(object sender, EventArgs e) // 取消
        {
            this.DialogResult = DialogResult.Cancel;
            this.userIndex = -1;
            this.Close();
        }
    }
}