using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using DevComponents.DotNetBar;
using DevComponents.DotNetBar.Controls;

namespace DesignerSceneEditor
{
    public partial class ProgressForm : Form
    {
        public Label LoadingItemName { get { return label1; } set { label1 = value; } }

        public ProgressBarX LoadingItemProgress { get { return progressbar1; } set { progressbar1 = value; } }

        public int ProgressPercent;

        public ProgressForm()
        {
            InitializeComponent();
        }

        public void ShowProgress(string name, int percent)
        {
            label1.Text = name;
            progressbar1.Value = percent;
        }
    }
}