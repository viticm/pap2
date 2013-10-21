using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace DockingTest
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();

            dockPanel1.DocumentStyle = Fireball.Docking.DocumentStyles.DockingWindow;
        }

        protected override void OnLoad(EventArgs e)
        {
            base.OnLoad(e);


            MyDocument doc = new MyDocument();            

            doc.Show(dockPanel1, Fireball.Docking.DockState.DockLeft);

            for (int i = 0; i < 10; i++)
            {

                MyDocument doc2 = new MyDocument();

                doc2.Show(dockPanel1, Fireball.Docking.DockState.Document);
            }
        }
    }
}