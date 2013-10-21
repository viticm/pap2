using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace testQuestSelector
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            QuestSelector.ClassQuestSelector c = new QuestSelector.ClassQuestSelector();
            int r = c.Show(new System.Data.SqlClient.SqlConnection("Server = jx3web; DataBase = s3design_debug; Uid = s3design; Password = davidbowie;"), 1605);
            this.Text = r.ToString();
        }
    }
}