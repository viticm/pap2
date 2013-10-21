using System;
using System.Collections.Generic;
using System.Text;
using System.Data.SqlClient;
using System.Windows.Forms;
using ScriptViewer;

namespace testScriptView
{
    class Program
    {
        [STAThread]
        static void Main(string[] args)
        {


            testForm tf = new testForm();
            tf.Show();
            while (tf.Visible == true)
            {
                Application.DoEvents();
                System.Threading.Thread.Sleep(1);
            }
        }


    }
}
