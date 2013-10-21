using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Data;
using System.Data.SqlClient;

namespace OptimizeLoadTreeFinder
{
    public class Class1
    {
        static NodeFinder nodeFinder = new NodeFinder();

        public void Test(string strField, string strValue, bool bFindByNodeText, TreeView tree, SqlConnection conn)
        {
            nodeFinder.Init(strField, strValue, bFindByNodeText, tree, conn);
            nodeFinder.FindNext();

        }
       
    }
}
