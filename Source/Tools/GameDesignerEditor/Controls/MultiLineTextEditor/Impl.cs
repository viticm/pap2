using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace MultiLineTextEditor
{
    public class Impl
    {
        public string Show(string strText)
        {
            TextEditor frm = new TextEditor();
            frm.textBoxX1.Text = strText;
            System.Windows.Forms.DialogResult r = frm.ShowDialog();
            if (r == DialogResult.OK && frm.textBoxX1.Text != strText)
                return frm.textBoxX1.Text;
            else
                return null;
        }
    }
}
