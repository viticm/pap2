using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Collections;

namespace ClientDiff
{
    public class DiffGenerator
    {
        static public bool DiffGen(string file1, string file2, string postfix, string strkeys, string namefield, string rowcondition, ref string output, ref string output_basic, ref List<string> diffCols1, ref List<string> diffCols2)
        {
            if (postfix == "tab")
            {
                return DiffGenTab(file1, file2, strkeys, namefield, rowcondition, ref output, ref output_basic, ref diffCols1, ref diffCols2);
            }
            else
            {

            }

            return true;
        }

        private static int CompareByKey(string s1, string s2)
        {
            string[] cols1 = s1.Split('\t');
            string[] cols2 = s2.Split('\t');

            int id1 = -1, id2 = -1;
            bool bIsNumber1 = Int32.TryParse(cols1[0], out id1);
            bool bIsNumber2 = Int32.TryParse(cols2[0], out id2);
            if (!bIsNumber1)
                return -1;
            else if (!bIsNumber2)
                return 1;
            
            if (id1 > id2)
                return 1;
            else if (id1 < id2)
                return -1;
            else
                return 0;
        }

        static private string ObjToString(object o)
        {
            return o.ToString();
        }
        static private string DataRowToString(DataRow row, DataTable t)
        {
            StringBuilder sb = new StringBuilder();
            foreach (DataColumn col in t.Columns)
            {
                string s = (row[col.ColumnName] == null || row[col.ColumnName] == DBNull.Value) ? string.Empty : row[col.ColumnName].ToString().Trim();
                sb.Append(s);
                sb.Append('\t');
            }
            sb = sb.Remove(sb.Length - 1, 1);
            return sb.ToString().Trim();
        }
        static private bool RowFilter(string rowCondition, string content1, ref string[] lines1, string content2, ref string[] lines2)
        {
            if (rowCondition.Length == 0) return false;

            DataTable t1 = Helper.GetHelper().CreateDataTable(content1);
            DataRow[] rows1 = t1.Select(rowCondition);
            ArrayList list1 = new ArrayList();
            list1.Add(lines1[0]);
            foreach (DataRow r in rows1)
            {
                list1.Add(DataRowToString(r, t1));
            }
            lines1 = Array.ConvertAll(list1.ToArray(), new Converter<object, string>(ObjToString));

            DataTable t2 = Helper.GetHelper().CreateDataTable(content2);
            DataRow[] rows2 = t2.Select(rowCondition);
            ArrayList list2 = new ArrayList();
            list2.Add(lines2[0]);
            foreach (DataRow r in rows2)
            {
                list2.Add(DataRowToString(r, t2));
            }
            lines2 = Array.ConvertAll(list2.ToArray(), new Converter<object, string>(ObjToString));

            return true;
        }

        static public bool DiffGenTab(string file1, string file2, string strkeys, string namefield, string rowcondition, ref string output, ref string output_basic, ref List<string> diffCols1, ref List<string> diffCols2)
        {
            ArrayList outputlines = new ArrayList();

            string[] keys = strkeys.Split(new char[] { ',' });
            int[] keyIndices = new int[keys.Length];

            string content1 = FileFolderHelper.FileToString(file1);
            string[] lines1 = content1.Split(new string[] { "\r\n" }, StringSplitOptions.RemoveEmptyEntries);

            // 排序文件1
            if (file1.ToLower().EndsWith("other.tab"))
                Array.Sort(lines1, CompareByKey);

            string[] colnames1 = lines1[0].Split(new char[] { '\t' });
            bool[] _1exists = new bool[colnames1.Length];
            for (int i = 0; i < _1exists.Length; i++)
                _1exists[i] = false;

            // namefield处理
            string[] nameFields = new string[0];
            int[] namefieldsindices = new int[0];
            if (namefield.Length > 0)
            {
                nameFields = namefield.Split(',');
                namefieldsindices = new int[nameFields.Length];
                for (int i = 0; i < nameFields.Length; i++)
                {
                    for (int j = 0; j < colnames1.Length; j++)
                    {
                        if (colnames1[j] == nameFields[i])
                        {
                            namefieldsindices[i] = j;
                            break;
                        }
                    }
                }
            }


            // keys
            for (int i = 0; i < keys.Length; i++)
            {
                for (int j = 0; j < colnames1.Length; j++)
                {
                    if (keys[i] == colnames1[j])
                    {
                        keyIndices[i] = j;
                        break;
                    }
                }
            }

            string content2 = FileFolderHelper.FileToString(file2);
            string[] lines2 = content2.Split(new string[] { "\r\n" }, StringSplitOptions.RemoveEmptyEntries);

            // 过滤行
            try
            {
                RowFilter(rowcondition, content1, ref lines1, content2, ref lines2);
            }
            catch (System.Exception ex)
            {
                string err = string.Format("过滤行时发生异常：{0}", ex.Message);
                MessageBox.Show(err);
            }
            
            // 排序文件2
            if (file2.ToLower().EndsWith("other.tab"))
                Array.Sort(lines2, CompareByKey);
            
            string[] colnames2 = lines2[0].Split(new char[] { '\t' });
            bool[] _2exists = new bool[colnames2.Length];
            for (int i = 0; i < _2exists.Length; i++)
                _2exists[i] = false;

            for (int i = 0; i < colnames1.Length; i++)
            {
                for (int j = 0; j < colnames2.Length; j++)
                {
                    if (colnames1[i] == colnames2[j])
                    {
                        _1exists[i] = true;
                        _2exists[j] = true;
                        break;
                    }
                }
            }

            // 找出纯粹的列差异
            for (int i = 0; i < colnames1.Length; i++)
            {
                if (!_1exists[i])
                    diffCols1.Add(colnames1[i]);
            }
            for (int i = 0; i < colnames2.Length; i++)
            {
                if (!_2exists[i])
                    diffCols2.Add(colnames2[i]);
            }

            // 输出
            //string outputcontent = string.Empty;
            StringBuilder sb = new StringBuilder();
            ArrayList useful_indices_1 = new ArrayList();
            ArrayList useful_indices_2 = new ArrayList();
            for (int i = 0; i < _1exists.Length; i++)
            {
                if (_1exists[i])
                    useful_indices_1.Add(i);
            }
            for (int i = 0; i < _2exists.Length; i++)
            {
                if (_2exists[i])
                    useful_indices_2.Add(i);
            }
            foreach (int i in useful_indices_1)
            {
                sb.Append(colnames1[i]);
                sb.Append('\t');
            }
            sb = sb.Remove(sb.Length - 1, 1);

            outputlines.Add(sb.ToString());
            sb = new StringBuilder();

            // 决定最后导出是否导出某列
            bool[] bExportCols = new bool[useful_indices_1.Count];
            for (int i = 0; i < bExportCols.Length; i++)
                bExportCols[i] = false;
            foreach (int ikey in keyIndices)
            {
                bExportCols[ikey] = true;
            }
            foreach (int iname in namefieldsindices)
            {
                bExportCols[iname] = true;
            }

            // 内容比对
            int rowindex1 = 1, rowindex2 = 1;
            bool bThisLineDiff;

        FIND_AGAIN_IN_1:
            bThisLineDiff = false;
            //if (rowindex1 >= lines1.Length)
            //    goto FINALIZE;

            string thisLine1 = string.Empty;
            string[] cols1 = null;
            if (rowindex1 < lines1.Length)
            {
                thisLine1 = lines1[rowindex1];
                cols1 = thisLine1.Split(new char[] { '\t' });
            }
            else
                cols1 = new string[colnames1.Length];
            //string[] cols1 = thisLine1.Split(new char[] { '\t' });

            string keycontent1 = string.Empty;
            for (int i = 0; i < keyIndices.Length; i++)
            {
                keycontent1 += cols1[keyIndices[i]];
                keycontent1 += "\t";
            }

        FIND_AGAIN_IN_2:
            string thisLine2 = string.Empty;
            string keycontent2 = string.Empty;
            string[] cols2 = null;
            //if (rowindex2 >= lines2.Length)
            //    goto FINALIZE;

            if (rowindex2 < lines2.Length)
            {
                thisLine2 = lines2[rowindex2];
                cols2 = thisLine2.Split(new char[] { '\t' });
            }
            else
                cols2 = new string[colnames2.Length];
            //cols2 = thisLine2.Split(new char[] { '\t' });

            for (int i = 0; i < keyIndices.Length; i++)
            {
                keycontent2 += cols2[keyIndices[i]];
                keycontent2 += "\t";
            }

            // check
            if (rowindex1 >= lines1.Length && rowindex2 >= lines2.Length)
                goto FINALIZE;

            // compare
            switch (CompareKey(keycontent1, keycontent2))
            {
                case 0:
                    rowindex1++;
                    rowindex2++;
                    goto OUTPUT_THIS_LINE_BOTH;
                case 1:
                    rowindex2++;
                    //cols1 = new string[useful_indices_1.Count];
                    cols1 = new string[colnames1.Length];
                    goto OUTPUT_THIS_LINE_BOTH;
                    //goto FIND_AGAIN_IN_2;
                case -1:
                    rowindex1++;
                    //cols2 = new string[useful_indices_2.Count];
                    cols2 = new string[colnames2.Length];
                    goto OUTPUT_THIS_LINE_BOTH;
                    //goto FIND_AGAIN_IN_1;
                default:
                    break;
            }

        OUTPUT_THIS_LINE_BOTH:
            // 接下来可以比对cols1和cols2
            for (int i = 0; i < useful_indices_1.Count; i++)
            {
                int index1 = Convert.ToInt32(useful_indices_1[i]);
                int index2 = Convert.ToInt32(useful_indices_2[i]);
                string col1 = string.Empty;
                string col2 = string.Empty;
                if (index1 < cols1.Length)
                    col1 = cols1[index1];
                if (index2 < cols2.Length)
                    col2 = cols2[index2];

                if (col1 != col2)
                {
                    sb.Append(col1 + "|" + col2);
                    bExportCols[i] = true;
                    bThisLineDiff = true;
                }
                else
                {
                    bool isKey = false;
                    foreach (int ikey in keyIndices)
                    {
                        if (i == ikey)
                        {
                            isKey = true;
                            break;
                        }
                    }
                    bool isName = false;
                    foreach (int iname in namefieldsindices)
                    {
                        if (i == iname)
                        {
                            isName = true;
                            break;
                        }
                    }
                    if (isKey || isName)
                        sb.Append(col1);
                }
                sb.Append('\t');
            }
            sb = sb.Remove(sb.Length - 1, 1);

            if (bThisLineDiff)
                outputlines.Add(sb.ToString());
            sb = new StringBuilder();

            goto FIND_AGAIN_IN_1;


        FINALIZE:
            sb = new StringBuilder();
            foreach (string line in outputlines)
            {
                string[] cols = line.Split(new char[] { '\t' });
                for (int i = 0; i < cols.Length; i++)
                {
                    if (bExportCols[i])
                    {
                        sb.Append(cols[i]);
                        sb.Append('\t');
                    }
                }
                sb = sb.Remove(sb.Length - 1, 1);
                sb.Append("\r\n");
            }

            // 输出到内存
            //FileFolderHelper.StringToFile(sb.ToString(), output);
            output = sb.ToString();
            output_basic = string.Empty;

            return true;
        }

        static private int CompareKey(string keycontent1, string keycontent2)
        {
            string[] strkeys1 = keycontent1.Split(new string[] { "\t" }, StringSplitOptions.RemoveEmptyEntries);
            string[] strkeys2 = keycontent2.Split(new string[] { "\t" }, StringSplitOptions.RemoveEmptyEntries);

            if (strkeys1.Length > strkeys2.Length)
                return -1;
            else if (strkeys1.Length < strkeys2.Length)
                return 1;

            for (int i = 0; i < strkeys1.Length; i++)
            {
                int key1 = -1, key2 = -1;
                if (!Int32.TryParse(strkeys1[i], out key1) || !Int32.TryParse(strkeys2[i], out key2))
                {
                    throw new Exception("错误的字符串格式。\r\n可能你选择了不正确的主键字段。");
                }
                //int key1 = Convert.ToInt32(strkeys1[i]);
                //int key2 = Convert.ToInt32(strkeys2[i]);
                if (key1 > key2)
                    return 1;
                else if (key1 < key2)
                    return -1;
            }
            return 0;
        }
    }
}
