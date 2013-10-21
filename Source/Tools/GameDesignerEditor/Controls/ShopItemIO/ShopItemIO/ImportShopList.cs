using System;
using System.Collections.Generic;
using System.Text;
using System.Data.SqlClient;
using System.Collections;
using System.Data;
using System.Windows.Forms;

namespace ShopItemIO
{
    class ImportShopList
    {
        private SqlConnection conn; // sql连接
        private ArrayList fields; // 列名链表
        private string[] values; // 行数据数组

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="conn">sql连接</param>
        /// <param name="fields">列名链表</param>
        /// <param name="values">行数据数组</param>
        public ImportShopList(SqlConnection conn, ArrayList fields, string[] values)
        {
            this.conn = conn;
            this.fields = fields;
            this.values = values;
        }

        /// <summary>
        /// 更新物品的价格
        /// </summary>
        /// <returns>是否更新成功</returns>
        public bool UpdateItemPrice()
        {
            bool success = true; // 更新是否成功
            Hashtable indexTable = new Hashtable();
            SqlTransaction transaction = null;
            List<string> updateFieldList = new List<string>();

            updateFieldList.Add("Price");
            updateFieldList.Add("IsValid");
            updateFieldList.Add("CoinType1");
            updateFieldList.Add("CoinAmount1");
            updateFieldList.Add("Contribution");
            updateFieldList.Add("Prestige");
            updateFieldList.Add("AchievementPoint");

            for (int i = 0; i < fields.Count; i++)
            {                                
                indexTable[fields[i].ToString()] = i;
            }

            try
            {
                if (conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }

                string sqlString = "SELECT * FROM tbl_npc_shopitem";
                DataTable itemTable = GetDataTable(sqlString);
                transaction = conn.BeginTransaction();
                SqlCommand cmd = conn.CreateCommand();
                cmd.Transaction = transaction;
                bool firstLine = true;

                foreach (string s in values)
                {
                    if (s != "")
                    {
                        if(firstLine) // 跳过第一行(列数据)
                        {
                            firstLine = false;
                            continue;
                        }

                        string[] data = s.Split('\t');
                        string type = data[(int)indexTable["TabType"]];
                        string id = data[(int)indexTable["ItemIndex"]];
                        string durability = data[(int)indexTable["Durability"]];                        

                        DataRow[] rows = itemTable.Select(string.Format("TabType = '{0}' AND ItemIndex = '{1}' AND Durability = {2}",
                                                                        type, id, durability));
                        foreach(DataRow row in rows)
                        {
                            foreach (string fieldName in updateFieldList)
                            {
                                string fieldValue = data[(int)indexTable[fieldName]];

                                if (row[fieldName].ToString() != fieldValue)
                                {
                                    if (fieldValue == "")
                                    {
                                        sqlString = string.Format("UPDATE tbl_npc_shopitem SET [{0}] = NULL WHERE TabType = '{1}' AND ItemIndex = '{2}' AND Durability = {3}",
                                                                  fieldName, type, id, durability);
                                    }
                                    else
                                    {
                                        sqlString = string.Format("UPDATE tbl_npc_shopitem SET [{0}] = '{1}' WHERE TabType = '{2}' AND ItemIndex = '{3}' AND Durability = {4}",
                                                                  fieldName, fieldValue, type, id, durability);
                                    }

                                    cmd.CommandText = sqlString;
                                    cmd.ExecuteNonQuery();
                                }
                            }
                        }
                    }
                }

                transaction.Commit();
            }
            catch (SqlException ex)
            {
                MessageBox.Show("在更新商店道具的信息时产生sql异常: " + ex.ToString());
                success = false;

                if (transaction != null)
                {
                    transaction.Rollback();
                }                
            }
            finally
            {
                if (conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
            }

            MessageBox.Show("商店道具信息更新成功!");

            return success;
        }

        /// <summary>
        /// 获取数据表
        /// </summary>
        /// <param name="sql">sql语句</param>
        /// <returns>数据表</returns>
        private DataTable GetDataTable(string sql)
        {
            SqlDataAdapter adp = new SqlDataAdapter(sql, conn);
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            DataSet ds = new DataSet();
            adp.Fill(ds);
            System.Data.DataTable tbl = ds.Tables[0];
            return tbl;
        }
    }
}
