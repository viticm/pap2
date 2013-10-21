using System;
using System.Collections.Generic;
using System.Text;
using System.Data.SqlClient;
using System.Data;
using System.Collections;
using System.Windows.Forms;
using System.IO;

namespace RecipeIO
{
    public class Program
    {
        private ArrayList fields; // 列名链表
        private string[] values; // 行数据数组
        private SqlConnection conn; // sql连接
        private string filePath; // 文件路径

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="conn">数据库连接</param>
        /// <param name="filePath">文件路径</param>
        /// <param name="fields">列名链表</param>
        /// <param name="values">行数据数组</param>
        public Program(SqlConnection conn, string filePath, ArrayList fields, string[] values)
        {
            this.conn = conn;
            this.filePath = filePath;
            this.fields = fields;
            this.values = values;
        }

        /// <summary>
        /// 填充数字字符串
        /// </summary>
        /// <param name="s">数字字符串</param>
        /// <returns>填充后的字符串</returns>
        private string FullFillNumber(string s)
        {
            string newString = s;

            for(int i = s.Length; i < 3; i++)
            {
                newString = "0" + newString;
            }

            return newString;
        }

        /// <summary>
        /// 导入数据
        /// </summary>
        /// <returns>是否导入成功</returns>
        public bool Import()
        {
            bool success = true;

            DataTable table = null;
            try
            {
                if(conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }

                string sqlString = string.Format("SELECT * FROM {0}", "CraftBelongName");
                table = GetDataTable(sqlString);
            }
            catch (SqlException ex)
            {
                MessageBox.Show("读取配方配置表数据时产生sql异常: " + ex.ToString());
            }
            finally
            {
                if(conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
            }

            // 遍历列数据
            int belongIndex = 0;
            for(int i = 0; i < fields.Count; i++)
            {
                if(fields[i].ToString() == "Belong")
                {
                    belongIndex = i;
                    break;
                }
            }

            // 增加新列
            fields.Add("BelongName");           
            fields.Add("filename"); 
            fields.Add("filerealname");
            fields.Add("fileindex");

            // 遍历行数据
            string fileName = Path.GetFileName(filePath);
            string[] data;
            string className = "";
            string fileIndex = "";
            string belongID = "";
            bool firstLine = true;

            for (int i = 0; i < values.Length; i++)
            {
                string s = values[i];
                if(firstLine) // 自动跳过第一行
                {
                    s += "\tBelongName\tfilename\tfilerealname\tfileindex";
                    values[i] = s;
                    firstLine = false;
                    continue;
                }

                
                if (s != "") // 跳过空的数据行
                {
                    switch (fileName)
                    {
                        case "cooking.tab":
                            {
                                className = "烹饪";
                                fileIndex = "4";
                                break;
                            }
                        case "tailoring.tab":
                            {
                                className = "缝纫";
                                fileIndex = "5";
                                break;
                            }
                        case "founding.tab":
                            {
                                className = "铸造";
                                fileIndex = "6";
                                break;
                            }
                        case "medicine.tab":
                            {
                                className = "医术";
                                fileIndex = "7";
                                break;
                            }
                        default:
                            {
                                MessageBox.Show(string.Format("数据错误，当前文件名{0}无效！", fileName));
                                success = false;
                                break;
                            }
                    }

                    if (success)
                    {
                        data = s.Split(new char[] { '\t' });
                        belongID = data[belongIndex];

                        DataRow[] rows = table.Select(string.Format("ProfessionID = {0} AND BelongID = {1}", fileIndex, belongID));
                        if (rows.Length > 0)
                        {
                            s += "\t";
                            s += FullFillNumber(belongID) + "_" + rows[0]["BelongName"].ToString() + "\t";
                            s += className + "\t";
                            s += fileName + "\t";
                            s += fileIndex;
                        }
                        else
                        {
                            if(belongID == "0") // 默认行
                            {
                                s += "\t";
                                s += FullFillNumber(belongID) + "_无分类\t";
                                s += className + "\t";
                                s += fileName + "\t";
                                s += fileIndex;
                            }
                            else
                            {
                                MessageBox.Show(string.Format("数据错误，没有找到ProfessionID = {0}，BelongID = {1}的BelongName！",
                                                                fileIndex, belongID));
                                success = false;
                            }                            
                        }

                        values[i] = s;
                    }
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
