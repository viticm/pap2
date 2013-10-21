using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace GameDesingerTools
{
    public partial class ImportInfo : Form
    {
        public string KeyField;
        public int BeginNum;
        public int EndNum;
        public bool All;

        public ImportInfo(string[] strColNames)
        {
            InitializeComponent();
            this.radioButton1.Checked = true;
            if (strColNames != null)
                foreach (string colname in strColNames)
                {
                    this.comboKey.Items.AddRange(new object[] { colname });
                }
        }

        public ImportInfo()
        {
            InitializeComponent();
        }

        private void buttonOK_Click(object sender, EventArgs e)
        {
            KeyField = this.comboKey.Text.Trim();
            try
            {
                BeginNum = Convert.ToInt32(this.editBegin.Text.Trim());
                EndNum = Convert.ToInt32(this.editEnd.Text.Trim());
            }
            catch
            {
                BeginNum = 0;
                EndNum = 0;
            }
            All = this.radioButton1.Checked;

            DialogResult = DialogResult.OK;
            Close();
        }

        private void buttonCancel_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            Close();
        }

        private void radioButton1_CheckedChanged(object sender, EventArgs e)
        {
            if (this.radioButton1.Checked == true)
            {
                this.groupBox1.Enabled = false;
            }
            else
            {
                this.groupBox1.Enabled = true;
            }
        }



    }
}