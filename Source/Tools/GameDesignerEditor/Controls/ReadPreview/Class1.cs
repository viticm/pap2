using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;


namespace ReadPreview
{
    public class Class1
    {
        public Class1()
        {

        }
        public string Show(string strContent, int Showtype, string ClientDir, SqlConnection conn)
        {
            string Newstr = strContent;
            string strtemp = strContent.Replace("\\n", "\n");

            Form1 myform = new Form1(strtemp,Showtype, ClientDir,conn);
            if (!myform.Init(strtemp, Showtype, ClientDir, conn))
            {
                return Newstr;
            }            
            if (myform.ShowDialog() == DialogResult.OK)
            {
                Newstr = myform.GetText();
                Newstr = Newstr.Replace("\n", "\\n");
            }                                 
            return Newstr;
        }
    }
}
