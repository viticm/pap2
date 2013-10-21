using System;
using System.Collections.Generic;
using System.Text;
using System.Data.SqlClient;
using System.Collections;
using System.Data;
using System.Windows.Forms;
using System.IO;

namespace ShopItemIO
{
    class ExportShopList
    {
        private SqlConnection conn; // sql连接
        private ArrayList fields; // 列数据链表
        private string[] values; // 行数据链表
        private string path; // 导出路径
        private List<string> excludeColumnNameList = new List<string>(); // 不导出的列列名链表
        private List<int> excludeColumnIndexList = new List<int>(); // 不导出的列序号链表
        private Hashtable itemTable = new Hashtable(); // 保存物品数据的hash表
        private string fileName; // 导出文件的文件名

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="conn">sql连接</param>
        /// <param name="fields">列数据链表</param>
        /// <param name="values">行数据链表</param>
        /// <param name="path">导出路径</param>
        public ExportShopList(SqlConnection conn, ArrayList fields, string[] values, string path)
        {
            this.conn = conn;
            this.fields = fields;
            this.values = values;
            this.path = path;
        }

        /// <summary>
        /// 定义不导出的列
        /// </summary>
        /// <param name="columnName">不导出列的列名</param>
        public void AddExcludeColumn(string columnName)
        {
            excludeColumnNameList.Add(columnName);
        }

        /// <summary>
        /// 定义导出文件的文件名
        /// </summary>
        /// <param name="fileName">文件名</param>
        public void SetFileName(string fileName)
        {
            this.fileName =fileName;
        }

        /// <summary>
        /// 检查商店道具的商店ID一致性
        /// </summary>
        private bool CheckCoordinate()
        {
            bool coordinate = true; // 是否一致
            Hashtable pathTable = new Hashtable(); // 保存商店路径的哈希表
            List<string> errorList = new List<string>(); // 出错信息提示的链表
            List<string> pathList = new List<string>(); // 出错路径的链表

            try
            {
                if(conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }

                string folderName;
                string fileName;
                string shopTemplateID;
                string path;

                string sqlString = string.Format("SELECT foldername, filename, ShopTemplateID FROM {0}", "tbl_npc_shopitem");
                DataTable itemTable = GetDataTable(sqlString);
                foreach(DataRow r in itemTable.Rows)
                {
                    folderName = r["foldername"].ToString();
                    fileName = r["filename"].ToString();
                    shopTemplateID = r["ShopTemplateID"].ToString();

                    path = folderName + "\\" + fileName;
                    if(pathTable[path] == null)
                    {
                        pathTable[path] = shopTemplateID;
                    }
                    else
                    {
                        if(pathTable[path].ToString() != shopTemplateID) // 同路径下有不同的商店ID
                        {
                            if(pathTable[path].ToString() == "0") // 商店ID为0时自动更新
                            {
                                pathTable[path] = shopTemplateID;
                            }
                            else
                            {
                                if(shopTemplateID != "0")
                                {
                                    if(!pathList.Contains(path))
                                    {
                                        pathList.Add(path);
                                        errorList.Add(string.Format("路径{0}下有冲突的商店ID!", path));
                                    }
                                }
                            }
                        }
                    }
                }
            }
            catch (SqlException ex)
            {
                MessageBox.Show("在检查商店道具的商店ID一致性时产生sql异常: " + ex.ToString());
            }
            finally
            {
                if(conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
            }

            coordinate = (errorList.Count == 0);
            if(!coordinate) // 商店道具的商店ID出现不一致的情况
            {
                string message = "";
                foreach(string s in errorList)
                {
                    message = message + s + "\n";
                }
                MessageBox.Show(message, "商店道具ShopTemplateID一致性检查", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

            return coordinate;
        }

        /// <summary>
        /// 导出商店对象表文件
        /// </summary>
        public void ExportNpcShopList()
        {
            if(!CheckCoordinate()) // 没有通过商店道具ShopTemplateID一致性检查
            {
                return;
            }

            List<string> outputList = new List<string>(); // 导出的数据链表
            List<string> existShopList = new List<string>(); // 已经存在的商店链表
            int folderNameIndex = 0; // 商店道具文件夹的序号
            int fileNameIndex = 0; // 商店道具文件的序号
            int shopTemplateIDIndex = 0; // 商店ID的序号 
            int idIndex = 0; // 道具ID的序号
            string fieldName; // 字段名
            string[] data; // 行数据

            for (int i = 0; i < fields.Count; i++)
            {
                fieldName = fields[i].ToString().TrimEnd(new char[] {'\r', '\n'});
                switch(fieldName)
                {
                    case "ShopTemplateID":
                        {
                            shopTemplateIDIndex = i;
                            break;
                        }
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

                SqlCommand cmd = conn.CreateCommand();
                string sqlString = string.Format("SELECT * FROM {0} ORDER BY {1}", "tbl_npc_shoplist", "ShopTemplateID");
                DataTable shopListTable = GetDataTable(sqlString);
                DataRow defaultRow = null; // 默认行
                int maxShopIndex = 1; // 最大商店ID
                if(shopListTable.Rows.Count > 0)
                {
                    maxShopIndex = int.Parse(shopListTable.Rows[shopListTable.Rows.Count - 1]["ShopTemplateID"].ToString()) + 1;
                    defaultRow = shopListTable.Rows[0];
                }

                foreach(string s in values)
                {
                    if(s != "") // 跳过最后的空行
                    {
                        data = s.Split('\t');
                        string fileName = data[fileNameIndex];
                        string folderName = data[folderNameIndex];
                        string path = Path.Combine(folderName, fileName);
                        string shopTemplateID = data[shopTemplateIDIndex];
                        string id = data[idIndex];

                        DataRow row = shopListTable.Rows.Find(shopTemplateID);
                        if((shopTemplateID == "0") && (row != null)) // shopTemplateID为0的行非法
                        {
                            shopListTable.Rows.Remove(row);
                            row = null; // 跳过处理流程
                        }

                        if(row == null) // 没有找到对应的数据行
                        {
                            DataRow[] rows = shopListTable.Select(string.Format("ConfigFile = '{0}'", path));
                            if(rows.Length > 0) // 该路径对应的商店已经存在
                            {
                                string shopID = rows[0]["ShopTemplateID"].ToString();
                                sqlString = string.Format("UPDATE {0} SET ShopTemplateID = '{1}' WHERE id = '{2}'", "tbl_npc_shopitem", shopID, id);
                                cmd.CommandText = sqlString;
                                cmd.ExecuteNonQuery();
                            }
                            else
                            {
                                if (defaultRow != null) // 存在默认行
                                {
                                    string canRepair = "0"; // 将布尔值转换为0和1
                                    if((bool)defaultRow["CanRepair"])
                                    {
                                        canRepair = "1";
                                    }

                                    sqlString = string.Format("INSERT INTO {0} (ShopTemplateID, ConfigFile, ShopName, CanRepair) VALUES ('{1}', '{2}', '{3}', '{4}')",
                                    "tbl_npc_shoplist", maxShopIndex.ToString(), path, defaultRow["ShopName"], canRepair);
                                    cmd.CommandText = sqlString;
                                    cmd.ExecuteNonQuery();

                                    DataRow newRow = shopListTable.NewRow();
                                    newRow["ShopTemplateID"] = maxShopIndex.ToString();
                                    newRow["ConfigFile"] = path;
                                    newRow["ShopName"] = defaultRow["ShopName"];
                                    newRow["CanRepair"] = defaultRow["CanRepair"];
                                    shopListTable.Rows.Add(newRow);
                                    existShopList.Add(maxShopIndex.ToString()); // 记录当前存在的商店ID

                                }
                                else // 不存在默认行
                                {
                                    sqlString = string.Format("INSERT INTO {0} (ShopTemplateID, ConfigFile) VALUES ('{1}', '{2}')",
                                    "tbl_npc_shoplist", maxShopIndex.ToString(), path);
                                    cmd.CommandText = sqlString;
                                    cmd.ExecuteNonQuery();

                                    DataRow newRow = shopListTable.NewRow();
                                    newRow["ShopTemplateID"] = maxShopIndex.ToString();
                                    newRow["ConfigFile"] = path;
                                    shopListTable.Rows.Add(newRow);
                                    existShopList.Add(maxShopIndex.ToString()); // 记录当前存在的商店ID
                                }

                                if(shopTemplateID == "0") // 被复制粘贴过或者新建的道具，并且它对应的商店还不存在，需要更新道具表
                                {
                                    sqlString = string.Format("UPDATE {0} SET ShopTemplateID = '{1}' WHERE id = '{2}'",
                                        "tbl_npc_shopitem", maxShopIndex.ToString(), id);
                                    cmd.CommandText = sqlString;
                                    cmd.ExecuteNonQuery();
                                }

                                maxShopIndex++;
                            }
                        }
                        else
                        {
                            if((row["ConfigFile"].ToString() != path)) // 需要更新对应商店的信息
                            {
                                sqlString = string.Format("UPDATE {0} SET ConfigFile = '{1}' WHERE ShopTemplateID = '{2}'",
                                    "tbl_npc_shoplist", path, shopTemplateID);
                                cmd.CommandText = sqlString;
                                cmd.ExecuteNonQuery();

                                row["ConfigFile"] = path;
                            }
                            existShopList.Add(shopTemplateID); // 记录当前存在的商店ID
                        }
                    }
                }

                // 清理没有被引用到的商店数据
                List<DataRow> deleteRowList = new List<DataRow>(); // 需要删除的数据行链表
                foreach(DataRow r in shopListTable.Rows)
                {
                    string shopTemplateID = r["ShopTemplateID"].ToString();
                    if(!existShopList.Contains(shopTemplateID))
                    {
                        sqlString = string.Format("DELETE FROM {0} WHERE ShopTemplateID = '{1}'", "tbl_npc_shoplist",
                            shopTemplateID);
                        cmd.CommandText = sqlString;
                        cmd.ExecuteNonQuery();
                        deleteRowList.Add(r);
                    }
                }

                foreach(DataRow r in deleteRowList)
                {
                    shopListTable.Rows.Remove(r);
                }

                outputList.Add("ShopTemplateID\tShopName\tConfigFile\tCanRepair");
                foreach(DataRow r in shopListTable.Rows)
                {
                    object[] itemArray = r.ItemArray;
                    string item;
                    string value = "";
                    foreach(object o in itemArray)
                    {
                        item = o.ToString();
                        switch(item)
                        {
                            case "True":
                                {
                                    item = "1";
                                    break;
                                }
                            case "False":
                                {
                                    item = "0";
                                    break;
                                }
                        }
                        value += string.Format("{0}\t", item);
                    }
                    outputList.Add(value.TrimEnd('\t'));
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("在更新商店对象表时产生异常: " + ex.ToString());
            }
            finally
            {
                if (conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
            }

            SaveDataToFile(outputList);
        }

        /// <summary>
        /// 更新商店道具信息
        /// </summary>
        /// <param name="itemID">事件发送者</param>
        /// <param name="itemType">事件参数</param>
        /// <param name="infoList">信息链表</param>
        private void UpdateShopItemInfo(string itemID, string itemType, List<string> infoList)
        {
            try
            {
                if (conn.State == ConnectionState.Closed)
                {
                    conn.Open();
                }

                SqlCommand cmd = conn.CreateCommand();
                string sqlString = string.Format("UPDATE tbl_npc_shopitem SET Name = '{0}', Genre = '{1}', SubType = '{2}', Level = '{3}', Quality = '{4}' WHERE ItemIndex = '{5}' AND TabType = '{6}'",
                                                 infoList[0], infoList[1], infoList[2], infoList[3], infoList[4], itemID, itemType);
                cmd.CommandText = sqlString;
                cmd.ExecuteNonQuery();
            }
            catch (SqlException ex)
            {
                MessageBox.Show("在更新商店道具信息时产生sql异常：" + ex.Message, "更新商店道具信息",
                                MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            finally
            {
                if (conn.State == ConnectionState.Open)
                {
                    conn.Close();
                }
            }            
        }

        /// <summary>
        /// 导出商店道具列表文件
        /// </summary>
        public void ExportSupplyList()
        {
            List<string> dataList = new List<string>(); // 导出数据的链表

            // 添加列数据
            string headLine = string.Empty; // 列数据行
            string fieldName; // 列名
            int countIndex = 0; // 引用计数列的序号
            int itemIDIndex = 0; // 物品ID列的序号
            int itemTypeIndex = 0; // 物品类型列的序号
            int nameIndex = 0; // Name的序号
            int genreIndex = 0; // Genre的序号
            int subTypeIndex = 0; // SubType的序号
            int levelIndex = 0; // Level的序号
            int qualityIndex = 0; // Quality的序号
            int durabilityIndex = 0; // Qurability的序号

            for (int i = 0, j = 0; i < fields.Count; i++)
            {
                fieldName = fields[i].ToString().Trim(new char[] {'\r', '\n'}); // 去掉可能存在的换行字符
                if (!excludeColumnNameList.Contains(fieldName)) // 该列没有被定义为不导出
                {
                    switch(fieldName)
                    {
                        case "UsedCount":
                            {
                                countIndex = j;
                                break;
                            }
                        case "ItemIndex":
                            {
                                itemIDIndex = j;
                                break;
                            }
                        case "TabType":
                            {
                                itemTypeIndex = j;
                                break;
                            }
                        case "Name":
                            {
                                nameIndex = j;
                                break;
                            }
                        case "Genre":
                            {
                                genreIndex = j;
                                break;
                            }
                        case "SubType":
                            {
                                subTypeIndex = j;
                                break;
                            }
                        case "Level":
                            {
                                levelIndex = j;
                                break;
                            }
                        case "Quality":
                            {
                                qualityIndex = j;
                                break;
                            }
                        case "Durability":
                            {
                                durabilityIndex = j;
                                break;
                            }
                    }
                 
                    headLine += fieldName + "\t";
                    j++;
                }
                else
                {
                    excludeColumnIndexList.Add(i);
                }
            }

            dataList.Add(headLine.TrimEnd('\t'));

            // 添加行数据
            string key;
            string[] data;
            string[] newData;
            List<string> itemInfo;
            ItemData itemData;

            foreach(string s in values)
            {
                if(s != "")
                {
                    data = s.Split(new char[] { '\t' });
                    newData = new string[data.Length - excludeColumnIndexList.Count];
                    for (int i = 0, j = 0; i < data.Length; i++)
                    {
                        if (!excludeColumnIndexList.Contains(i)) // 该列没有被定义为不导出
                        {
                            newData[j] = data[i];
                            j++;
                        }
                    }
                    key = newData[itemIDIndex] + "," + newData[itemTypeIndex] + "," + newData[durabilityIndex];
                    itemInfo = GetItemInfo(newData[itemIDIndex], newData[itemTypeIndex]);
                    newData[nameIndex] = itemInfo[0];
                    newData[genreIndex] = itemInfo[1];
                    newData[subTypeIndex] = itemInfo[2];
                    newData[levelIndex] = itemInfo[3];
                    newData[qualityIndex] = itemInfo[4];

                    // 更新商店道具信息
                    UpdateShopItemInfo(newData[itemIDIndex], newData[itemTypeIndex], itemInfo);

                    if (itemTable[key] != null)
                    {
                        itemData = itemTable[key] as ItemData;
                        itemData.AddItem();
                    }
                    else
                    {
                        itemData = new ItemData(newData, countIndex);
                        itemTable[key] = itemData;
                    }
                }
            }

            foreach(string s in itemTable.Keys)
            {
                itemData = itemTable[s] as ItemData;
                dataList.Add(itemData.GetData());
            }

            SaveDataToFile(dataList);
        }

        /// <summary>
        /// 获取道具信息链表
        /// </summary>
        /// <param name="id">道具id</param>
        /// <param name="type">道具类型</param>
        /// <returns>道具信息链表</returns>
        private List<string> GetItemInfo(string id, string type)
        {
            List<string> infoList = new List<string>();
            string tableName = "Other";

            if (id == "" || type == "")
            {
                return null;
            }

            switch (type)
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

            try
            {
                if (conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }

                string sqlString = string.Format("SELECT * FROM {0} WHERE ID = '{1}'", tableName, id);
                DataTable table = GetDataTable(sqlString);

                if (table.Rows.Count > 0)
                {
                    DataRow row = table.Rows[0];
                    if (tableName == "Other") // Other表里没有Level字段
                    {
                        infoList.Add(row["Name"].ToString());
                        infoList.Add(row["Genre"].ToString());
                        infoList.Add(row["SubType"].ToString());
                        infoList.Add("");
                        infoList.Add(row["Quality"].ToString());
                    }
                    else
                    {
                        infoList.Add(row["Name"].ToString());
                        infoList.Add(row["Genre"].ToString());
                        infoList.Add(row["SubType"].ToString());
                        infoList.Add(row["Level"].ToString());
                        infoList.Add(row["Quality"].ToString());
                    }
                }
                else
                {
                    for (int i = 0; i < 5; i++)
                    {
                        infoList.Add("");
                    }
                }
            }
            catch (SqlException ex)
            {
                MessageBox.Show("在读取物品信息时产生sql异常: " + ex.ToString());
            }
            finally
            {
                if (conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
            }

            return infoList;
        }

        /// <summary>
        /// 将数据写入文件
        /// </summary>
        private void SaveDataToFile(List<string> dataList)
        {
            try
            {
                string filePath = Path.Combine(path, fileName);
                FileInfo fi = new FileInfo(filePath);
                StreamWriter sw;

                if (!fi.Exists) // 文件不存在
                {
                    if(!Directory.Exists(path)) // 目录不存在
                    {
                        Directory.CreateDirectory(path); // 先创建目录，再创建文件
                    }
                    FileStream fs = fi.Create();
                    fs.Close();
                    sw = new StreamWriter(filePath, false, Encoding.GetEncoding("gb2312"));
                }
                else // 文件已经存在
                {
                    sw = new StreamWriter(filePath, false, Encoding.GetEncoding("gb2312"));
                }

                foreach(string s in dataList)
                {
                    sw.WriteLine(s);
                }
                sw.Close();
                MessageBox.Show(string.Format("文件 {0} 已经成功导出!", filePath));
            }
            catch(IOException ex)
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

        /// <summary>
        /// 保存物品数据的内部类
        /// </summary>
        private class ItemData
        {
            private string[] data; // 物品数据数组
            private int count = 1; // 引用计数
            private int countIndex; // 引用计数列的序号

            /// <summary>
            /// 构造函数
            /// </summary>
            /// <param name="data">物品数据数组</param>
            /// <param name="countIndex">引用计数列的序号</param>
            public ItemData(string[] data, int countIndex)
            {
                this.data = data;
                this.countIndex = countIndex;
            }
            
            /// <summary>
            /// 引用计数加一
            /// </summary>
            public void AddItem()
            {
                count++;
            }

            /// <summary>
            /// 获取物品数据行
            /// </summary>
            /// <returns>物品数据行</returns>
            public string GetData()
            {                
                StringBuilder value = new StringBuilder();

                for(int i = 0; i < data.Length; i++)
                {
                    if (i == countIndex)
                    {
                        value.Append(string.Format("{0}\t", count.ToString()));                        
                    }
                    else
                    {
                        value.Append(string.Format("{0}\t", data[i]));
                    }     
                }

                value.Remove(value.Length - 1, 1);

                return value.ToString();
            }
        }
    }
}
