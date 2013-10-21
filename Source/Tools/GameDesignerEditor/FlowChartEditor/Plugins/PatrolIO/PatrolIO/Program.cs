using System;
using System.Collections.Generic;
using System.Text;
using System.Data.SqlClient;
using System.Windows.Forms;

namespace PatrolIO
{
    public class Program
    {
        PatrolEditor patrolEditor;

        public string PatrolIndex
        {
            get
            {
                return patrolEditor.PatrolIndex;
            }
        }

        public string PatrolName
        {
            get
            {
                return patrolEditor.PatrolName;
            }
        }

        public string PatrolPointIndex
        {
            get
            {
                return patrolEditor.PatrolPointIndex;
            }
        }

        public int Show(SqlConnection conn, string mapName)
        {
            patrolEditor = new PatrolEditor(conn, mapName);
            DialogResult result = patrolEditor.ShowDialog();
            if(result == DialogResult.OK)
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }
    }
}
