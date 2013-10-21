using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;
using System.Windows.Forms;
using System.Data.SqlClient;
using System.Data;

namespace MasterIO
{
    class ImportFile
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
        public ImportFile(SqlConnection conn, ArrayList fields, string[] values)
        {
            this.conn = conn;
            this.fields = fields;
            this.values = values;
        }

        /// <summary>
        /// 导入学习配方列表
        /// </summary>
        /// <returns>是否导入成功</returns>
        public bool ImportLearnRecipeList()
        {
            bool success = true; // 是否导入成功
            string fieldName; // 字段名称
            string[] data; // 行数据
            int craftIDIndex = 0; // 技能ID索引
            int recipeIDIndex = 0; // 配方ID索引
            int nPriceIndex = 0; // 价格索引
            string craftID; // 技能id
            string recipeID; // 配方id
            string nPrice; // 价格

            for (int i = 0; i < fields.Count; i++)
            {
                fieldName = fields[i].ToString();
                switch(fieldName)
                {
                    case "CraftID":
                        {
                            craftIDIndex = i;
                            break;
                        }
                    case "RecipeID":
                        {
                            recipeIDIndex = i;
                            break;
                        }
                    case "nPrice":
                        {
                            nPriceIndex = i;
                            break;
                        }
                }
            }

            try
            {
                if (conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }

                string sqlString = string.Format("SELECT * FROM {0}", "tbl_master_skills");
                DataTable skillTable = GetDataTable(sqlString);
                DataRow[] rows;
                SqlCommand cmd = conn.CreateCommand();
                bool firstLine = true;

                foreach(string s in values)
                {
                    if(s != "") // 去掉空的数据行
                    {
                        if(firstLine) // 跳过第一行(列数据)
                        {
                            firstLine = false;
                            continue;
                        }

                        data = s.Split(new char[]{'\t'});
                        craftID = data[craftIDIndex];
                        recipeID = data[recipeIDIndex];
                        nPrice = data[nPriceIndex];
 
                        rows = skillTable.Select(string.Format("CraftID = '{0}' AND RecipeID = '{1}'", int.Parse(craftID), int.Parse(recipeID)));
                        if(rows.Length > 0) // 找到对应的数据行
                        {
                            if(rows[0]["nPrice"].ToString() != nPrice) // 需要更新价格
                            {
                                sqlString = string.Format("UPDATE {0} SET nPrice = {1} WHERE CraftID = {2} AND RecipeID = {3}",
                                    "tbl_master_skills", nPrice, craftID, recipeID);
                                cmd.CommandText = sqlString;
                                cmd.ExecuteNonQuery();
                            }
                        }
                    }
                }

                MessageBox.Show("学习师技能配方价格更新成功!");
            }
            catch (SqlException ex)
            {
                MessageBox.Show("在导入学习配方列表时产生sql异常: " + ex.ToString());
                success = false;
            }
            finally
            {
                if (conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
            }

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
