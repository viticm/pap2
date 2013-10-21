using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using System.Data.SqlClient;

namespace GuideConfig
{
    class Helper
    {
        /// <summary>
        /// 获取数据表
        /// </summary>
        /// <param name="sqlString">查询字符串</param>
        /// <param name="conn">sql连接</param>
        /// <returns></returns>
        public static DataTable GetDataTable(string sqlString, SqlConnection conn)
        {
            DataTable table = null;

            if (conn.State == ConnectionState.Closed)
            {
                conn.Open();
            }

            SqlDataAdapter adapter = new SqlDataAdapter(sqlString, conn);
            adapter.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            DataSet dataSet = new DataSet();
            adapter.Fill(dataSet);
            table = dataSet.Tables[0];

            if (conn.State == ConnectionState.Open)
            {
                conn.Close();
            }

            return table;
        }
    }
}
