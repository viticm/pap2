using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using System.Collections;

namespace ClientDiff
{
    public partial class DiffViewForm : Form
    {
        List<CheckBox> checkboxes = new List<CheckBox>();
        //CheckBox[] checkboxes = null;
        DataTable tblAll = null;
        string[] StrKeys = new string[0];
        string[] StrNames = new string[0];
        bool ImmediateReCalculate = true;
        string m_content = string.Empty;
        List<string> m_diffCols1;
        List<string> m_diffCols2;
        string m_filename = string.Empty;

        public DiffViewForm(string filename, string postfix, string keys, string names, string input, string inputbasic, List<string> diffCols1, List<string> diffCols2)
        {
            InitializeComponent();

            m_filename = filename;

            StrKeys = keys.Split(',');
            if (names.Length > 0)
                StrNames = names.Split(',');
            m_content = input;

            if (postfix == "tab")
            {
                ReadTab(input);
            }

            m_diffCols1 = diffCols1;
            m_diffCols2 = diffCols2;
        }

        private void ReadTab(string input)
        {
            tblAll = FileToDataTable(input);

            string strIniFile = Application.StartupPath + "/config_public.ini";
            StringBuilder sb = new StringBuilder(255);

            // 忽略列
            string[] arrIgnoreCols = null;
            FileFolderHelper.ReadIniFile("IgnoreColumns", "Count", "", sb, 255, strIniFile);
            int IgnoreCount = Convert.ToInt32(sb.ToString());
            for (int j = 1; j <= IgnoreCount; j++)
            {
                FileFolderHelper.ReadIniFile("IgnoreColumns", "Table" + j.ToString(), "", sb, 255, strIniFile);
                string tabName = sb.ToString();
                FileFolderHelper.ReadIniFile("IgnoreColumns", "Columns" + j.ToString(), "", sb, 255, strIniFile);
                string columns = sb.ToString();

                if (tabName.ToLower() == m_filename.ToLower())
                {
                    arrIgnoreCols = columns.Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
                    break;
                }
            }

            // ui
            int i = 0;
            foreach (DataColumn col in tblAll.Columns)
            {
                string colname = col.ColumnName;

                // 判断是否被ignore了。
                bool bColIsIgnored = false;
                if (arrIgnoreCols != null)
                {
                    foreach (string ignoreCol in arrIgnoreCols)
                    {
                        if (ignoreCol.ToLower() == colname.ToLower())
                        {
                            bColIsIgnored = true;
                            break;
                        }
                    }
                }
                if (bColIsIgnored)
                {
                    continue;
                }

                // 没有ignore，添加checkbox
                CheckBox checkbox = new CheckBox();
                checkbox.AutoSize = true;
                checkbox.Location = CalculateCheckPos(i);
                checkbox.Name = "checkBox" + i.ToString();
                checkbox.Size = new System.Drawing.Size(78, 16);
                checkbox.TabIndex = i;
                checkbox.Text = colname;
                checkbox.UseVisualStyleBackColor = true;
                checkbox.Checked = true;
                if (ColIsNames(colname) || ColIsKey(colname))
                    checkbox.Enabled = false;
                checkbox.CheckedChanged += new System.EventHandler(this.checkBox_CheckedChanged);
                this.splitContainer1.Panel1.Controls.Add(checkbox);
                checkboxes.Add(checkbox);

                i++;
            }

            // datagridview
            this.dataGridView1.DataSource = tblAll;
            foreach (DataGridViewColumn col in dataGridView1.Columns)
            {
                if (ColIsKey(col.Name) || ColIsNames(col.Name))
                    col.Frozen = true;
            }

            // 去掉ignore的列。
            if (arrIgnoreCols != null)
            {
                foreach (string col in arrIgnoreCols)
                {
                    if (tblAll.Columns.Contains(col))
                        tblAll.Columns.Remove(col);
                }
                ReCalculateTable();
            }

            // 着色
            FileFolderHelper.ReadIniFile("ColoredColumns", "Count", "", sb, 255, strIniFile);
            int ColoredTabCount = Convert.ToInt32(sb.ToString());
            for (int j = 1; j <= ColoredTabCount; j++)
            {
                FileFolderHelper.ReadIniFile("ColoredColumns", "Table" + j.ToString(), "", sb, 255, strIniFile);
                string tabName = sb.ToString();
                FileFolderHelper.ReadIniFile("ColoredColumns", "ColumnColors" + j.ToString(), "", sb, 255, strIniFile);
                string tabColColors = sb.ToString();

                if (tabName.ToLower() == m_filename.ToLower())
                {
                    string[] arrColColors = tabColColors.Split(new char[] { ';' }, StringSplitOptions.RemoveEmptyEntries);
                    foreach (string colColor in arrColColors)
                    {
                        string[] col_and_color = colColor.Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
                        string col = col_and_color[0];
                        string color = col_and_color[1];
                        if (dataGridView1.Columns[col] != null)
                            dataGridView1.Columns[col].DefaultCellStyle.BackColor = Color.FromName(color);
                    }

                    break;
                }
            }
        }
        private void ReCalculateTable()
        {
            DataTable tbl = CloneDataTable(tblAll);

            foreach (CheckBox checkbox in checkboxes)
            {
                if (!checkbox.Checked)
                    tbl.Columns.Remove(checkbox.Text);
            }

            ArrayList rowDelete = new ArrayList();
            foreach (DataRow row in tbl.Rows)
            {
                bool bEmptyRow = true;
                foreach (DataColumn col in tbl.Columns)
                {
                    if (ColIsKey(col.ColumnName) || ColIsNames(col.ColumnName))
                    {
                        continue;
                    }
                    else
                    {
                        if (row[col.ColumnName].ToString().Trim().Length != 0)
                        {
                            bEmptyRow = false;
                            break;
                        }
                    }
                }
                if (bEmptyRow)
                    rowDelete.Add(row);
            }

            foreach (DataRow r in rowDelete)
                r.Delete();

            this.dataGridView1.DataSource = tbl;
        }
        private DataTable CloneDataTable(DataTable oldTbl)
        {
            DataTable tblnew = oldTbl.Clone();
            foreach (DataRow r in oldTbl.Rows)
            {
                DataRow newRow = tblnew.NewRow();
                newRow.BeginEdit();

                foreach (DataColumn col in oldTbl.Columns)
                {
                    newRow[col.ColumnName] = r[col.ColumnName];
                }

                tblnew.Rows.Add(newRow);
                newRow.EndEdit();
            }
            return tblnew;
        }

        private Point CalculateCheckPos(int index)
        {
            int maxwidth = 1200;
            int xdiff = 160;
            int ydiff = 15;
            int num_a_row = maxwidth / xdiff;

            int row_index = index % num_a_row;
            int col_index = index / num_a_row;

            return new Point(12 + row_index * xdiff, 42 + col_index * ydiff);
        }

        private void checkBox_CheckedChanged(object sender, EventArgs e)
        {
            if (ImmediateReCalculate)
                ReCalculateTable();
        }
        private bool ColIsKey(string colname)
        {
            bool bIsKey = false;
            foreach (string k in StrKeys)
            {
                if (colname == k)
                {
                    bIsKey = true;
                    break;
                }
            }
            return bIsKey;
        }
        private bool ColIsNames(string colname)
        {
            bool bIsName = false;
            foreach (string name in StrNames)
            {
                if (colname == name)
                {
                    bIsName = true;
                    break;
                }
            }
            return bIsName;
        }



        private DataTable FileToDataTable(string filecontent)
        {
            DataTable tbl = new DataTable();

            // 获取文件内容
            string content = filecontent;
            string[] lines = content.Split(new string[] { "\r\n" }, StringSplitOptions.RemoveEmptyEntries);
            string headline = lines[0];
            string[] colnames = headline.Split(new char[] { '\t' });

            bool[] bColAddFirst = new bool[colnames.Length];
            for (int i = 0; i < bColAddFirst.Length; i++)
            {
                bColAddFirst[i] = false;
            }
            for (int i = 0; i < bColAddFirst.Length; i++)
            {
                string colname = colnames[i];

                if (ColIsKey(colname) || ColIsNames(colname))
                    bColAddFirst[i] = true;
            }

            // 添加列
            int iii = -1;
            foreach (string colname in colnames)
            {
                iii++;
                if (!tbl.Columns.Contains(colname) && bColAddFirst[iii]) // 列名未在表中
                {
                    DataColumn c = tbl.Columns.Add(colname);
                    c.MaxLength = 3000; // 设置最大长度
                }
            }
            iii = -1;
            foreach (string colname in colnames)
            {
                iii++;
                if (!tbl.Columns.Contains(colname) && !bColAddFirst[iii]) // 列名未在表中
                {
                    DataColumn c = tbl.Columns.Add(colname);
                    c.MaxLength = 3000; // 设置最大长度
                }
            }

            // 添加内容
            bool bFirstLine = true;
            foreach (string eachLine in lines)
            {
                if (bFirstLine)
                {
                    bFirstLine = false;
                    continue;
                }

                string[] strFields = eachLine.Split(new char[1] { '\t' });

                DataRow newRow = tbl.NewRow();
                newRow.BeginEdit();

                if (!WriteRow(ref tbl, ref newRow, ref colnames, ref strFields))
                {
                    MessageBox.Show("导入失败，所有新增数据将不被导入!",
                        "导入数据表", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return null;
                }

                tbl.Rows.Add(newRow);
                newRow.EndEdit();
            }

            // save
            tbl.AcceptChanges();

            return tbl;
        }

        private bool WriteRow(ref DataTable tbl, ref DataRow row, ref string[] strColNames, ref string[] strFields)
        {
            try
            {
                for (int i = 0; i < strColNames.Length; i++)
                {
                    if (strColNames[i].Length == 0)
                        continue;

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

        private void buttonAll_Click(object sender, EventArgs e)
        {
            ImmediateReCalculate = false;

            foreach (CheckBox checkbox in checkboxes)
                if (!ColIsKey(checkbox.Text) && !ColIsNames(checkbox.Text))
                    checkbox.Checked = true;

            ReCalculateTable();
            ImmediateReCalculate = true;
        }

        private void buttonReverse_Click(object sender, EventArgs e)
        {
            ImmediateReCalculate = false;

            foreach (CheckBox checkbox in checkboxes)
                if (!ColIsKey(checkbox.Text) && !ColIsNames(checkbox.Text))
                    checkbox.Checked = !checkbox.Checked;

            ReCalculateTable();
            ImmediateReCalculate = true;
        }

        private void buttonClear_Click(object sender, EventArgs e)
        {
            ImmediateReCalculate = false;

            foreach (CheckBox checkbox in checkboxes)
                if (!ColIsKey(checkbox.Text) && !ColIsNames(checkbox.Text))
                    checkbox.Checked = false;

            ReCalculateTable();
            ImmediateReCalculate = true;
        }

        private void button1_Click(object sender, EventArgs e) // 保存
        {
            string targetsavefile = FileFolderHelper.BrowseFileSave();
            FileFolderHelper.StringToFile(GetDataGridViewContent(), targetsavefile);
        }

        private string GetDataGridViewContent()
        {
            DataTable tbl = (DataTable)this.dataGridView1.DataSource;
            StringBuilder sb = new StringBuilder();
            // 列名
            foreach (DataColumn col in tbl.Columns)
            {
                sb.Append(col.ColumnName);
                sb.Append('\t');
            }
            sb.Remove(sb.Length - 1, 1);
            sb.Append("\r\n");
            // 行内容
            foreach (DataRow row in tbl.Rows)
            {
                foreach (DataColumn col in tbl.Columns)
                {
                    sb.Append(row[col.ColumnName].ToString());
                    sb.Append('\t');
                }
                sb.Remove(sb.Length - 1, 1);
                sb.Append("\r\n");

            }

            return sb.ToString();
        }

        private void buttonDisplayDiffCol_Click(object sender, EventArgs e)
        {
            DiffColsForm frm = new DiffColsForm(m_diffCols1, m_diffCols2);
            frm.ShowDialog();
        }
    }
}