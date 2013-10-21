using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.IO;
using System.Collections;
using System.Data.SqlClient;
using System.Data;
using System.Text;

namespace MasterIO
{
    public class Program
    {
        private string path; // 文件路径
        private string[] fields; // 列数据数组
        private string[] values; // 行数据数组
        private SqlConnection conn; // sql连接
        private List<string> valuesList = new List<string>(); // 行数据链表
        private bool loadedFields = false; // 是否已经添加过列名数组 

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="conn">sql连接</param>
        /// <param name="path">文件路径</param>
        public Program(SqlConnection conn, string path)
        {
            this.conn = conn;
            this.path = path;
        }

        /// <summary>
        /// 列名数组属性
        /// </summary>
        public string[] Fields
        {
            get
            {
                return fields;
            }
        }

        /// <summary>
        /// 行数据数组属性
        /// </summary>
        public string[] Values
        {
            get
            {
                return values;
            }
        }

        /// <summary>
        /// 导入学习师信息
        /// </summary>
        public bool LoadMasterSkillFromPath()
        {
            bool loadSucess = true; // 是否导入成功
            string rootFolder = GetLoadPath(); // 根目录
            string appendFolder; // 子目录
            string baseFolder; // 最底层目录
            string filePath; // 文件目录
            string id; // 学习师ID

            if (rootFolder == null) // 用户未选择学习师技能所在的目录
            {
                MessageBox.Show("没有选择学习师技能所在的目录!");
                return false;
            }

            try
            {
                if(conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }

                string sqlString = string.Format("SELECT * FROM {0}", "tbl_master_list");
                DataTable table = GetDataTable(sqlString);
                foreach(DataRow r in table.Rows)
                {
                    appendFolder = r["TabFile"].ToString().Replace('/', '\\'); // 转换路径字符串
                    appendFolder = appendFolder.Substring(7, appendFolder.Length - 7); // 去掉"master\"字符串
                    id = r["ID"].ToString();
                    filePath = Path.Combine(rootFolder, appendFolder);
                    baseFolder = Path.GetDirectoryName(appendFolder); 
                    LoadCraftSkill(filePath, baseFolder, id);
                }
            }
            catch(SqlException ex)
            {
                MessageBox.Show("在读取学习师技能路径时产生sql异常: " + ex.ToString());
                loadSucess = false;
            }
            finally
            {
                if(conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
            }

            values = new string[valuesList.Count + 1];
            values[0] = "empty line";
            for(int i = 0; i < valuesList.Count; i++)
            {
                values[i + 1] = valuesList[i];
            }

            return loadSucess;
        }

        /// <summary>
        /// 获取学习师技能所在的目录
        /// </summary>
        /// <returns>学习师技能所在的目录</returns>
        private string GetLoadPath()
        {
            string resultFolder = null;
            FolderBrowserDialog openFolderDialog1 = new FolderBrowserDialog();
            openFolderDialog1.Description = "请选择学习师技能表所在的目录";
            openFolderDialog1.SelectedPath = path;
            if(openFolderDialog1.ShowDialog() == DialogResult.OK)
            {
                resultFolder = openFolderDialog1.SelectedPath;
            }

            return resultFolder;
        }

        /// <summary>
        /// 获取配方名称
        /// </summary>
        /// <param name="id">配方id</param>
        /// <returns>配方名称</returns>
        private string GetRecipeName(string recipeID, string professionID)
        {
            string recipeName = null;
            string professionName = null;

            switch(professionID)
            {
                case "4":
                    {
                        professionName = "烹饪";
                        break;
                    }
                case "5":
                    {
                        professionName = "缝纫";
                        break;
                    }
                case "6":
                    {
                        professionName = "铸造";
                        break;
                    }
                case "7":
                    {
                        professionName = "医术";
                        break;
                    }
            }

            if(professionName == null) // 没有找到对应的profession
            {
                return null;
            }

            try
            {
                if(conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }

                string sqlString = string.Format("SELECT Name FROM {0} WHERE ID = '{1}' AND filename = '{2}'", "tbl_Recipe", recipeID, professionName);
                SqlCommand cmd = conn.CreateCommand();
                cmd.CommandText = sqlString;
                object o = cmd.ExecuteScalar();
                if((o != null) && !(o is DBNull))
                {
                    recipeName = o.ToString();
                }
            }
            catch(SqlException ex)
            {
                MessageBox.Show("在读取配方信息时产生sql异常: " + ex.ToString());
            }
            finally
            {
                if(conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
            }

            return recipeName;
        }

        /// <summary>
        /// 从文件中读取数据
        /// </summary>
        /// <param name="fileName">文件路径</param>
        /// <param name="folder">文件夹</param>
        /// <param name="masterID">学习师ID</param>
        private void LoadCraftSkill(string fileName, string folder, string masterID)
        {
            FileInfo fi = new FileInfo(fileName);
            if(fi.Exists)
            {
                try
                {
                    StreamReader sr = new StreamReader(fileName, Encoding.GetEncoding("gb2312"));
                    string text = sr.ReadLine();
                    string craftName;
                    string[] data;
                    int index = 0;
                    string appendText;

                    while(text != null)
                    {
                        data = text.Split(new char[] {'\t'});

                        if(index == 0) // 读入第一行，即列数据
                        {
                            if(!loadedFields) // 还没有写列数组
                            {             
                                fields = new string[4 + data.Length];
                                fields[0] = "foldername";
                                fields[1] = "filename";
                                fields[2] = "Name";
                                fields[3] = "MasterID";

                                for (int i = 0; i < data.Length; i++)
                                {
                                    fields[4 + i] = data[i];
                                }

                                loadedFields = true;
                            }
                        }
                        else // 读入行数据
                        {
                            appendText = folder + "\t";
                            appendText += Path.GetFileName(fileName) + "\t";
                            craftName = GetRecipeName(data[1], data[2]);
                            if(craftName != null)
                            {
                                appendText += craftName + "\t";
                            }
                            else
                            {
                                appendText += "\t";
                            }
                            appendText += masterID + "\t";
                            valuesList.Add(appendText + text);
                        }

                        index++;
                        text = sr.ReadLine();
                    }
                }
                catch (IOException ex)
                {
                    MessageBox.Show("在读取场景学习师配置表文件时产生IO异常: " + ex.ToString());
                }
            }
            else
            {
                MessageBox.Show(string.Format("文件{0}不存在!", fileName));
            }
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