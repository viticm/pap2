using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using XSJ.Sword3.Editor.Studio;

namespace XSJ.Sword3.Editor.Studio
{
    public partial class frmLogin : Form
    {
        public frmLogin()
        {
            InitializeComponent();
        }

        private void groupBox1_Enter(object sender, EventArgs e)
        {

        }

        private void frmLogin_Load(object sender, EventArgs e)
        {
            
        }

        private void btnOK_Click(object sender, EventArgs e)
        {
            App.AppVariables.g_dataprovider.Init(
                    App.AppVariables.g_connstring.Replace("%s1", txtUsername.Text).Replace("%s2", txtPassword.Text));
            string errmsg = string.Empty;
            string stackinfo = string.Empty;
            bool bret = App.AppVariables.g_dataprovider.TryConnect(out errmsg,out stackinfo);
            if(true == bret)
            {
                App.AppVariables.g_logon = true;
                frmMain frmmain = new frmMain();
                frmmain.Show();
                this.Hide();
            }
            else
            {
                MessageBox.Show(errmsg, "µÇÂ¼Ê§°Ü!", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            this.Close();
        }
    }
}