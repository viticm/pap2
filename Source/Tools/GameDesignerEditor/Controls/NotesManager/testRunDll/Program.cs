using System;
using System.Collections.Generic;
using System.Text;
using System.Data.SqlClient;


namespace testRunDll
{
    class Program
    {
        [STAThread]
        static void Main(string[] args)
        {
#if true
            parentForm pf = new parentForm();
            pf.ShowDialog(); 

#else

            NotesManager.doit nm = new NotesManager.doit();
            nm.show(conn,"npc,1");
#endif

        }
    }
}
