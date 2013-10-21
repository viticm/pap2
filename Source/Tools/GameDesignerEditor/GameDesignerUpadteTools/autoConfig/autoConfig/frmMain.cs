using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;

namespace autoConfig
{
    public partial class frmMain : Form
    {
        public frmMain(string[] fileList, string strText)
        {
            InitializeComponent();
            this.Text = strText;
            foreach(string strFile in fileList)
            {
                this.ckList.Items.Add(strFile, false);
            }
        }

        private void cmdDel_Click(object sender, EventArgs e)
        {
            if (this.ckList.CheckedItems.Count == 0)
            {
                MessageBox.Show("请选择文件", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            foreach(string delFile in this.ckList.CheckedItems)
            {

                if (File.Exists(delFile))
                {
                    try
                    {
                        File.Delete(delFile);
                    }
                    catch
                    {
                        MessageBox.Show("无法删除" + delFile, "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                        continue;
                    }

                }
                if (Directory.Exists(delFile))
                {
                    try
                    {
                        Directory.Delete(delFile);
                    }
                    catch
                    {
                        continue;
                    }
                }
            }
            this.Close();
        }

        private void cmdCancel_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void cmdAll_Click(object sender, EventArgs e)
        {
            for(int i = 0; i < ckList.Items.Count; i++)
            {
                ckList.SetItemChecked(i, true);
            }
        }
    }
}