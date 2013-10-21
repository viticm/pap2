using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using System.Data.SqlClient;
using System.Windows.Forms;

namespace ToolUserManager
{
    class Helper
    {
        public static System.Data.DataTable GetDataTable(string sql, SqlConnection conn)
        {
            SqlDataAdapter adp = new SqlDataAdapter(sql, conn);
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            DataSet ds = new DataSet();
            adp.Fill(ds);
            System.Data.DataTable tbl = ds.Tables[0];
            return tbl;
        }

        public static string GetIPAddress(string hostName)
        {            
            string ip = string.Empty;
            try
            {
                ip = System.Net.Dns.GetHostEntry(hostName).AddressList[0].ToString();
            }
            catch (System.Exception ex)
            {
            	
            }
            
            return ip;
        }
        //判断字符串能否转换为数字（整数，浮点数）
        public static bool CanConverToNumber(string ConvertingStr)
        {
            bool bRet = System.Text.RegularExpressions.Regex.IsMatch(ConvertingStr, @"^[+-]?\d+([\.]\d+)?$");
            return bRet;
        }
        public static bool IsValidIpAddress(string ipAddress)
        {
            bool bRet = System.Text.RegularExpressions.Regex.IsMatch(ipAddress, @"^(\d{0,3}\.){3}\d{0,3}$");
            return bRet;
        }


        //执行sql非查询语句
        public static int SqlNoneQureyExcute( string sql, SqlConnection conn)
        {
            int affectedRowCnt = 0;

            if (conn.State == ConnectionState.Closed)
            {
                conn.Open();
            }
            SqlCommand cmd = new SqlCommand(sql, conn);

            try
            {
                affectedRowCnt = cmd.ExecuteNonQuery();
            }
            catch (System.Exception e)
            {
                MessageBox.Show("execute sql cmd error:" + e.ToString());
            }
            finally
            {
                conn.Close();
            }

            return affectedRowCnt;
        }
        /// <summary>
        /// 查询并返回第一个单元的（string）值
        /// </summary>
        /// <param name="sql"></param>
        /// <param name="conn"></param>
        /// <returns></returns>
        public static string GetFirstCellStr(string sql, SqlConnection conn)
        {
            string strRet = string.Empty;
            object ob = null;

            if (conn.State == ConnectionState.Closed)
            {
                conn.Open();
            }
            SqlCommand cmd = new SqlCommand(sql, conn);

            try
            {
                ob = cmd.ExecuteScalar();
            }
            catch (System.Exception e)
            {
                MessageBox.Show("execute sql cmd error:" + e.ToString());
            }
            finally
            {
                conn.Close();
            }

            if (ob == null)
            {
                strRet = string.Empty;
            }
            else
            {
                strRet = ob.ToString();
            }

            return strRet;
        }
    
        public static bool QueryHasResult(string sql, SqlConnection conn)
        {
            DataTable dt = GetDataTable(sql, conn);
            return dt.Rows.Count > 0;
        }
    
    }
}
