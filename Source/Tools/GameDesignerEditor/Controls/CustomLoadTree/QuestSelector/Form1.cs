using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using CustomLoadTree;
using System.Data.SqlClient;

namespace QuestSelector
{
    public partial class Form1 : Form
    {
        private int m_oldid;
        public int ID;
        private TreeView m_tv = null;

        public Form1(SqlConnection Conn, int id)
        {
            InitializeComponent();
            this.DialogResult = DialogResult.Cancel;
            this.ID = this.m_oldid = id;
            Class1 c = new Class1("tbl_quests", "_Cat", this.panel1, Conn);
            this.m_tv = c.LoadTreePlugin(ID);
        }

        private void panelEx1_Click(object sender, EventArgs e)
        {

        }

        private void buttonX1_Click(object sender, EventArgs e)
        {
            if (m_tv != null)
            {
                this.ID = Convert.ToInt32(((object[])(m_tv.SelectedNode.Tag))[0]);
                if (this.ID == -2)
                    return;
                this.DialogResult = DialogResult.OK;
            }
            this.Close();
        }

        private void buttonX2_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void Form1_Resize(object sender, EventArgs e)
        {
            this.panel1.Dock = DockStyle.Fill;
        }
    }
}