/*===================================
 * Purpose:
 * Created By: WilsonWei
 * Created On: 2007-2-8
 * (c)Kingsoft Corporation
 * Comments:
 * 
 * 
 * 
 * ====================================*/

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Microsoft.Win32;

namespace XSJ.Sword3.Editor.Studio
{
    public partial class frmSettings : Form
    {
        public frmSettings()
        {
            InitializeComponent();
            txtOutputFolder.MouseHover += new EventHandler(txtOutputFolder_MouseHover);
        }

        void txtOutputFolder_MouseHover(object sender, EventArgs e)
        {
            toolTip1.Show(txtOutputFolder.Text, txtOutputFolder);
        }

        private void btnOK_Click(object sender, EventArgs e)
        {
            _doSave();
            this.Close();
        }

        private void _doSave()
        {
            this.txtOutputFolder.Text = this.folderBrowserDialog1.SelectedPath;
            RegistryKey subkey = Application.UserAppDataRegistry.CreateSubKey("ShopStudio");
            subkey.SetValue("OutputDir", this.txtOutputFolder.Text);
            App.AppVariables.OutputDir = this.txtOutputFolder.Text;
        }

        private void btnSelFolder_Click(object sender, EventArgs e)
        {
            DialogResult dr = this.folderBrowserDialog1.ShowDialog();
            if (dr == DialogResult.OK)
            {
                _doSave();
            }
        }

        private void txtOutputFolder_TextChanged(object sender, EventArgs e)
        {

        }

        private void frmSettings_Load(object sender, EventArgs e)
        {
            RegistryKey subkey = Application.UserAppDataRegistry.OpenSubKey("ShopStudio");
            if (null == subkey) return;
            App.AppVariables.OutputDir = subkey.GetValue("OutputDir", string.Empty).ToString();
            txtOutputFolder.Text = App.AppVariables.OutputDir;
            txtOutputFolder.KeyUp += new KeyEventHandler(txtOutputFolder_KeyUp);
        }

        void txtOutputFolder_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Escape)
            {
                this.Close();
            }
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            this.Close();
        }
    }
}