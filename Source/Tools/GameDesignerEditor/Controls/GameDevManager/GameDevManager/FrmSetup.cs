using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace GameDevManager
{
    public partial class FrmSetup : Form
    {
        private FrmMain m_f;

        public FrmSetup(FrmMain f)
        {
            this.m_f = f;
            InitializeComponent();
            CD1.Filter = "³ÌÐòÎÄ¼þ(*.exe)|*.exe";
        }

        private void button3_Click(object sender, EventArgs e)
        {
            CD1.FileName = textBox1.Text;
            CD1.ShowDialog();
            if (CD1.FileName != "")
                textBox1.Text = CD1.FileName;
        }

        private void button4_Click(object sender, EventArgs e)
        {
            CD1.FileName = textBox2.Text;
            CD1.ShowDialog();
            if (CD1.FileName != "")
                textBox2.Text = CD1.FileName;
        }

        private void button5_Click(object sender, EventArgs e)
        {
            CD1.FileName = textBox3.Text;
            CD1.ShowDialog();
            if (CD1.FileName != "")
                textBox3.Text = CD1.FileName;
        }

        private void btnOK_Click(object sender, EventArgs e)
        {
            FrmMain.aFileNames[0] = textBox1.Text;
            FrmMain.aFileNames[1] = textBox2.Text;
            FrmMain.aFileNames[2] = textBox3.Text;
            FrmMain.aFileNames[3] = textBox4.Text;
            FrmMain.aFileNames[4] = checkBox1.Checked.ToString().ToLower();
            this.m_f.SaveSetting();
            this.Close();
                 }

        private void FrmSetup_Load(object sender, EventArgs e)
        {
            textBox1.Text = FrmMain.aFileNames[0];
            textBox2.Text = FrmMain.aFileNames[1];
            textBox3.Text = FrmMain.aFileNames[2];
            textBox4.Text = FrmMain.aFileNames[3];
            checkBox1.Checked = FrmMain.aFileNames[4] == "true";
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            CD1.FileName = textBox4.Text;
            CD1.ShowDialog();
            if (CD1.FileName != "")
                textBox4.Text = CD1.FileName;
        }
    }
}