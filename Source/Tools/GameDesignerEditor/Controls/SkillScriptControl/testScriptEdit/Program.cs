using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using XMLEditControl;

namespace testScriptEdit
{
    class Program
    {
        [STAThread]
        static void Main(string[] args)
        {
            SSControl ssc = new SSControl();
            string strTest =  ssc.XMLDataEdit("<ArgList><Arg ID=\"1\" Name=\"-\" Type=\"7\" Text=\"Íæ¼Ò\"/></ArgList>", null);
            //MessageBox.Show("xxxx");
        }
    }
}
