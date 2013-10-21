using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace GameDesingerTools
{
    public partial class NameInput : Form
    {

        string m_strInput;
        public string StringInput { get { return m_strInput; } set { m_strInput = value; } }

        public NameInput()
        {
            InitializeComponent();
        }

        private void bOK_Click(object sender, EventArgs e)
        {
            StringInput = this.textBoxX1.Text.Trim();
            DialogResult = DialogResult.OK;
            Close();
        }

        private void bCancel_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            Close();
        }

        private void textBoxX1_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                StringInput = this.textBoxX1.Text.Trim();
                DialogResult = DialogResult.OK;
                Close();
            }
            else if (e.KeyCode == Keys.Escape)
            {
                DialogResult = DialogResult.Cancel;
                Close();
            }
        }
    }
}