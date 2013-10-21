using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace MapLogicalCompare
{
    public partial class ProgressForm : Form
    {
        private int maxValue = 1;

        public ProgressForm(int maxValue)
        {
            this.maxValue = maxValue;
            InitializeComponent();
            progressBarX1.Maximum = maxValue;
        }

        public int MaxValue
        {
            get
            {
                return maxValue;
            }
        }

        public void Progress(int currentValue, string currentText)
        {
            labelX1.Text = currentText;
            progressBarX1.Value = currentValue;
        }
    }
}