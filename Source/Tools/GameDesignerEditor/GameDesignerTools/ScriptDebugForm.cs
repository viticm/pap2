using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace GameDesingerTools
{
    public partial class ScriptDebugForm : Form
    {
        public ScriptDebugForm()
        {
            InitializeComponent();
            foreach(Form frm in Application.OpenForms)
            {
                if (frm is MainForm)
                {
                    dotNetBarManager1.ParentForm = frm;
                    break;
                }
            }
        }

        private void ScriptDebugForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            e.Cancel = true;
            Hide();
        }

        private void dockContainerItem1_DoubleClick(object sender, EventArgs e)
        {
            //if (e.Button == MouseButtons.Right)
                OutputBox.Clear();
        }
    }
}