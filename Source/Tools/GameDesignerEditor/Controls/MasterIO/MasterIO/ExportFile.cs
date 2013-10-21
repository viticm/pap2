using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Data;
using System.Data.SqlClient;
using System.Collections;
using System.Windows.Forms;

namespace MasterIO
{
    class ExportFile
    {
        private SqlConnection conn; // sql连接
        private ArrayList fields; // 列数据链表
        private string[] values; // 行数据数组
        private string rootPath; // 导出根目录

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="conn">sql连接</param>
        /// <param name="rootPath">导出根目录</param>
        /// <param name="fields">列数据链表</param>
        /// <param name="values">行数据数组</param>
        public ExportFile(SqlConnection conn, string rootPath, ArrayList fields, string[] values)
        {
            this.conn = conn;
            this.rootPath = rootPath;
            this.fields = fields;
            this.values = values;
        }

        /// <summary>
        /// 导出学习师索引文件
        /// </summary>
        /// <returns>是否导出成功</returns>
        public bool ExportMasterList()
        {
            bool success = true; // 是否导出成功
            List<string> dataList = new List<string>(); // 导出数据的链表
            List<string> existMasterList = new List<string>(); // 已经存在的学习师链表
            string[] data; // 数据数组
            int folderNameIndex = 0; // 文件名索引
            int fileNameIndex = 0; // 文件夹名索引
            int masterIDIndex = 0; // 索引号索引
            int idIndex = 0; // id索引
            string fileName; // 文件名
            string folderName; // 文件夹名 
            string masterID; // 索引号
            string filePath; // 文件路径
            string id; // id

            for (int i = 0; i < fields.Count; i++)
            {
                string fieldName = fields[i].ToString().TrimEnd(new char[] {'\r', '\n'});
                switch(fieldName)
                {
                    case "foldername":
                        {
                            folderNameIndex = i;
                            break;
                        }
                    case "filename":
                        {
                            fileNameIndex = i;
                            break;
                        }
                    case "MasterID":
                        {
                            masterIDIndex = i;
                            break;
                        }
                    case "id":
                        {
                            idIndex = i;
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

                string sqlString = string.Format("SELECT * FROM {0} ORDER BY ID", "tbl_master_list");
                DataTable masterTable = GetDataTable(sqlString);
                SqlCommand cmd = conn.CreateCommand();
                DataRow newRow;
                int maxID = 1;
                if (masterTable.Rows.Count > 0)
                {
                    maxID = int.Parse(masterTable.Rows[masterTable.Rows.Count - 1]["ID"].ToString()) + 1;
                } 

                foreach(string s in values)
                {
                    if(s != "") // 去掉为空的数据行
                    {
                        data = s.Split('\t');
                        fileName = data[fileNameIndex];
                        folderName = data[folderNameIndex];
                        masterID = data[masterIDIndex];
                        id = data[idIndex];
                        filePath = string.Format("Master/{0}/{1}", folderName, fileName);                    

                        DataRow row = masterTable.Rows.Find(masterID);
                        if((masterID == "0") && (row != null)) // 无效的学习师索引ID
                        {
                            masterTable.Rows.Remove(row);
                            row = null;
                        }

                        if(row != null) // 找到对应学习师索引所在的列
                        {
                            if(filePath != row["TabFile"].ToString()) // 文件路径已经被修改过
                            {
                                sqlString = string.Format("UPDATE {0} SET TabFile = '{1}' WHERE ID = '{2}'", "tbl_master_list", filePath, masterID);
                                cmd.CommandText = sqlString;
                                cmd.ExecuteNonQuery();
                                row["TabFile"] = filePath;
                            }

                            existMasterList.Add(masterID);
                        }
                        else // 没有找到对应学习师索引所在的列
                        {
                            DataRow[] rows = masterTable.Select(string.Format("TabFile = '{0}'", filePath));
                            if(rows.Length > 0) // 该路径对应的学习师已经存在
                            {
                                string newMasterID = rows[0]["ID"].ToString();
                                sqlString = string.Format("UPDATE {0} SET MasterID = '{1}' WHERE id = '{2}'", "tbl_master_skills", newMasterID, id);
                                cmd.CommandText = sqlString;
                                cmd.ExecuteNonQuery();
                            }
                            else
                            {
                                sqlString = string.Format("INSERT INTO {0} (ID, TabFile) VALUES ('{1}', '{2}')",
                                                                "tbl_master_list", maxID.ToString(), filePath);
                                cmd.CommandText = sqlString;
                                cmd.ExecuteNonQuery();
                                newRow = masterTable.NewRow();
                                newRow["ID"] = maxID;
                                newRow["TabFile"] = filePath;
                                masterTable.Rows.Add(newRow);
                                existMasterList.Add(maxID.ToString());

                                if (masterID == "0") // 被复制粘贴过或者新建的技能，并且它对应的学习师索引还不存在，需要更新技能表
                                {
                                    sqlString = string.Format("UPDATE {0} SET MasterID = '{1}' WHERE id = '{2}'",
                                        "tbl_master_skills", maxID.ToString(), id);
                                    cmd.CommandText = sqlString;
                                    cmd.ExecuteNonQuery();
                                }

                                maxID++;
                            }         
                        }
                    }
                }

                // 清理没有被引用到的学习师数据
                List<DataRow> deleteRowList = new List<DataRow>(); // 需要删除的数据行链表
                foreach (DataRow r in masterTable.Rows)
                {
                    masterID = r["ID"].ToString();
                    if (!existMasterList.Contains(masterID))
                    {
                        sqlString = string.Format("DELETE FROM {0} WHERE ID = '{1}'", "tbl_master_list",
                            masterID);
                        cmd.CommandText = sqlString;
                        cmd.ExecuteNonQuery();
                        deleteRowList.Add(r);
                    }
                }

                foreach (DataRow r in deleteRowList)
                {
                    masterTable.Rows.Remove(r);
                }

                // 导出学习师索引文件
                dataList.Add("ID\tTabFile");
                foreach(DataRow r in masterTable.Rows)
                {
                    masterID = r["ID"].ToString();
                    filePath = r["TabFile"].ToString();
                    dataList.Add(string.Format("{0}\t{1}", masterID, filePath));
                }

                DirectoryInfo di = new DirectoryInfo(rootPath);
                string topFolder = di.Parent.FullName;
                SaveDataToFile(Path.Combine(topFolder, "Master.tab"), dataList, true);
            }
            catch (SqlException ex)
            {
                MessageBox.Show("在获取学习师信息时产生sql异常: " + ex.ToString());
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
        /// 导出配方配置文件
        /// </summary>
        /// <returns>是否导出成功</returns>
        public bool ExportRecipeList()
        {
            bool success = true; // 是否导出成功
            List<string> dataList = new List<string>(); // 导出数据的链表
            string[] data; // 数据数组
            string fieldName; // 列名
            int craftIDIndex = 0; // craftID索引
            int recipeIDIndex = 0; // recipeID索引
            int professionIDIndex = 0; // professionID索引
            int professionLevelIndex = 0; // professionLevel索引
            int branchIDIndex = 0; // branchID索引
            int reputationIDIndex = 0; // reputationID索引
            int nReputationLevelIndex = 0; // nReputationLevel索引
            int nPriceIndex = 0; // nPrice索引

            DataTable recipeTable; // 配方数据表

            dataList.Add("CraftID\tCraft\tRecipeID\tName\tBelong\t_Quality\tProfessionLevel\tBranchID\tReputationID\tnReputationLevel\tnPrice"); // 添加列

            for (int i = 0; i < fields.Count; i++)
            {
                fieldName = fields[i].ToString().Trim(new char[] { '\r', '\n' }); // 去掉可能存在的换行字符

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
                    case "ProfessionID":
                        {
                            professionIDIndex = i;
                            break;
                        }
                    case "ProfessionLevel":
                        {
                            professionLevelIndex = i;
                            break;
                        }
                    case "BranchID":
                        {
                            branchIDIndex = i;
                            break;
                        }
                    case "ReputationID":
                        {
                            reputationIDIndex = i;
                            break;
                        }
                    case "nReputationLevel":
                        {
                            nReputationLevelIndex = i;
                            break;
                        }
                    case "nPrice":
                        {
                            nPriceIndex = i;
                            break;
                        }
                    default:
                        {
                            break;
                        }
                }
            }

            string newLine = "";

            foreach(string s in values)
            {
                if(s != "") // 忽略空行
                {
                    data = s.Split('\t');
                    string craftID = data[craftIDIndex];
                    string recipeID = data[recipeIDIndex];
                    string professionID = data[professionIDIndex];
                    string professionLevel = data[professionLevelIndex];
                    string branchID = data[branchIDIndex];
                    string reputationID = data[reputationIDIndex];
                    string nReputationLevel = data[nReputationLevelIndex];
                    string nPrice = data[nPriceIndex];

                    recipeTable = GetRecipeTable(recipeID, professionID);

                    newLine = string.Format("{0}\t", craftID);
                    newLine += string.Format("{0}\t", GetCraftName(craftID));
                    newLine += string.Format("{0}\t", recipeID);
                    if(recipeTable != null) // 找到对应的配方
                    {
                        newLine += string.Format("{0}\t", recipeTable.Rows[0]["Name"].ToString());
                        newLine += string.Format("{0}\t", recipeTable.Rows[0]["Belong"].ToString());
                        newLine += string.Format("{0}\t", recipeTable.Rows[0]["_Quality"].ToString());
                    }
                    else // 没有找到对应的配方
                    {
                        newLine += "\t\t\t";
                    }
                    newLine += string.Format("{0}\t", professionLevel);
                    newLine += string.Format("{0}\t", branchID);
                    newLine += string.Format("{0}\t", reputationID);
                    newLine += string.Format("{0}\t", nReputationLevel);
                    newLine += string.Format("{0}\t", nPrice);
                    dataList.Add(newLine.TrimEnd(new char[] { '\t' }));
                }
            }

            DirectoryInfo di = new DirectoryInfo(rootPath);
            string topFolder = di.Parent.FullName;
            topFolder = Path.Combine(topFolder, "ForEditor");
            string fileName = Path.Combine(topFolder, "LearnRecipeList.tab");
            SaveDataToFile(fileName, dataList, true);

            return success;
        }

        /// <summary>
        /// 获取技能的名称
        /// </summary>
        /// <param name="id"></param>
        /// <returns></returns>
        private string GetCraftName(string id)
        {
            string craftName = "";

            try
            {
                if(conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }

                string sqlString = string.Format("SELECT Name FROM {0} WHERE ID = '{1}'", "tbl_master_craft", id);
                DataTable craftTable = GetDataTable(sqlString);
                if(craftTable.Rows.Count > 0)
                {
                    craftName = craftTable.Rows[0][0].ToString();
                }
            }
            catch(SqlException ex)
            {
                MessageBox.Show("在读取学习师技能名称时产生sql异常: " + ex.ToString());
            }
            finally
            {
                if(conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
            }

            return craftName;
        }

        /// <summary>
        /// 导出学习师文件
        /// </summary>
        /// <returns>是否导出成功</returns>
        public bool ExportMasterFiles()
        {
            Hashtable fileTable = new Hashtable();
            List<string> dataList;
            DataTable masterTable = null;
            string headLine = "CraftID\tRecipeID\tProfessionID\tProfessionLevel\tBranchID\tReputationID\tnReputationLevel\tnPrice";
            string newLine;
            string fileName;
            string folderName;
            string filePath;

            // 读取数据库数据
            try
            {
                if(conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }

                string sqlString = string.Format("SELECT * FROM {0} ORDER BY {1}", "tbl_master_skills", "ProfessionLevel");
                masterTable = GetDataTable(sqlString);
            }
            catch (SqlException ex)
            {
                MessageBox.Show("读取学习师信息时产生sql异常: " + ex.ToString());
            }
            finally
            {
                if(conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
            }

            foreach(DataRow r in masterTable.Rows)
            {
                newLine = r["CraftID"].ToString() + "\t" + r["RecipeID"].ToString() + "\t" + r["ProfessionID"].ToString() + "\t" +
                    r["ProfessionLevel"].ToString() + "\t" + r["BranchID"].ToString() + "\t" + r["ReputationID"].ToString() + "\t" +
                    r["nReputationLevel"].ToString() + "\t" + r["nPrice"].ToString();
                fileName = r["filename"].ToString();
                folderName = r["foldername"].ToString();
                filePath = Path.Combine(folderName, fileName);

                if(fileTable[filePath] != null)
                {
                    dataList = fileTable[filePath] as List<string>;
                    dataList.Add(newLine);
                }
                else
                {
                    dataList = new List<string>();
                    dataList.Add(headLine);
                    dataList.Add(newLine);
                    fileTable[filePath] = dataList;
                }
            }

            List<string> fileList = new List<string>();
            foreach(object o in fileTable.Keys)
            {
                fileList.Add(o.ToString());
            }

            // 选择导出的路径
            string resultFolder = "";
            FolderBrowserDialog openFolderDialog1 = new FolderBrowserDialog();
            openFolderDialog1.Description = "请选择导出文件的保存目录";
            openFolderDialog1.ShowNewFolderButton = true;
            if(rootPath == null)
            {
                openFolderDialog1.RootFolder = Environment.SpecialFolder.MyComputer;
            }
            else
            {
                openFolderDialog1.SelectedPath = rootPath;
            }

            if (openFolderDialog1.ShowDialog() != DialogResult.OK)
            {
                return false;
            }
            resultFolder = openFolderDialog1.SelectedPath;
            rootPath = resultFolder;

            // 选择要导出的文件
            ChooseFileForm cForm = new ChooseFileForm("请选择要导出的文件", fileList);
            DialogResult result = cForm.ShowDialog();
            if(result != DialogResult.OK)
            {
                return false;
            }

            List<string> fileNameList = new List<string>();
            foreach(string s in fileList)
            {
                dataList = fileTable[s] as List<string>;
                filePath = Path.Combine(resultFolder, s);
                fileNameList.Add(filePath);
                SaveDataToFile(filePath, dataList, false);
            }

            // 显示导出文件列表
            string message = "下列文件:\n\n";
            foreach (string s in fileNameList)
            {
                message = message + string.Format("{0}\n", s);
            }
            message = message + "\n\n";
            message = message + "已经成功导出!";
            MessageBox.Show(message);

            return true;
        }

        /// <summary>
        /// 获取配方的信息数据表
        /// </summary>
        /// <param name="recipeID">配方的ID</param>
        /// <param name="professionID">profession的ID</param>
        /// <returns>配方的信息数据表</returns>
        private DataTable GetRecipeTable(string recipeID, string professionID)
        {
            DataTable recipeTable = null;
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

                string sqlString = string.Format("SELECT * FROM {0} WHERE ID = '{1}' AND filename = '{2}'", "tbl_Recipe", recipeID, professionName);
                recipeTable = GetDataTable(sqlString);
                if(recipeTable.Rows.Count == 0) // 没有找到对应的数据行
                {
                    recipeTable = null;
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

            return recipeTable;
        }

        /// <summary>
        /// 将数据写入文件
        /// </summary>
        /// <param name="fileName">文件名</param>
        /// <param name="dataList">数据链表</param>
        /// <param name="showMessage">是否显示提示信息</param>
        private void SaveDataToFile(string fileName, List<string> dataList, bool showMessage)
        {
            try
            {
                string path = Path.GetDirectoryName(fileName);
                FileInfo fi = new FileInfo(fileName);
                StreamWriter sw;

                if (!fi.Exists) // 文件不存在
                {
                    if (!Directory.Exists(path)) // 目录不存在
                    {
                        Directory.CreateDirectory(path); // 先创建目录，再创建文件
                    }
                    FileStream fs = fi.Create();
                    fs.Close();
                    sw = new StreamWriter(fileName, false, Encoding.GetEncoding("gb2312"));
                }
                else // 文件已经存在
                {
                    sw = new StreamWriter(fileName, false, Encoding.GetEncoding("gb2312"));
                }

                foreach (string s in dataList)
                {
                    sw.WriteLine(s);
                }
                sw.Close();

                if(showMessage)
                {
                    MessageBox.Show(string.Format("文件 {0} 已经成功导出!", fileName));
                }            
            }
            catch (IOException ex)
            {
                MessageBox.Show("在导出数据文件时产生IO异常: " + ex.ToString());
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
