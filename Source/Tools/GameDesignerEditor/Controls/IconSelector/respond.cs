using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace IconSelector
{
    public partial class respond : Form
    {
        public string m_respond = string.Empty;

        public respond(string content)
        {
            InitializeComponent();
            this.label1.Text = content;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            m_respond = "yestoall";
            Close();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            m_respond = "yes";
            Close();
        }

        private void button3_Click(object sender, EventArgs e)
        {
            m_respond = "no";
            Close();
        }

        private void button4_Click(object sender, EventArgs e)
        {
            m_respond = "notoall";
            Close();
        }

        private void button5_Click(object sender, EventArgs e)
        {
            m_respond = "cancel";
            Close();
        }
    }
}