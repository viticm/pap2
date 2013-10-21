using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace FlowChartVarialbe
{
    public partial class AddVariableForm : DevComponents.DotNetBar.Office2007Form
    {
        public AddVariableForm()
        {
            InitializeComponent();
        }
        public AddVariableForm(string strLabelTip, string strVariableName)
        {
            InitializeComponent();
            lbTip.Text = strLabelTip;
            tbxVariable.Text = strVariableName;
        }

        public string GetNewVariableName()
        {
            return tbxVariable.Text;
        }
        

        private void btnOK_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.OK;
        }

        private void btnCancle_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.Cancel;
        }
    }
}