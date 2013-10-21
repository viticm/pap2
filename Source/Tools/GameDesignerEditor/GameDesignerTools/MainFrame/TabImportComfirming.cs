using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace GameDesingerTools
{
    public partial class TabImportComfirming : Form
    {
        public TabImportComfirming()
        {
            InitializeComponent();
        }

        public TabImportComfirming(string strToShow)
        {
            InitializeComponent();
            label1.Text += ("\r\n" + strToShow);
        }

        private void button1_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Yes; // yes
            Close();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.OK; // yes to all
            Close();
        }

        private void button3_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.No; // no
            Close();
        }

        private void button4_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Ignore; // no to all
            Close();
        }

        private void button5_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel; // cancel
            Close();
        }
    }
}