using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Collections;
using System.Diagnostics;

namespace HelperEditor
{
    public class TabFileData
    {
        private const int TABLE_ROW_MAX = 100;                          // 可能的 ID 最大值

        private string[] tTabHeader;                                    // 文件表头
        private string[,] tTabContent;                                  // 文件内容, 矩阵数组

        private string szMainKeyName;                                       // 主键名字

        // 构造函数
        public TabFileData(string szFileName, string szKey)
        {
            Debug.Assert(File.Exists(szFileName), "文件 '" + szFileName + "' 没有找到。");      // 查找文件是否存在
            StreamReader stFile = new StreamReader(szFileName, Encoding.Default);
            string[] tFileLines = stFile.ReadToEnd().Split(new char[] { '\r', '\n' }, StringSplitOptions.RemoveEmptyEntries);
            stFile.Close();

            szMainKeyName = szKey;                                                              // 记录主键名
            tTabHeader = tFileLines[0].Split('\t');                                             // 获取表头
            tTabContent = new string[TABLE_ROW_MAX, tTabHeader.Length];                         // 初始化文件内容
            for (int nRow = 0; nRow < TABLE_ROW_MAX; nRow++)
                for (int nCol = 0; nCol < tTabHeader.Length; nCol++)
                {
                    tTabContent[nRow, nCol] = "";
                }

            for (int nRow = 1; nRow < tFileLines.Length; nRow++)
            {
                string[] tLine = tFileLines[nRow].Split('\t');
                int nID = Convert.ToInt32(tLine[GetColIndexByFieldName(szKey)]);
                //Debug.Assert(nID >= 0 && nID <= TABLE_ROW_MAX, "TAB文件数据访问，ID越界。");
                for (int nCol = 0; nCol < tTabHeader.Length; nCol++)
                {
                    tTabContent[nID, nCol] = tLine[nCol];
                }
            }
        }

        // *************** 属性 ***************
        /// <summary>
        /// 数据表格的有效记录总数。
        /// </summary>
        public int RecordLength
        {
            get
            {
                int nCount = 0;
                for (int i = 0; i < TABLE_ROW_MAX; i++)
                {
                    if (tTabContent[i, GetColIndexByFieldName(szMainKeyName)] != "")
                        nCount++;
                }
                return nCount;
            }
        }

        /// <summary>
        /// 数据表格记录的最大长度。
        /// </summary>
        public int RecordLengthMax
        {
            get
            {
                return TABLE_ROW_MAX;
            }
        }

        /// <summary>
        /// 数据表格的有效字段数（列数）。
        /// </summary>
        public int FieldLength
        {
            get
            {
                return tTabHeader.Length;
            }
        }

        /// <summary>
        /// 取得表的主键名。
        /// </summary>
        public string MainKeyName
        {
            get
            {
                return szMainKeyName;
            }
        }

        // *************** 方法 ***************
        /// <summary>
        /// 通过字段名获取其相应的列序号。
        /// </summary>
        /// <param name="szFieldName">字段名。</param>
        /// <returns>返回与字段名相应的列序号。</returns>
        public int GetColIndexByFieldName(string szFieldName)
        {
            for (int nCol = 0; nCol < tTabHeader.Length; nCol++)
            {
                if (tTabHeader[nCol] == szFieldName)
                {
                    return nCol;
                }
            }
            return -1;
        }

        /// <summary>
        /// 通过 MainKeyID 和 FieldName 获取数据表格中一个单元的值。
        /// </summary>
        /// <param name="nID">主键 ID，需要一个不小于 0 的整数。</param>
        /// <param name="szFieldName">字段名，用来定位一条记录的某个字段。</param>
        /// <returns>返回一个字符串数据。</returns>
        public string GetTabCell(int nID, string szFieldName)
        {
            int nColIndex = GetColIndexByFieldName(szFieldName);
            //Debug.Assert(nID >= 0 && nID <= TABLE_ROW_MAX, "TAB文件数据访问，ID越界。");
            //Debug.Assert(nColIndex >= 0 && nColIndex < tTabHeader.Length, "TAB文件数据访问，ID越界。");
            return tTabContent[nID, nColIndex];
        }
        /// <summary>
        /// 通过 MainKeyID 和 FieldName 获取数据表格中一个单元的值。
        /// </summary>
        /// <param name="szID">主键 ID，需要一个不小于 0 的整数形式的字符串。</param>
        /// <param name="szFieldName">字段名，用来定位一条记录的某个字段。</param>
        /// <returns>返回一个字符串数据。</returns>
        public string GetTabCell(string szID, string szFieldName)
        {
            int nID = Convert.ToInt32(szID);
            return GetTabCell(nID, szFieldName);
        }

        /// <summary>
        /// 通过 MainKeyID 和 FieldName 设置数据表格中一个单元的值。
        /// </summary>
        /// <param name="nID">主键 ID，需要一个不小于 0 的整数。</param>
        /// <param name="szFieldName">字段名，用来定位一条记录的某个字段。</param>
        /// <param name="szValue">新的单元格值。</param>
        public void SetTabCell(int nID, string szFieldName, string szValue)
        {
            int nColIndex = GetColIndexByFieldName(szFieldName);
            tTabContent[nID, nColIndex] = szValue;
        }

        /// <summary>
        /// 通过 MainKeyID 和 FieldName 设置数据表格中一个单元的值。
        /// </summary>
        /// <param name="szID">主键 ID，需要一个不小于 0 的整数形式的字符串。</param>
        /// <param name="szFieldName">字段名，用来定位一条记录的某个字段。</param>
        /// <param name="szValue">新的单元格值。</param>
        public void SetTabCell(string szID, string szFieldName, string szValue)
        {
            int nID = Convert.ToInt32(szID);
            SetTabCell(nID, szFieldName, szValue);
        }

        public void ExportSettingsFile(string szFileName)
        {
            StreamWriter stFile = new StreamWriter(szFileName, false, Encoding.Default);

            // 写表头
            string szFileHeader = "";
            foreach (string szTitleName in tTabHeader)
                szFileHeader += szTitleName + "\t";
            stFile.WriteLine(szFileHeader);

            for (int i = 0; i < TABLE_ROW_MAX; i++)
            {
                int nKeyIndex = GetColIndexByFieldName(MainKeyName);
                if (tTabContent[i, nKeyIndex] != "")
                {
                    string szFileLine = "";
                    for (int j = 0; j < tTabHeader.Length; j++)
                    {
                        szFileLine += tTabContent[i, j] + "\t";
                    }
                    stFile.WriteLine(szFileLine);
                }
            }
            stFile.Close();
        }

        /// <summary>
        /// 取得当前数据表格的表头数组。
        /// </summary>
        /// <returns>表头数组。</returns>
        public string[] GetTabHeader()
        {
            return tTabHeader;
        }

        /// <summary>
        /// 取得当前数据表格的内容数据数组。
        /// </summary>
        /// <returns>内容数据数组</returns>
        public string[,] GetTabContent()
        {
            return tTabContent;
        }
    }

    public class TabFile
    {
        public static TabFileData EventData;            // 声明事件表
        public static TabFileData ConditionData;        // 声明条件表
        public static TabFileData ActionData;           // 声明行为表
    }
}
