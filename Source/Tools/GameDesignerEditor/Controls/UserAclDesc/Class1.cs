using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using System.Data;
using System.Collections;

namespace UserAclDesc
{
    public class Class1
    {
        public static SqlConnection Conn = null;

        public Class1(SqlConnection conn)
        {
            Conn = conn;
        }

        public int NewUserIndex()
        {
            Form1 fm1 = new Form1();
            DialogResult r = fm1.ShowDialog();
            return fm1.userIndex;
        }

        public ArrayList NewModuleIndex(int userIndex)
        {
            Form2 fm2 = new Form2(userIndex);
            DialogResult r = fm2.ShowDialog();
            return fm2.moduleIndices;
        }


    }
}
