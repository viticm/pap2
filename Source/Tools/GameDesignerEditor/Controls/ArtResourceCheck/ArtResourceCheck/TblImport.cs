using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Data;
using System.Data.SqlClient;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.Diagnostics;
using System.Reflection;
using System.Data.OleDb;

namespace ArtResourceCheck
{
    public partial class Helper
    {
        public static bool ImportTab(string tblName, string strFile, SqlConnection Conn)
        {
            //////////////////////////////////////////////////////////////////////////
            // 准备工作
            //////////////////////////////////////////////////////////////////////////
            string[] strLines = null;
            string[] strColNames = null;

            DataTable tbl = GetDataTable("SELECT * FROM " + tblName, Conn);
            List<string> newNodeList = new List<string>(); // 新增加的树结点名称链表
            List<string> newColumnList = new List<string>(); // 新增加的列的名称链表
            List<DataRow> overwriteRowList = new List<DataRow>(); // 被重写的数据行的链表
            int rowNumber = tbl.Rows.Count; // 数据表的行数
            int iOverwriteCount = 0; // 覆盖的行数
            int iAddCount = 0; // 新增的行数

            // split
            string strContent = FileFolderHelper.FileToString(strFile);
            if (strContent == string.Empty)
            {
                MessageBox.Show("不能读取文件内容，可能文件正在被其他程序打开。", "警告", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return false;
            }

            strLines = strContent.Split(new char[2] { '\r', '\n' }, StringSplitOptions.RemoveEmptyEntries);

            bool bHasTitle = true;
            if (bHasTitle && strLines.Length < 2)
                return false;

            if (bHasTitle)
            {
                strColNames = strLines[0].Split(new char[1] { '\t' }, StringSplitOptions.RemoveEmptyEntries);
                for (int i = 0; i < strColNames.Length; i++)
                {
                    strColNames[i] = strColNames[i].Trim();
                }
            }

            //////////////////////////////////////////////////////////////////////////
            // 询问是否全部导入
            //////////////////////////////////////////////////////////////////////////
            try // 开始对临时表进行操作，不实际写入数据库，可以忽略所有产生的异常
            {
                // 添加字段
                foreach (string colname in strColNames)
                {
                    if (!tbl.Columns.Contains(colname)) // 列名未在表中
                    {
                        DataColumn c = tbl.Columns.Add(colname);
                        c.MaxLength = 3000; // 设置最大长度
                        newColumnList.Add(colname);
                    }
                }


                // 读取数据, 考虑是否是新表
                bool bFirstLine = true;
                bool bTblHasPrimKey = (tbl.PrimaryKey.Length != 0);

                foreach (string eachLine in strLines)
                {
                    // 第一行是字段名; 检查
                    if (bHasTitle)
                    {
                        if (bFirstLine)
                        {
                            bFirstLine = false;
                            continue;
                        }
                    }
                    string[] strFields = eachLine.Split(new char[1] { '\t' });
                    if (eachLine.Split(new char[1] { '\t' }, StringSplitOptions.RemoveEmptyEntries).Length == 0)
                    {
                        MessageBox.Show("发现文件中有空行，请检查。");
                        return false;
                    }

                    // 开始导入
                    if (!bTblHasPrimKey) // 没定义key，当新表处理，添加全部行
                    {
                        DataRow newRow = tbl.NewRow();
                        newRow.BeginEdit();

                        if (!WriteRow(ref tbl, ref newRow, ref strColNames, ref strFields))
                        {
                            MessageBox.Show("导入失败，所有新增数据将不被导入!",
                                "导入数据表", MessageBoxButtons.OK, MessageBoxIcon.Error);
                            return false;
                        }

                        tbl.Rows.Add(newRow);
                        newRow.EndEdit();

                        iAddCount++;
                    }

                    else // 是旧表，需要根据key判断原来是否存在该行
                    {
                        string strAsking = "已经存在 ";

                        // format key values to find
                        object[] objKeyValues = new object[tbl.PrimaryKey.Length];
                        int iKeyIndex = 0;
                        foreach (DataColumn col in tbl.PrimaryKey)
                        {
                            string primKeyToFind = col.ColumnName;
                            for (int i = 0; i < strColNames.Length; i++)
                            {
                                if (strColNames[i].Trim().ToLower() == primKeyToFind.Trim().ToLower())
                                {
                                    if (strFields[i] == string.Empty)
                                    {
                                        string strError = string.Format("在导入表 {0} 时发现某些主键为空的行。\r\n请检查源文件 {1}。", tblName, strFile);
                                        MessageBox.Show(strError, "失败", MessageBoxButtons.OK, MessageBoxIcon.Error);
                                        return false;
                                    }

                                    objKeyValues[iKeyIndex++] = strFields[i];
                                    strAsking += primKeyToFind;
                                    strAsking += "为";
                                    strAsking += strFields[i];
                                    strAsking += " ";
                                }
                            }
                        }
                        strAsking += "的行。";

                        // find it
                        DataRow row = tbl.Rows.Find(objKeyValues);
                        if (row != null) // 有相同key的行，询问
                        {
                            if (!WriteRow(ref tbl, ref row, ref strColNames, ref strFields))
                            {
                                MessageBox.Show("导入失败，所有新增数据将不被导入!",
                                    "导入数据表", MessageBoxButtons.OK, MessageBoxIcon.Error);
                                return false;
                            }

                            overwriteRowList.Add(row); // 添加入重写行链表
                            iOverwriteCount++;
                        }

                        // 加入新行
                        else
                        {
                            DataRow newRow = tbl.NewRow();
                            newRow.BeginEdit();

                            if (!WriteRow(ref tbl, ref newRow, ref strColNames, ref strFields))
                            {
                                MessageBox.Show("导入失败，所有新增数据将不被导入!",
                                    "导入数据表", MessageBoxButtons.OK, MessageBoxIcon.Error);
                                return false;
                            }

                            tbl.Rows.Add(newRow);
                            newRow.EndEdit();

                            iAddCount++;
                        }
                    }
                }

            }
            catch (Exception ex)
            {
                MessageBox.Show("读取数据行时产生异常: " + ex.ToString() + "\r\n\r\n新增数据将不被保存",
                                    "导入数据表", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return false;
            }

            // 提交 
            if (Conn.State == ConnectionState.Closed) // 打开sql连接
            {
                Conn.Open();
            }

            // 允许数据库系统表更新
            SqlCommand cfg_cmd = Conn.CreateCommand();
            cfg_cmd.CommandText = "sp_configure 'allow updates', '1'";
            cfg_cmd.ExecuteNonQuery();
            cfg_cmd.CommandText = "reconfigure with override";
            cfg_cmd.ExecuteNonQuery();

            SqlTransaction transaction = Conn.BeginTransaction(); // 开启事务
            SqlCommand cmd = new SqlCommand(); // 新建sql命令
            cmd.Connection = Conn;
            cmd.Transaction = transaction; // 绑定事务对象

            int col_index = 0; // 列序号
            int index = 0; // 行计数游标

            List<string> strColNameList = new List<string>(); // 记录导入列名的链表
            foreach (string s in strColNames)
            {
                strColNameList.Add(s);
            }

            try
            {
                // 添加字段
                foreach (string colname in newColumnList)
                {
                    string columnName = colname.Replace("]", "]]"); // 替换特殊字符
                    cmd.CommandText = string.Format("ALTER TABLE {0} ADD [{1}] [varchar] (3000) NULL", tblName, columnName);
                    cmd.ExecuteNonQuery();
                    newNodeList.Add(colname);
                }

                // 检查是否导入列的顺序发生了变化
                bool sameOrder = true; // 列顺序是否变化的标记
                for (int i = 0; i < strColNames.Length; i++)
                {
                    if (strColNames[i] != tbl.Columns[i].ColumnName)
                    {
                        sameOrder = false;
                        break;
                    }
                }

                if (!sameOrder) // 导入列的顺序发生了变化，需要重排序
                {
                    // 因为系统表的列重排序后可能会出现数据问题，所以先清空所有行数据，调整后再重新加入
                    cmd.CommandText = string.Format("DELETE FROM {0}", tblName);
                    cmd.ExecuteNonQuery();

                    foreach (DataColumn c in tbl.Columns) // 所有列colid加1000，为重排序做准备
                    {
                        col_index++;
                        cmd.CommandText = string.Format("UPDATE syscolumns SET colid = '{0}' WHERE name = '{1}' AND id = object_id('{2}')", col_index + 1000, c.ColumnName, tblName);
                        cmd.ExecuteNonQuery();
                    }

                    col_index = 0;

                    foreach (string colname in strColNames) // 新列重排序
                    {
                        col_index++;
                        cmd.CommandText = string.Format("UPDATE syscolumns SET colid = '{0}' WHERE name = '{1}' AND id = object_id('{2}')", col_index, colname, tblName);
                        cmd.ExecuteNonQuery();
                    }

                    foreach (DataColumn c in tbl.Columns) // 旧列重排序
                    {
                        string colname = c.ColumnName;
                        if (!strColNameList.Contains(colname))
                        {
                            col_index++;
                            cmd.CommandText = string.Format("UPDATE syscolumns SET colid = '{0}' WHERE name = '{1}' AND id = object_id('{2}')", col_index, colname, tblName);
                            cmd.ExecuteNonQuery();
                        }
                    }

                    // 恢复主键约束 列重排序可能改变了原先的主键约束
                    DataColumn[] primaryKeys = tbl.PrimaryKey;
                    if (primaryKeys.Length > 0) // 存在主键约束
                    {
                        string keyString = string.Empty;
                        foreach (DataColumn c in primaryKeys)
                        {
                            keyString = keyString + c.ColumnName + ",";
                        }

                        // 获取主键约束
                        // cmd.CommandText = string.Format("SELECT constrname FROM sysconstraints WHERE tabid IN (SELECT tabid FROM systables WHERE tabname = '{0}')", tblName);
                        cmd.CommandText = string.Format("SELECT name FROM sysindexes WHERE id = OBJECT_ID('{0}') AND name LIKE 'PK%'", tblName);
                        object constraint = cmd.ExecuteScalar();

                        // 删除主键约束
                        cmd.CommandText = string.Format("ALTER TABLE {0} DROP CONSTRAINT {1}", tblName, constraint);
                        cmd.ExecuteNonQuery();

                        // 增加新的主键约束
                        cmd.CommandText = string.Format("ALTER TABLE {0} ADD CONSTRAINT PK_{1} PRIMARY KEY({2})", tblName, tblName, keyString.TrimEnd(','));
                        cmd.ExecuteNonQuery();
                    }

                    cmd.CommandText = string.Format("SELECT OBJECTPROPERTY(OBJECT_ID('{0}'), 'TableHasIdentity')", tblName);
                    object executeResult = cmd.ExecuteScalar();

                    if (executeResult != null && executeResult.ToString() == "1") // 检查数据表是否有自动编号列
                    {
                        // 允许将值插入到自动编号列中
                        cmd.CommandText = string.Format("SET IDENTITY_INSERT {0} ON", tblName);
                        cmd.ExecuteNonQuery();
                    }

                    // 重新添加表数据
                    foreach (DataRow r in tbl.Rows)
                    {
                        string newValue = "";
                        string columns = "";

                        //for (int i = 0; i < strColNames.Length; i++)
                        for (int i = 0; i < tbl.Columns.Count; i++)
                        {
                            string raw_colname = tbl.Columns[i].ColumnName;

                            string value = r[raw_colname].ToString().Replace("'", "''"); // 替换特殊字符
                            string columnName = raw_colname.Replace("]", "]]"); // 替换特殊字符
                            if (r[raw_colname] is Boolean) // 判断是否是布尔值
                            {
                                value = (bool)r[raw_colname] ? "1" : "0";
                            }

                            if (value == string.Empty) // 自动忽略空值，解决数字的空值转换的问题
                            {
                                continue;
                            }
                            else
                            {
                                columns += string.Format("[{0}], ", columnName);
                                newValue += string.Format("'{0}', ", value);
                            }
                        }

                        newValue = newValue.Trim().TrimEnd(',');
                        columns = columns.Trim().TrimEnd(',');
                        cmd.CommandText = string.Format("INSERT INTO {0} ({1}) VALUES ({2})", tblName, columns, newValue);
                        try
                        {
                            cmd.ExecuteNonQuery();
                        }
                        catch (SqlException ex)
                        {
                            MessageBox.Show(ex.Message + "\r\n\r\n" + cmd.CommandText);
                            throw ex;
                        }
                    }
                }
                else // 列顺序不变，只需添加行数据即可
                {
                    // 覆盖旧的数据行
                    if (overwriteRowList.Count > 0)
                    {
                        DataColumn[] keys = tbl.PrimaryKey;
                        foreach (DataRow r in overwriteRowList)
                        {
                            string setValue = "";
                            string keyValue = "";

                            foreach (string s in strColNames)
                            {
                                string columnName = s.Replace("]", "]]"); // 替换特殊字符
                                string value = r[s].ToString().Replace("'", "''"); // 替换特殊字符
                                if (r[s] is Boolean) // 判断是否是布尔值
                                {
                                    value = (bool)r[s] ? "1" : "0";
                                }

                                if (value == string.Empty) // 空值是特殊条件，需要特殊处理
                                {
                                    setValue += string.Format("[{0}] = NULL, ", columnName);
                                }
                                else
                                {
                                    setValue += string.Format("[{0}] = \'{1}\', ", columnName, value);
                                }
                            }
                            setValue = setValue.Trim().TrimEnd(',');

                            foreach (DataColumn c in keys)
                            {
                                string columnName = c.ColumnName.Replace("]", "]]"); // 替换特殊字符
                                string value = r[c.ColumnName].ToString().Replace("'", "''"); // 替换特殊字符
                                keyValue += string.Format("[{0}] = \'{1}\' AND ", columnName, value);
                            }
                            keyValue = keyValue.Substring(0, keyValue.Length - 5);

                            cmd.CommandText = string.Format("UPDATE {0} SET {1} WHERE {2}", tblName, setValue, keyValue);
                            try
                            {
                                cmd.ExecuteNonQuery();
                            }
                            catch (SqlException ex)
                            {
                                MessageBox.Show(ex.Message + "\r\n\r\n" + cmd.CommandText);
                                throw ex;
                            }
                        }
                    }

                    // 增加新数据行
                    foreach (DataRow r in tbl.Rows)
                    {
                        if (index < rowNumber) // 忽略旧的数据行
                        {
                            index++;
                            continue;
                        }

                        string newValue = "";
                        string columns = "";

                        for (int i = 0; i < strColNames.Length; i++)
                        {
                            string value = r[strColNames[i]].ToString().Replace("'", "''"); // 替换特殊字符
                            string columnName = strColNames[i].Replace("]", "]]"); // 替换特殊字符
                            if (r[strColNames[i]] is Boolean) // 判断是否是布尔值
                            {
                                value = (bool)r[strColNames[i]] ? "1" : "0";
                            }

                            if (value == string.Empty) // 自动忽略空值，解决数字的空值转换的问题
                            {
                                continue;
                            }
                            else
                            {
                                columns += string.Format("[{0}], ", columnName);
                                newValue += string.Format("'{0}', ", value);
                            }
                        }

                        newValue = newValue.Trim().TrimEnd(',');
                        columns = columns.Trim().TrimEnd(',');
                        cmd.CommandText = string.Format("INSERT INTO {0} ({1}) VALUES ({2})", tblName, columns, newValue);
                        try
                        {
                            cmd.ExecuteNonQuery();
                        }
                        catch (SqlException ex)
                        {
                            MessageBox.Show(ex.Message + "\r\n\r\n" + cmd.CommandText);
                            throw ex;
                        }
                    }
                }

                tbl.AcceptChanges();
                transaction.Commit(); // 提交事务
            }
            catch (SqlException ex)
            {
                MessageBox.Show("数据更新时产生异常: " + ex.ToString() + "\r\n\r\n新增数据将不被保存",
                     "导入数据表", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                transaction.Rollback(); // 事务回滚
                return false;
            }
            finally
            {
                // 禁止数据库系统标更新
                cfg_cmd.CommandText = "sp_configure 'allow updates', '0'";
                cfg_cmd.ExecuteNonQuery();
                cfg_cmd.CommandText = "reconfigure with override";
                cfg_cmd.ExecuteNonQuery();

                if (Conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    Conn.Close();
                }
            }

            //////////////////////////////////////////////////////////////////////////
            // 更新内存 报告
            //////////////////////////////////////////////////////////////////////////
            string rpt = string.Format("成功导入数据,覆盖行：{0}  增加行：{1}", iOverwriteCount, iAddCount);
            //MessageBox.Show(rpt);
            return true;
        }

        private static bool WriteRow(ref DataTable tbl, ref DataRow row, ref string[] strColNames, ref string[] strFields)
        {
            try
            {
                for (int i = 0; i < strColNames.Length; i++)
                {
                    if (i < strFields.Length)
                    {
                        if (strFields[i] == "")
                            row[strColNames[i]] = DBNull.Value;
                        else
                        {
                            if (tbl.Columns[strColNames[i]].DataType.Name == "Boolean")
                            {
                                if (strFields[i].Trim() == "0")
                                    row[strColNames[i]] = false;
                                else
                                    row[strColNames[i]] = true;
                            }
                            else
                            {
                                try
                                {
                                    row[strColNames[i]] = strFields[i];
                                }
                                catch (Exception ex)
                                {
                                    string err = string.Format("在修改第 {0} 个数据行 (字段名: {1}; 字段值: {2}) 时发生错误, 请修改数据表后重新导入。\r\n\r\n错误信息：{3}", i, strColNames[i], strFields[i], ex.Message);
                                    MessageBox.Show(err);
                                    return false;
                                }
                            }
                        }
                    }
                    else
                    {
                        row[strColNames[i]] = DBNull.Value;
                    }
                }
            }
            catch//Exception ex)
            {
                return false;
            }

            return true;
        }
    }
}
