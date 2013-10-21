using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace CommonInputForm
{
    public class Class1
    {
        public Class1()
        {

        }

        public string Show(string label, string oldstring)
        {
            Form1 frm = new Form1(label, oldstring);
            DialogResult result = frm.ShowDialog();
            if (result == DialogResult.OK)
                return frm.strRet;
            else // cancel
                return string.Empty;
        }
    }
}
