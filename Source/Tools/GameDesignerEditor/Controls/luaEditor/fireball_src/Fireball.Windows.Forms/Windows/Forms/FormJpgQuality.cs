using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace Fireball.Windows.Forms
{
    public partial class FormJpgQuality : Form
    {
        public FormJpgQuality()
        {
            InitializeComponent();
        }
        public FormJpgQuality(int quality)
            : this()
        {
            if (quality < 0) quality = 0;
            if (quality > 100) quality = 100;

            trackBar.Value = quality / 10;
        }
        public FormJpgQuality(string title, int quality)
            :this(quality)
        {
            this.Text = title;
        }

        private void trackBar_ValueChanged(object sender, EventArgs e)
        {
            int val = trackBar.Value * 10;
            txtValue.Text = val.ToString();
        }

        public int JpegQuality
        {
            get
            {
                return trackBar.Value * 10;
            }
            set
            {
                if (value < 0) value = 0;
                if (value > 100) value = 100;

                trackBar.Value = value / 10;
            }
        }

    }
}