using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using System.Collections;
using System.Windows.Forms;


namespace NpcTemplateIO
{
    public class Class1
    {
        private string m_strRelaAIType = "/settings/AIType.tab";
        private string m_strRelaNpcTem = "/settings/NpcTemplate.tab";

        public Class1()
        {
        }

        private void GetAITypeFieldsStartEndIndex(ArrayList fields, ref int iStart, ref int iEnd, ref int iID)
        {
            for (int i = 0; i < fields.Count; i++)
            {
                string fieldname = fields[i].ToString().ToLower();
                if (fieldname == "id")
                {
                    iID = i;
                }
                bool bIsAiTypeField = fieldname.StartsWith("aitype_");
                if (bIsAiTypeField)
                {
                    if (iStart == -1)
                        iStart = i;
                    else
                        iEnd = i;
                }
            }
            fields[fields.Count - 1] = fields[fields.Count - 1].ToString().Replace("\r\n", string.Empty);
        }

        private void DoExport(ArrayList fields, string[] values, string strTargetFileName, bool bAiType/*, int iStart, int iEnd, int iID*/)
        {
            StringBuilder strBuf = new StringBuilder();
            for (int i = 0; i < fields.Count; i++)
            {
                if (i == 0)
                {
                    if (bAiType)
                    {
                        strBuf.Append("AIType");
                        strBuf.Append("\t");
                    }
                    else
                    {
                        strBuf.Append(fields[i]); // "ID"
                        strBuf.Append("\t");
                    }
                }
                else if (fields[i].ToString().StartsWith("AIType_") && bAiType)
                {
                    strBuf.Append(fields[i].ToString().Replace("AIType_", string.Empty));
                    strBuf.Append("\t");
                }
                else if (!fields[i].ToString().StartsWith("AIType_") && (!bAiType))
                {
                    strBuf.Append(fields[i]);
                    strBuf.Append("\t");
                }
            }
            strBuf.Remove(strBuf.Length - 1, 1);
            strBuf.Append("\r\n");

            string firstLine = string.Empty;
            string[] firstCols = null;
            for (int i = 0; i < values.Length; i++)
            {
                string thisLine = values[i];
                if (thisLine.Length == 0) continue;
                string[] cols = thisLine.Split(new char[] { '\t' });

                if (i == 0 && bAiType) // aitype表特殊处理：遇dbnull填充第一行的值。
                {
                    firstLine = thisLine;
                    firstCols = firstLine.Split(new char[] { '\t' });
                }

                for (int j = 0; j < cols.Length; j++)
                {
                    if ((j == 0) || (fields[j].ToString().StartsWith("AIType_") && bAiType) || (!fields[j].ToString().StartsWith("AIType_") && (!bAiType)))
                    {
                        string item = cols[j];

                        // aitype表特殊处理：遇dbnull填充第一行的值。
                        if (bAiType)
                        {
                            if (item.Length == 0)
                            {
                                item = firstCols[j];
                            }
                        }

                        if (fields[j].ToString() == "AIType") // npctemplate.tab里面的aitype不知道为什么有时候没与ID同步
                            strBuf.Append(cols[0]);
                        else
                            strBuf.Append(item);
                        strBuf.Append("\t");
                    }
                }
                strBuf.Remove(strBuf.Length - 1, 1);
                strBuf.Append("\r\n");
            }

            Helper.StringToFile(strBuf.ToString(), strTargetFileName + (bAiType ? m_strRelaAIType : m_strRelaNpcTem));
        }

        public void ExportFile(ArrayList fields, string[] values, string strBasePath)
        {
            //int iStart = -1, iEnd = -1, iID = -1;
            //GetAITypeFieldsStartEndIndex(fields, ref iStart, ref iEnd, ref iID);
            fields[fields.Count - 1] = fields[fields.Count - 1].ToString().Replace("\r\n", string.Empty);

            // 导出aitype
            DoExport(fields, values, strBasePath, true/*, iStart, iEnd, iID*/);

            // 导出npctemplate
            DoExport(fields, values, strBasePath, false/*, iStart, iEnd, iID*/);

            string strReport = string.Format("成功导出文件！\r\n\r\n{0}\r\n\r\n{1}", strBasePath + m_strRelaAIType, strBasePath + m_strRelaNpcTem);
            //MessageBox.Show(strReport);
            //Console.WriteLine(strReport);
        }

    }
}
