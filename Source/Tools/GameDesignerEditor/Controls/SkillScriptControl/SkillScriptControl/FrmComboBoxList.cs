using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Collections;


namespace SkillScriptControl
{
    public partial class FrmComboBoxList : Form
    {
        public string strResult = "";

        public FrmComboBoxList(string strCode, string strText)
        {
            InitializeComponent();
            comboBoxEx1.Items.Clear();
            ArrayList al_ret = SkillScriptControl.SSControl.GetColList(strCode);
            foreach(string strLine in  al_ret)
            {
                comboBoxEx1.Items.Add("tSkillData[dwSkillLevel]." + strLine);
            }
            comboBoxEx1.Text = strText;
        }

        private void btnOK_Click(object sender, EventArgs e)
        {
            strResult = comboBoxEx1.Text;
            this.DialogResult = DialogResult.OK;
            this.Visible = false;
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            strResult = null;
            this.Visible = false;
        }

    }
}