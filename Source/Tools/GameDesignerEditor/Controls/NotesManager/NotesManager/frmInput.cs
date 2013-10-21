using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace NotesManager
{
    public partial class frmInput : Form
    {
        public string input = "";
        public frmInput()
        {
            InitializeComponent();
        }

        private void buttonX1_Click(object sender, EventArgs e)
        {
            input = textBoxX1.Text;
            this.Hide();
        }

        public static string show_out(string defText)
        {
            frmInput fi = new frmInput();
            fi.input = defText;
            fi.textBoxX1.Text = defText;
            fi.ShowDialog();
            return (fi.input);
        }

        public static string show_out()
        {
            frmInput fi = new frmInput();
            fi.ShowDialog();
            return (fi.input);
        }

        private void textBoxX1_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                buttonX1_Click(sender, e);
            }
        }
    }
}