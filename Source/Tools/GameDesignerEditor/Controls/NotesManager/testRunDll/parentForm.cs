using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;

namespace testRunDll
{
    public partial class parentForm : Form
    {
        public SqlConnection conn;
        private NotesManager.doit nm;
        public parentForm()
        {
            InitializeComponent();
            conn = new SqlConnection("Server = jx3web; DataBase = s3design_rtm; Uid = s3design; Password = ddd12345678;");
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }

        private void button1_Click(object sender, EventArgs e)
        {
            nm = new NotesManager.doit();
            panel1.Controls.Clear();
            panel1.Controls.Add(nm.Init(conn));
        }

        private void button2_Click(object sender, EventArgs e)
        {
            nm.saveAll();
        }

        private void button3_Click(object sender, EventArgs e)
        {
            MessageBox.Show(nm.hasChanged().ToString());
        }

        private void button4_Click(object sender, EventArgs e)
        {
            nm.reloadAll();
        }

        private void button5_Click(object sender, EventArgs e)
        {
            nm.reloadAll(textBox1.Text);
        }

    }
}