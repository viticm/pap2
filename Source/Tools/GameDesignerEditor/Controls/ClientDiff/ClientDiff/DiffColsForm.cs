using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace ClientDiff
{
    public partial class DiffColsForm : Form
    {
        public DiffColsForm(List<string> diffCols1, List<string> diffCols2)
        {
            InitializeComponent();
            listBox1.Items.AddRange(diffCols1.ToArray());
            listBox1.Items.AddRange(diffCols2.ToArray());
            
        }
    }
}