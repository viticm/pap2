using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using AtlKGRecordeOperatorLib;

namespace RecordeOperator
{
    public partial class MainForm : Form
    {
        private BaseLib m_baseLib = new BaseLib();

        public MainForm()
        {
            Init();
            InitializeComponent();
        }

        ~MainForm()
        {
            UnInit();
        }

        public void UnInit()
        {
            m_baseLib.UnInit();
        }

        public void Init()
        {
            int nhWnd = this.Handle.ToInt32();
            m_baseLib.Init(nhWnd);
        }

        private void opentoolStripMenuItem_Click(object sender, EventArgs e)
        {
            openFileDialog.Filter = "RecordeOperator 文件 (*.rec)|*.rec|所有文件 (*.*)|*.*";

            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                string strOpenFileName = openFileDialog.FileName;
                openFileDialog.InitialDirectory = System.IO.Path.GetDirectoryName(strOpenFileName);

                OpenChildForm(strOpenFileName);
            }
        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {

        }

        private void saveAsToolStripMenuItem_Click(object sender, EventArgs e)
        {

        }

        private void closeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ChildForm Child = (ChildForm)this.ActiveMdiChild;
            if (Child == null)
                return;
            Child.Close();
        }

        private void OpenChildForm(string strOpenFileName)
        {
            bool IsOpen = false;

            for (int i = 0; i < MdiChildren.Length; i++)
            {
                if (MdiChildren[i].Name == strOpenFileName)
                {
                    MdiChildren[i].BringToFront();
                    IsOpen = true;
                    break;
                }
            }

            if (!IsOpen)
            {
                ChildForm Child = new ChildForm(this);
                Child.Name = strOpenFileName;
                Child.OpenFile(strOpenFileName);
                Child.Show();
            }
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            m_baseLib.UnInit();
            Application.Exit();
        }

        private void openToolStripButton_Click(object sender, EventArgs e)
        {
            opentoolStripMenuItem_Click(sender, e);
        }

        private void saveToolStripButton_Click(object sender, EventArgs e)
        {
            saveToolStripMenuItem_Click(sender, e);
        }
    }
}