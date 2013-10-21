using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;
using System.IO;
using System.Windows.Forms;
using System.Data.SqlClient;
using System.Data;

namespace ShopItemIO
{
    class ExportMultiFile
    {
        private ArrayList fields; // 列名链表
        private string[] values; // 数据行数组
        private List<string> excludeFields = new List<string>(); // 不导出的列名
        private List<int> excludeIndexList = new List<int>(); // 不导出的序号链表
        private Hashtable fileTable = new Hashtable(); // 记录导出文件数据的hash表
        private string path = null; // 文件保存路径
        private SqlConnection conn; // sql连接
        private List<string> exportedFiles = new List<string>(); // 已经导出的文件名列表
        private string exportPath; // 导出文件的路径

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="conn">sql连接</param>
        /// <param name="fields">列名链表</param>
        /// <param name="values">数据行数组</param>
        public ExportMultiFile(SqlConnection conn, ArrayList fields, string[] values)
        {
            this.conn = conn;
            this.fields = fields;
            this.values = values;
        }

        /// <summary>
        /// 导出文件的路径属性
        /// </summary>
        public string ExportPath
        {
            get
            {
                return exportPath;
            }
        }

        /// <summary>
        /// 设置不导出的列
        /// </summary>
        /// <param name="s">不导出的列名</param>
        public void AddExcludeFields(string s)
        {
            excludeFields.Add(s);
        }

        /// <summary>
        /// 设置文件保存路径
        /// </summary>
        /// <param name="path">文件保存路径</param>
        public void SetPath(string path)
        {
            this.path = path;
            this.exportPath = path;
        }

        /// <summary>
        /// 导出文件
        /// </summary>
        /// <param name="folderName">包含文件夹名的列</param>
        /// <param name="fileName">包含文件名的列</param>
        /// <returns>是否导出成功</returns>
        public bool ExportFiles(string folderName, string fileName)
        {
            bool exportSuccess = false;
            string resultFolder = "";
            FolderBrowserDialog openFolderDialog1 = new FolderBrowserDialog();
            openFolderDialog1.Description = "请选择导出文件的保存目录";
            openFolderDialog1.ShowNewFolderButton = true;

            if (path == null)
            {
                openFolderDialog1.RootFolder = Environment.SpecialFolder.MyComputer;
            }
            else
            {
                openFolderDialog1.SelectedPath = path;
            }

            if (openFolderDialog1.ShowDialog() == DialogResult.OK)
            {
                resultFolder = openFolderDialog1.SelectedPath + "\\";
                exportPath = openFolderDialog1.SelectedPath;

                string headline = string.Empty;
                int fileNameIndex = -1;
                int folderNameIndex = -1;

                for (int i = 0; i < fields.Count; i++)
                {
                    string field = (fields[i] as string).TrimEnd(new char[] { '\r', '\n' });
                    if (excludeFields.Contains(field))
                    {
                        excludeIndexList.Add(i);
                    }
                    else
                    {
                        headline = headline + field + "\t";
                    }

                    if (field == fileName)
                    {
                        fileNameIndex = i;
                    }
                    else if(field == folderName)
                    {
                        folderNameIndex = i;
                    }
                }

                if (fileNameIndex == -1 || folderNameIndex == -1) // 没有找到导出文件名所在列
                {
                    MessageBox.Show("没有找到导出文件夹和文件名所在列!");
                    return false;
                }

                headline = headline.TrimEnd(new char[] { '\r', '\n', '\t' }); // 去掉冗余信息

                foreach (string s in values) // 添加所有数据行到hash表中
                {
                    if (s == "") // 忽略空行
                    {
                        continue;
                    }
                    string[] data = s.Split('\t');
                    string fullPath = Path.Combine(resultFolder, Path.Combine(data[folderNameIndex], data[fileNameIndex]));

                    if (fileTable[fullPath] != null)
                    {
                        List<string> lines = fileTable[fullPath] as List<string>;
                        string t = string.Empty;
                        for (int i = 0; i < data.Length; i++)
                        {
                            if (!excludeIndexList.Contains(i))
                            {
                                t = t + data[i] + "\t";
                            }
                        }
                        //lines.Add(t.TrimEnd('\t'));
                        lines.Add(t.Substring(0, t.Length - 1));
                    }
                    else
                    {
                        List<string> lines = new List<string>();
                        lines.Add(headline);
                        string t = string.Empty;
                        for (int i = 0; i < data.Length; i++)
                        {
                            if (!excludeIndexList.Contains(i))
                            {
                                t = t + data[i] + "\t";
                            }
                        }
                        //lines.Add(t.TrimEnd('\t'));
                        lines.Add(t.Substring(0, t.Length - 1));
                        fileTable[fullPath] = lines;
                    }
                }

                List<string> fileNameList = new List<string>();
                foreach (string s in fileTable.Keys)
                {
                    fileNameList.Add(s);
                }

                // 导出文件
                foreach (string s in fileTable.Keys)
                {
                    if (fileNameList.Contains(s))
                    {
                        List<string> lines = fileTable[s] as List<string>;
                        string tempValue;

                        // 对行数据进行排序
                        bool finished = true;

                        if (lines.Count > 100)
                        {
                            MessageBox.Show(string.Format("商店{0}的道具已经超过上限100！\r\n\r\n该文件将不会被导出。", s), "更新道具信息",
                                                          MessageBoxButtons.OK, MessageBoxIcon.Information);
                            continue;
                        }

                        for (int i = 0; i < lines.Count - 1; i++)
                        {
                            finished = true;

                            for (int j = 1; j < lines.Count - i - 1; j++)
                            {
                                if (IsLargeThan(lines[j], lines[j + 1]))
                                {
                                    tempValue = lines[j + 1];
                                    lines[j + 1] = lines[j];
                                    lines[j] = tempValue;
                                    finished = false;
                                }
                            }

                            if (finished) // 已经不需要再排序
                            {
                                break;
                            }
                        }

                        SaveData(s, lines);
                    }
                }

                // 显示导出文件列表
                string message = "文件已经成功导出！";
                MessageBox.Show(message);
                exportSuccess = true;
            }

            return exportSuccess;
        }

        /// <summary>
        /// 获取道具信息
        /// </summary>
        /// <param name="tableName">道具表名</param>
        /// <param name="id">道具id</param>
        /// <param name="fieldName">道具字段</param>
        /// <returns>道具字段</returns>
        private string GetInfo(string tableName, string id, string fieldName)
        {
            string value = null;

            try
            {
                if (conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }

                SqlCommand cmd = conn.CreateCommand();
                string sqlString = string.Format("SELECT {0} FROM {1} WHERE ID = '{2}'", fieldName, tableName, id);
                cmd.CommandText = sqlString;
                object o = cmd.ExecuteScalar();
                if (o != null)
                {
                    value = o.ToString();
                }
            }
            catch (SqlException ex)
            {
                MessageBox.Show("在读取道具信息时产生sql异常: " + ex.ToString());
            }
            finally
            {
                if (conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
            }

            if (value == "")
            {
                value = null;
            }

            return value;
        }

        /// <summary>
        /// 检查数值字符串a的权值是否比b大
        /// </summary>
        /// <param name="a">输入字符串</param>
        /// <param name="b">输入字符串</param>
        /// <returns>a的权值是否比b大</returns>
        private bool IsLargeThan(string a, string b)
        {
            bool result = false;

            string[] arrayA = a.Split(new char[] { '\t' });
            string[] arrayB = b.Split(new char[] { '\t' });

            // 先比较ReputeLevel
            int reputeLevelA = 0;
            int reputeLevelB = 0;

            if(arrayA[5] != "")
            {
                reputeLevelA = int.Parse(arrayA[5]);
            }

            if(arrayB[5] != "")
            {
                reputeLevelB = int.Parse(arrayB[5]);
            }

            if(reputeLevelA > reputeLevelB)
            {
                result = true;
            }
            else if(reputeLevelA < reputeLevelB)
            {
                result = false;
            }
            else
            {
                // 比较TabType
                int tabTypeNumberA = 0;
                int tabTypeNumberB = 0;

                string tabTypeA = arrayA[0];
                switch (tabTypeA)
                {
                    case "Other":
                        {
                            tabTypeNumberA = 1;
                            break;
                        }
                    case "CustomWeapon":
                        {
                            tabTypeNumberA = 2;
                            break;
                        }
                    case "CustomArmor":
                        {
                            tabTypeNumberA = 3;
                            break;
                        }
                    case "CustomTrinket":
                        {
                            tabTypeNumberA = 4;
                            break;
                        }
                }

                string tabTypeB = arrayB[0];
                switch (tabTypeB)
                {
                    case "Other":
                        {
                            tabTypeNumberB = 1;
                            break;
                        }
                    case "CustomWeapon":
                        {
                            tabTypeNumberB = 2;
                            break;
                        }
                    case "CustomArmor":
                        {
                            tabTypeNumberB = 3;
                            break;
                        }
                    case "CustomTrinket":
                        {
                            tabTypeNumberB = 4;
                            break;
                        }
                }

                if (tabTypeNumberA > tabTypeNumberB)
                {
                    result = true;
                }
                else if (tabTypeNumberA < tabTypeNumberB)
                {
                    result = false;
                }
                else
                {
                    if (tabTypeNumberA == 1) // 道具表内的比较
                    {
                        // 比较_CATEGORY
                        string itemIndexA = arrayA[1];
                        string itemIndexB = arrayB[1];
                        string catA = GetInfo("Other", itemIndexA, "_CATEGORY");
                        string catB = GetInfo("Other", itemIndexB, "_CATEGORY");
                        string _strlevelA = GetInfo("Other", itemIndexA, "RequireLevel");
                        string _strlevelB = GetInfo("Other", itemIndexB, "RequireLevel");
                        int levelA = _strlevelA != null ? Convert.ToInt32(_strlevelA) : 0;
                        int levelB = _strlevelB != null ? Convert.ToInt32(_strlevelB) : 0;

                        if (catA == null)
                        {
                            result = false;
                        }
                        else if (catB == null)
                        {
                            result = true;
                        }
                        else if (catA.CompareTo(catB) > 0)
                        {
                            result = true;
                        }
                        else if (catA.CompareTo(catB) < 0)
                        {
                            result = false;
                        }
                        else if (levelA != levelB)
                        {
                            result = levelA > levelB;
                        }
                        else
                        {
                            // 比较Price
                            int priceA = int.Parse(arrayA[6]);
                            int priceB = int.Parse(arrayB[6]);
                            result = priceA > priceB;
                        }
                    }
                    else // 其余表内的比较
                    {
                        string tableName = null;
                        string itemIndexA = arrayA[1];
                        string itemIndexB = arrayB[1];

                        switch (tabTypeNumberA)
                        {
                            case 2:
                                {
                                    tableName = "item_Custom_Weapon";
                                    break;
                                }
                            case 3:
                                {
                                    tableName = "item_Custom_Armor";
                                    break;
                                }
                            case 4:
                                {
                                    tableName = "Custom_Trinket";
                                    break;
                                }
                        }

                        if (tableName != null)
                        {
                            // 成就点数
                            int achieveA = 0;
                            int achieveB = 0;
                            if (arrayA[7].Length != 0)
                                achieveA = int.Parse(arrayA[7]);
                            if (arrayB[7].Length != 0)
                                achieveB = int.Parse(arrayB[7]);

                            // 比较SubType
                            string subTpyeStringA = GetInfo(tableName, itemIndexA, "SubType");
                            string subTypeStringB = GetInfo(tableName, itemIndexB, "SubType");

                            if (achieveA != achieveB)
                            {
                                result = achieveA > achieveB;
                            }
                            else if (subTpyeStringA == null)
                            {
                                result = false;
                            }
                            else if (subTypeStringB == null)
                            {
                                result = true;
                            }
                            else
                            {
                                int subTypeA = int.Parse(subTpyeStringA);
                                int subTypeB = int.Parse(subTypeStringB);

                                if (subTypeA > subTypeB)
                                {
                                    result = true;
                                }
                                else if (subTypeA < subTypeB)
                                {
                                    result = false;
                                }
                                else
                                {
                                    // 比较DetailType
                                    string detailTypeStringA = GetInfo(tableName, itemIndexA, "DetailType");
                                    string detailTypeStringB = GetInfo(tableName, itemIndexB, "DetailType");

                                    if (detailTypeStringA == null)
                                    {
                                        result = false;
                                    }
                                    else if (detailTypeStringB == null)
                                    {
                                        result = true;
                                    }
                                    else
                                    {
                                        int detailTypeA = int.Parse(detailTypeStringA);
                                        int detailTypeB = int.Parse(detailTypeStringB);

                                        if (detailTypeA > detailTypeB)
                                        {
                                            result = true;
                                        }
                                        else if (detailTypeA < detailTypeB)
                                        {
                                            result = false;
                                        }
                                        else
                                        {
                                            // 比较Price
                                            int priceA = int.Parse(arrayA[6]);
                                            int priceB = int.Parse(arrayB[6]);

                                            result = priceA > priceB;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            

            return result;
        }

        /// <summary>
        /// 导出文件
        /// </summary>
        /// <param name="fileName">包含文件名的列</param>
        /// <returns>是否导出成功</returns>
        public bool ExportFiles(string fileName)
        {
            bool exportSuccess = false;
            string resultFolder = "";
            FolderBrowserDialog openFolderDialog1 = new FolderBrowserDialog();
            openFolderDialog1.Description = "请选择导出文件的保存目录";
            openFolderDialog1.ShowNewFolderButton = true;
            if(path == null)
            {
                openFolderDialog1.RootFolder = Environment.SpecialFolder.MyComputer;
            }
            else
            {
                openFolderDialog1.SelectedPath = path;
            }

            if (openFolderDialog1.ShowDialog() == DialogResult.OK)
            {
                resultFolder = openFolderDialog1.SelectedPath + "\\";
                exportPath = openFolderDialog1.SelectedPath;

                string headline = string.Empty;
                int fileNameIndex = -1;

                for (int i = 0; i < fields.Count; i++)
                {
                    string field = (fields[i] as string).TrimEnd(new char[] { '\r', '\n' });
                    if (excludeFields.Contains(field))
                    {
                        excludeIndexList.Add(i);
                    }
                    else
                    {
                        headline = headline + field + "\t";
                    }

                    if (field == fileName)
                    {
                        fileNameIndex = i;
                    }
                }

                if (fileNameIndex == -1) // 没有找到导出文件名所在列
                {
                    MessageBox.Show("没有找到导出文件名所在列!");
                    return false;
                }

                headline = headline.TrimEnd(new char[] { '\r', '\n', '\t' }); // 去掉冗余信息

                foreach (string s in values) // 添加所有数据行到hash表中
                {
                    if(s == "") // 忽略空行
                    {
                        continue;
                    }
                    string[] data = s.Split('\t');
                    if (fileTable[Path.Combine(resultFolder, data[fileNameIndex])] != null)
                    {
                        List<string> lines = fileTable[Path.Combine(resultFolder, data[fileNameIndex])] as List<string>;
                        string t = string.Empty;
                        for (int i = 0; i < data.Length; i++)
                        {
                            if(!excludeIndexList.Contains(i))
                            {
                                t = t + data[i] + "\t";
                            }
                        }
                        //lines.Add(t.TrimEnd('\t'));
                        lines.Add(t.Substring(0, t.Length - 1));
                    }
                    else
                    {
                        List<string> lines = new List<string>();
                        lines.Add(headline);
                        string t = string.Empty;
                        for (int i = 0; i < data.Length; i++)
                        {
                            if (!excludeIndexList.Contains(i))
                            {
                                t = t + data[i] + "\t";
                            }
                        }
                        //lines.Add(t.TrimEnd('\t'));
                        lines.Add(t.Substring(0, t.Length - 1));
                        fileTable[Path.Combine(resultFolder, data[fileNameIndex])] = lines;
                    }
                }

                List<string> fileNameList = new List<string>();
                foreach (string s in fileTable.Keys)
                {
                    fileNameList.Add(s);
                }

                // 导出文件
                foreach (string s in fileTable.Keys)
                {
                    if (fileNameList.Contains(s))
                    {
                        List<string> lines = fileTable[s] as List<string>;

                        string tempValue;

                        // 对行数据进行排序
                        bool finished = true;
                        for (int i = 0; i < lines.Count - 1; i++)
                        {
                            finished = true;

                            for (int j = 0; j < lines.Count - i - 1; j++)
                            {
                                if (IsLargeThan(lines[j], lines[j + 1]))
                                {
                                    tempValue = lines[j + 1];
                                    lines[j + 1] = lines[j];
                                    lines[j] = tempValue;
                                    finished = false;
                                }
                            }

                            if(finished) // 已经不需要再排序
                            {
                                break;
                            }
                        }
                        SaveData(s, lines);
                    }
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

                exportSuccess = true;
            }

            return exportSuccess;
        }

        /// <summary>
        /// 保存默认数据行
        /// </summary>
        /// <param name="fileName">文件名</param>
        /// <param name="values">数据内容</param>
        private void SaveData(string fileName, List<string> values)
        {
            try
            {
                FileInfo fi = new FileInfo(fileName);
                StreamWriter sw;

                if (!fi.Exists) // 文件不存在
                {
                    MessageBox.Show(string.Format("文件{0}不存在,将新建该文件!", fileName));
                    string path = Path.GetDirectoryName(fileName); // 获取目录
                    if(!Directory.Exists(path)) // 目录不存在
                    {
                        Directory.CreateDirectory(path); // 先创建目录，再创建文件
                    }
                    FileStream fs = fi.Create();
                    fs.Close();
                    // sw = fi.CreateText(); // 新建文本
                    sw = new StreamWriter(fileName, false, Encoding.GetEncoding("gb2312"));
                }
                else
                {
                    if(!exportedFiles.Contains(fileName)) // 第一次写文件
                    {
                        // sw = fi.CreateText();
                        sw = new StreamWriter(fileName, false, Encoding.GetEncoding("gb2312"));
                        exportedFiles.Add(fileName);
                    }
                    else // 添加新行
                    {
                        // sw = fi.AppendText(); // 添加文本
                        sw = new StreamWriter(fileName, true, Encoding.GetEncoding("gb2312"));
                    }              
                }
                 
                foreach (string s in values)
                {
                    sw.WriteLine(s);
                }
                sw.Close();
            }
            catch (Exception ex)
            {
                MessageBox.Show(string.Format("对文件{0}进行IO操作时产生异常: " + ex.ToString(), fileName));
            }
        }
    }
}
