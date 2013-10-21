using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using BaseExp;

namespace ConditionLine
{
    public partial class SelectionForm : DevComponents.DotNetBar.Office2007Form
    {
        public Exp ResultExp = null;

        public SelectionForm(Exp selExp, List<Exp> list)
        {
            this.DialogResult = DialogResult.Cancel;
            InitializeComponent();
            this.listBox1.Items.AddRange(list.ToArray());
            listBox1.SelectedItem = selExp;
        }

        private void buttonX1_Click(object sender, EventArgs e)
        {
            if(listBox1.SelectedItem != null)
            {
                this.ResultExp = listBox1.SelectedItem as Exp;
                this.DialogResult = DialogResult.OK;
                this.Close();
            }
        }

        private void buttonX2_Click(object sender, EventArgs e)
        {

        }

    }
}