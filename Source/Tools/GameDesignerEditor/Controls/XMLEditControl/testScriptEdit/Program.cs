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
            string strTest =  ssc.XMLDataEdit("", DBManager.DataBaseManager.GetDataBaseManager().Connection);
            MessageBox.Show(strTest);
        }
    }
}
