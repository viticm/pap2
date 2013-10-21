using System;
using System.Collections.Generic;
using System.Text;
using System.Data.SqlClient;

namespace QuestSelector
{
    public class ClassQuestSelector
    {
        private Form1 f = null;// = new Form1(Conn, id);
        public int Show(SqlConnection Conn, int id)
        {
            if (f == null)
            {
                f = new Form1(Conn, id);
                f.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            }
            if(f.ShowDialog()== System.Windows.Forms.DialogResult.OK)
            {
                return f.ID;
            }
            else
            {
                return id;
            }
        }
    }
}
