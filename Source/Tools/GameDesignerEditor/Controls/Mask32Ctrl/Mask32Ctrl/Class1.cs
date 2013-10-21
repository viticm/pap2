using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using LuaInterface;

namespace Mask32Ctrl
{
    public class Class1
    {
        public Class1()
        {

        }

        public uint Show(uint oldvalue, LuaTable nums, LuaTable texts)
        {
            Form1 frm = new Form1(oldvalue, nums, texts);
            DialogResult result = frm.ShowDialog();
            if (result == DialogResult.OK)
                return frm.ret;
            else // cancel
                return oldvalue;
        }
    }
}
