using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices; // import
using System.Diagnostics;
using System.Diagnostics;

namespace WindowsApplication1
{
    public partial class Form1 : Form
    {
        [DllImport("DllMergeTga.dll")]//, CharSet = CharSet.Unicode)]
        public static extern int TgaToUitex(string strFileTga, string strOutputFolder);

        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            string file = @"C:\Documents and Settings\kuangsihao\桌面\MergeTga\BJ_万花001.tga";
            string folder = @"C:\Documents and Settings\kuangsihao\桌面\MergeTga\Debug";

            TgaToUitex(file, folder);
        }
    }
}