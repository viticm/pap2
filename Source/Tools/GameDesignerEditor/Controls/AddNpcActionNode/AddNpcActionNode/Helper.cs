using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;

namespace AddNpcActionNode
{
    class Helper
    {
        /// <summary>
        /// 获取数据表
        /// </summary>
        /// <param name="sql">查询字符串</param>
        /// <returns>数据表</returns>
        static public DataTable GetDataTable(string sql, SqlConnection conn)
        {
            DataTable tbl = null;

            try
            {
                if (conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }

                SqlDataAdapter adp = new SqlDataAdapter(sql, conn);
                adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
                DataSet ds = new DataSet();
                adp.Fill(ds);
                tbl = ds.Tables[0];
            }
            catch (SqlException ex)
            {
                MessageBox.Show("在查询数据表内容时产生sql异常: " + ex.ToString());
            }
            finally
            {
                if (conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
            }

            return tbl;
        }
    }
}
