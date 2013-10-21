using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using ScriptViewer;

namespace testScriptView
{
    public partial class testForm : Form
    {
        public testForm()
        {
            InitializeComponent();
        }

        private void testForm_Load(object sender, EventArgs e)
        {
            SqlConnection conn = new SqlConnection();
            conn.ConnectionString = "Server = jx3web; DataBase = s3design_test; Uid = s3design; Password = davidbowie;";
            ScriptViewer.frmScriptView fsv   = new ScriptViewer.frmScriptView(conn);
            fsv.OnNewWindow += new svEventsHandle(fsv_OnNewWindow);
            fsv.OpenWindow(7374, 206, null);
            fsv.Show();
            fsv.OpenWindow(7374, 206, null);
            fsv.Show();
        }

        private void fsv_OnNewWindow(object sender, ScriptViewer.svEventsArgs e)
        {
            
            //MessageBox.Show("new window");
        }

    }
}