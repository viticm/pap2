using System;
using System.Collections.Generic;
using System.Text;

namespace NpcSocketDesc
{
    class Helper
    {
        public static System.Data.DataTable GetDataTable(string sql, System.Data.SqlClient.SqlConnection conn)
        {
            System.Data.SqlClient.SqlDataAdapter adp = new System.Data.SqlClient.SqlDataAdapter(sql, conn);
            adp.MissingSchemaAction = System.Data.MissingSchemaAction.AddWithKey;
            System.Data.DataSet ds = new System.Data.DataSet();
            adp.Fill(ds);
            System.Data.DataTable tbl = ds.Tables[0];
            return tbl;
        }
    }
}
