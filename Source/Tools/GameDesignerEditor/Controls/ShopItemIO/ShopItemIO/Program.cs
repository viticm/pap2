using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.IO;
using System.Data.SqlClient;
using System.Collections;
using System.Data;
using System.Text;

namespace ShopItemIO
{
    public class Program
    {
        private string path; // 文件路径
        private string rootFolder; // 商店表根目录
        private SqlConnection conn; // sql连接
        private string[] fields; // 列名数组
        private string[] values; // 行数据数组
        private List<string> valuesList = new List<string>(); // 行数据链表
        private bool loadedFields = false; // 是否已经添加过列名数组 
        private DataTable shopListTable; // 商店索引表
        private DataTable shopItemTable; // 商店道具表

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="conn">sql连接</param>
        /// <param name="path">根路径</param>
        public Program(SqlConnection conn, string path)
        {
            this.conn = conn;
            this.path = path;
            Init();
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
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            if (conn.State == ConnectionState.Closed)
            {
                conn.Open();
            }

            string sqlString = "SELECT * FROM tbl_npc_shoplist";
            shopListTable = GetDataTable(sqlString);

            sqlString = "SELECT * FROM tbl_npc_shopitem";
            shopItemTable = GetDataTable(sqlString);

            if (conn.State == ConnectionState.Open)
            {
                conn.Close();
            }
        }

        /// <summary>
        /// 刷新商店道具名称
        /// </summary>
        /// <param name="baseTree">分类树</param>
        public void RefreshShopItemName(TreeView baseTree)
        {
            if (conn.State == ConnectionState.Closed)
            {
                conn.Open();
            }

            int refreshCount = 0;
            SqlCommand cmd = conn.CreateCommand();

            foreach (DataRow dataRow in shopItemTable.Rows)
            {
                string id = dataRow["id"].ToString();
                string folderName = dataRow["foldername"].ToString();
                string fileName = dataRow["filename"].ToString();
                string itemName = dataRow["Name"].ToString();
                string tabType = dataRow["TabType"].ToString();
                string itemIndex = dataRow["ItemIndex"].ToString();

                if (tabType != "" && itemIndex != "")
                {
                    Hashtable itemInfo = GetItemInfo(cmd, tabType, itemIndex);
                    string newName = itemInfo["Name"] as string;

                    if (!string.IsNullOrEmpty(newName) && itemName != newName)
                    {
                        string sqlString = string.Format("UPDATE tbl_npc_shopitem SET Name = '{0}' WHERE TabType = '{1}' AND ItemIndex = '{2}'",
                                                         newName, tabType, itemIndex);
                        cmd.CommandText = sqlString;
                        cmd.ExecuteNonQuery();

                        RefreshNodeText(baseTree, folderName, fileName, id, newName);

                        refreshCount++;
                    }
                }                                
            }

            if (conn.State == ConnectionState.Open)
            {
                conn.Close();
            }

            MessageBox.Show(string.Format("刷新成功，共有{0}个商店道具的名称被刷新！", refreshCount), "刷新商店道具名称",
                            MessageBoxButtons.OK, MessageBoxIcon.Information);
        }

        /// <summary>
        /// 刷新树结点的文本
        /// </summary>
        /// <param name="tree">分类树</param>
        /// <param name="folderName">文件夹名</param>
        /// <param name="fileName">文件名</param>
        /// <param name="id">道具ID</param>
        /// <param name="newName">新名称</param>
        private void RefreshNodeText(TreeView tree, string folderName, string fileName, string id, string newName)
        {
            foreach (TreeNode folderNode in tree.Nodes)
            {
                if (folderNode.Text == folderName)
                {
                    foreach (TreeNode fileNode in folderNode.Nodes)
                    {
                        if (fileNode.Text == fileName)
                        {
                            foreach (TreeNode itemNode in fileNode.Nodes)
                            {
                                if (((object[])itemNode.Tag)[0].ToString() == id)
                                {
                                    itemNode.Text = newName;
                                    break;
                                }
                            }

                            break;
                        }
                    }

                    break;
                }
            }
        }

        /// <summary>
        /// 导入商店物品信息
        /// </summary>
        public bool LoadShopItemFromPath()
        {
            bool loadSucess = true; // 是否导入成功
            rootFolder = GetLoadPath(); // 根目录

            if (rootFolder == null) // 用户未选择商店道具表所在的目录
            {
                MessageBox.Show("没有选择商店道具表所在的目录!");
                return false;
            }

            List<string> fileNameList = new List<string>();
            DirectoryInfo shopDirectoryInfo = new DirectoryInfo(path);
            
            foreach (DirectoryInfo di in shopDirectoryInfo.GetDirectories())
            {
                if (!di.Attributes.ToString().Contains("Hidden")) // 不显示隐藏文件夹
                {
                    string folderName = di.Name;

                    foreach (FileInfo fi in di.GetFiles())
                    {
                        if (!fi.Attributes.ToString().Contains("Hidden")) // 不显示隐藏文件
                        {
                            string fileName = fi.Name;

                            fileNameList.Add(string.Format("{0}\\{1}", folderName, fileName));
                        }                        
                    }
                }                
            }

            ChooseFileForm cForm = new ChooseFileForm("请选择要导入的商店文件", fileNameList);

            if (cForm.ShowDialog() == DialogResult.OK)
            {
                SqlTransaction transaction = null;
                ProgressForm pForm = new ProgressForm(0, fileNameList.Count);
                pForm.Show();

                try
                {
                    if (conn.State == ConnectionState.Closed)
                    {
                        conn.Open();
                    }

                    transaction = conn.BeginTransaction();
                    SqlCommand cmd = conn.CreateCommand();
                    cmd.Transaction = transaction;

                    int index = 1;

                    foreach (string s in fileNameList)
                    {
                        string[] data = s.Split(new char[] { '\\' }, StringSplitOptions.RemoveEmptyEntries);
                        string folderName = data[0];
                        string fileName = data[1];

                        pForm.ShowProgress(index, string.Format("更新文件{0}的数据...", s));
                        UpdateShopItem(cmd, fileName, folderName);
                        index++;
                    }

                    transaction.Commit();
                }
                catch (SqlException ex)
                {
                	MessageBox.Show("在更新商店道具信息时产生sql异常：" + ex.Message, "更新商店道具信息", MessageBoxButtons.OK,
                                    MessageBoxIcon.Information);

                    if (transaction != null)
                    {
                        transaction.Rollback();
                    }

                    loadSucess = false;
                }
                finally
                {
                    if (conn.State == ConnectionState.Open)
                    {
                        conn.Close();
                    }
                }
            }
            else
            {
                loadSucess = false;
            }           

            if (loadSucess)
            {
                MessageBox.Show("商店道具信息更新成功！\r\n\r\n当前导表操作已经完成，继续的话会有bug哦~", "更新商店道具信息",
                                MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            else
            {
                MessageBox.Show("商店道具信息更新失败！\r\n\r\n当前导表操作已经完成，继续的话会有bug哦~", "更新商店道具信息",
                                MessageBoxButtons.OK, MessageBoxIcon.Information);
            }

            return loadSucess;
        }

        /// <summary>
        /// 获取商店ID
        /// </summary>
        /// <param name="folderName">文件夹名</param>
        /// <param name="fileName">文件名</param>
        /// <returns>商店ID</returns>
        private string GetShopTemplateID(string folderName, string fileName)
        {
            string shopID = "0";
            string configFile = string.Format("{0}\\{1}", folderName, fileName);

            DataRow[] rows = shopListTable.Select(string.Format("ConfigFile = '{0}'", configFile));

            if (rows.Length > 0)
            {
                shopID = rows[0]["ShopTemplateID"].ToString();
            }

            return shopID;
        }

        /// <summary>
        /// 获取商店道具表所在的目录
        /// </summary>
        /// <returns>商店道具表所在的目录</returns>
        private string GetLoadPath()
        {
            string resultFolder = null;
            FolderBrowserDialog openFolderDialog1 = new FolderBrowserDialog();
            openFolderDialog1.Description = "请选择商店道具表所在的目录";
            openFolderDialog1.SelectedPath = path;

            if(openFolderDialog1.ShowDialog() == DialogResult.OK)
            {
                resultFolder = openFolderDialog1.SelectedPath;
            }

            return resultFolder;
        }      

        /// <summary>
        /// 获取道具信息
        /// </summary>
        /// <param name="cmd">sql命令</param>
        /// <param name="tabType">道具表名</param>
        /// <param name="itemIndex">道具编号</param>
        /// <returns>道具信息</returns>
        private Hashtable GetItemInfo(SqlCommand cmd, string tabType, string itemIndex)
        {
            Hashtable infoTable = new Hashtable();
            List<string> fieldList = new List<string>();
            string tableName = "Other";

            if (tabType != "" && itemIndex != "")
            {
                switch (tabType)
                {
                    case "Other":
                        {
                            tableName = "Other";
                            break;
                        }
                    case "CustomWeapon":
                        {
                            tableName = "item_Custom_Weapon";
                            break;
                        }
                    case "CustomArmor":
                        {
                            tableName = "item_Custom_Armor";
                            break;
                        }
                    case "CustomTrinket":
                        {
                            tableName = "Custom_Trinket";
                            break;
                        }
                }

                if (tableName == "Other")
                {
                    fieldList.Add("Name");
                    fieldList.Add("Genre");
                    fieldList.Add("SubType");
                    fieldList.Add("Quality");
                }
                else
                {
                    fieldList.Add("Name");
                    fieldList.Add("Genre");
                    fieldList.Add("SubType");
                    fieldList.Add("Level");
                    fieldList.Add("Quality");
                }                

                foreach (string s in fieldList)
                {
                    string sqlString = string.Format("SELECT {0} FROM {1} WHERE ID = '{2}'", s, tableName, itemIndex);
                    cmd.CommandText = sqlString;
                    object executeResult = cmd.ExecuteScalar();
                    
                    if (executeResult != null)
                    {
                        infoTable[s] = executeResult.ToString();
                    }
                }
            }

            return infoTable;
        }

        /// <summary>
        /// 获取文件文本内容
        /// </summary>
        /// <param name="filePath">文件路径</param>
        /// <returns>文本内容</returns>
        private string GetFileContent(string filePath)
        {
            string content = null;

            if (File.Exists(filePath))
            {
                StreamReader reader = new StreamReader(filePath, Encoding.GetEncoding("gb2312"));
                content = reader.ReadToEnd();
                reader.Close();
            }
            else
            {
                MessageBox.Show(string.Format("文件{0}不存在！", filePath), "读取数据文件",
                                MessageBoxButtons.OK, MessageBoxIcon.Information);
            }

            return content;
        }

        /// <summary>
        /// 创建数据表
        /// </summary>
        /// <param name="content">内容</param>
        /// <returns>数据表</returns>
        private DataTable CreateDataTable(string content)
        {
            DataTable table = new DataTable();
            List<string> fieldList = new List<string>();

            bool firstLine = true;
            string[] lines = content.Split(new char[] { '\n' });

            foreach (string s in lines)
            {
                string line = s.TrimEnd(new char[] { '\r' });

                if (line != "") // 跳过空的数据行
                {
                    string[] data = line.Split(new char[] { '\t' });

                    if (firstLine)
                    {
                        firstLine = false;

                        for (int i = 0; i < data.Length; i++)
                        {
                            DataColumn dataColumn = new DataColumn(data[i]);
                            table.Columns.Add(dataColumn);
                            fieldList.Add(data[i]);
                        }
                    }
                    else
                    {
                        DataRow dataRow = table.NewRow();

                        for (int i = 0; i < data.Length; i++)
                        {
                            dataRow[i] = data[i];
                        }

                        table.Rows.Add(dataRow);
                    }
                }

            }

            return table;
        }

        /// <summary>
        /// 更新商店道具信息
        /// </summary>
        /// <param name="cmd">sql命令</param>
        /// <param name="fileName">文件名</param>
        /// <param name="folderName">文件夹名</param>        
        private void UpdateShopItem(SqlCommand cmd, string fileName, string folderName)
        {
            string filePath = Path.Combine(rootFolder, string.Format("{0}\\{1}", folderName, fileName));            
            string content = GetFileContent(filePath);
            DataTable itemTable = CreateDataTable(content);

            for (int i = 0; i < itemTable.Rows.Count; i++)
            {
                if (i >= 100)
                {
                    MessageBox.Show(string.Format("商店{0}的道具已经超过上限100！\r\n\r\n超出上限的道具将不会更新。", filePath), "更新道具信息",
                                                  MessageBoxButtons.OK, MessageBoxIcon.Information);
                    break;
                }

                DataRow dataRow = itemTable.Rows[i];
                string tabType = dataRow["TabType"].ToString();
                string itemIndex = dataRow["ItemIndex"].ToString();
                string durability = dataRow["Durability"].ToString();
                string shopTemplateID = GetShopTemplateID(folderName, fileName);
                string sqlString = null;
                DataRow itemRow = GetShopItemRow(tabType, itemIndex, durability, folderName, fileName);

                if (itemRow == null) // 需要插入道具
                {
                    sqlString = string.Format("INSERT INTO tbl_npc_shopitem (TabType, ItemIndex, Durability, foldername, filename, ShopTemplateID) VALUES ('{0}', '{1}', '{2}', '{3}', '{4}', '{5}')",
                                              tabType, itemIndex, durability, folderName, fileName, shopTemplateID);
                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();

                    Hashtable itemInfo = GetItemInfo(cmd, tabType, itemIndex);

                    foreach (string key in itemInfo.Keys)
                    {
                        string itemValue = itemInfo[key] as string;

                        if (!string.IsNullOrEmpty(itemValue))
                        {
                            sqlString = string.Format("UPDATE tbl_npc_shopitem SET [{0}] = '{1}' WHERE TabType = '{2}' AND ItemIndex = '{3}' AND Durability = '{4}'",
                                                      key, itemValue, tabType, itemIndex, durability);
                            cmd.CommandText = sqlString;
                            cmd.ExecuteNonQuery();
                        }
                    }
                }

                foreach (DataColumn dataColumn in itemTable.Columns)
                {
                    string columnName = dataColumn.ColumnName;
                    string newValue = dataRow[columnName].ToString();

                    if (columnName != "TabType" && columnName != "ItemIndex" && columnName != "Durability" && columnName != "foldername" && columnName != "filename")
                    {
                        if (itemRow == null)
                        {
                            if (newValue == "")
                            {
                                sqlString = string.Format("UPDATE tbl_npc_shopitem SET [{0}] = NULL WHERE TabType = '{1}' AND ItemIndex = '{2}' AND Durability = '{3}' AND foldername = '{4}' AND filename = '{5}'",
                                                          columnName, tabType, itemIndex, durability, folderName, fileName);
                            }
                            else
                            {
                                sqlString = string.Format("UPDATE tbl_npc_shopitem SET [{0}] = '{1}' WHERE TabType = '{2}' AND ItemIndex = '{3}' AND Durability = '{4}' AND foldername = '{5}' AND filename = '{6}'",
                                                          columnName, newValue, tabType, itemIndex, durability, folderName, fileName);
                            }

                            cmd.CommandText = sqlString;
                            cmd.ExecuteNonQuery();
                        }
                        else
                        {
                            string oldValue = itemRow[columnName].ToString();

                            if (oldValue != newValue)
                            {
                                if (newValue == "")
                                {
                                    sqlString = string.Format("UPDATE tbl_npc_shopitem SET [{0}] = NULL WHERE TabType = '{1}' AND ItemIndex = '{2}' AND Durability = '{3}' AND foldername = '{4}' AND filename = '{5}'",
                                                              columnName, tabType, itemIndex, durability, folderName, fileName);
                                }
                                else
                                {
                                    sqlString = string.Format("UPDATE tbl_npc_shopitem SET [{0}] = '{1}' WHERE TabType = '{2}' AND ItemIndex = '{3}' AND Durability = '{4}' AND foldername = '{5}' AND filename = '{6}'",
                                                              columnName, newValue, tabType, itemIndex, durability, folderName, fileName);
                                }

                                cmd.CommandText = sqlString;
                                cmd.ExecuteNonQuery();
                            }
                        }
                    }
                }
            }
        }

        /// <summary>
        /// 获取商店道具数据行
        /// </summary>
        /// <param name="tabType">道具表名</param>
        /// <param name="itemIndex">道具ID</param>
        /// <param name="durability">道具耐久度</param>
        /// <param name="folderName">文件夹名</param>
        /// <param name="fileName">文件名</param>
        /// <returns>商店道具数据行</returns>
        private DataRow GetShopItemRow(string tabType, string itemIndex, string durability, string folderName, string fileName)
        {
            DataRow dataRow = null;

            DataRow[] rows = shopItemTable.Select(string.Format("TabType = '{0}' AND ItemIndex = '{1}' AND Durability = '{2}' AND foldername = '{3}' AND filename = '{4}'",
                                                                tabType, itemIndex, durability, folderName, fileName));
            if (rows.Length > 0)
            {
                dataRow = rows[0];
            }

            return dataRow;
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