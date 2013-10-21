using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using atlaxLib;

namespace NpcSocketDesc
{
    public class Class1
    {
        public Class1()
        {

        }

        public string RunMyFunction(string basedir, Test3DEngine engine, string representid, string socket, SqlConnection conn)
        {
            Form1 frm = new Form1(basedir, engine, representid, socket, conn);
            frm.ShowDialog();
            return frm.m_strFirstDesc;
        }
    }
}
