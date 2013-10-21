using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using Fireball.Windows.Forms;

namespace CustomButtonTest
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();

            CustomButtonStyle style2 = new CustomButtonStyle(customButton2.Width, customButton2.Height,ButtonBorder.RoundRectangle);
            customButton2.Style = style2;
        }
    }
}