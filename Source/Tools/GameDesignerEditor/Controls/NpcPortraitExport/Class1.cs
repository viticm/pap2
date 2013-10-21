using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Collections;
using System.Reflection;
using System.Windows.Forms;

namespace NpcPortraitExport
{
    public class Class1
    {
        private ArrayList m_Fields = null;
        private string[] m_Values = null;

        public Class1(ArrayList fields, string[] values)
        {
            m_Fields = fields;
            m_Values = values;
        }

        public void ExportFile(string strRoot)
        {
            StringBuilder sb = new StringBuilder();
            sb.Append("NpcPortraitCameraInfo =\r\n{\r\n");

            foreach (string s in m_Values)
            {
                string[] sa = s.Split(new char[] { '\t' });
                if (sa.Length < 11)
                    continue;

                string oneline = string.Format("\t[{0}] = {{{1}, {2}, {3}, {4}, {5}, {6}, {7}, {8}}},\r\n",
                    sa[0], sa[3], sa[4], sa[5], sa[6], sa[7], sa[8], sa[9], sa[10]);
                sb.Append(oneline);
            }
            sb.Append("}\r\n");

            StringToFile(sb.ToString(), strRoot += "/ui/Script/npcportrait.lua");

            //MessageBox.Show("成功导出文件：" + strRoot + "/ui/Script/npcportrait.lua");
            //Console.WriteLine("成功导出文件：" + strRoot + "/ui/Script/npcportrait.lua");
        }

        // 保存string到文件
        private void StringToFile(string strContent, string strFileName)
        {
            StringToFile(strContent, strFileName, Encoding.GetEncoding("gb2312")); // 解决乱码关键在于Encoding
        }
        private void StringToFile(string strContent, string strFileName, Encoding encoding)
        {
            if (!File.Exists(strFileName))
            {
                CreatePath(strFileName);
                File.Create(strFileName).Close();
            }
            StreamWriter writer = new StreamWriter(strFileName, false, encoding);
            writer.Write(strContent);
            writer.Close();
        }
        private void CreatePath(string fileName)
        {
            string path = Path.GetDirectoryName(fileName); // 获取目录
            try
            {
                if (!Directory.Exists(path)) // 目录不存在
                {
                    Directory.CreateDirectory(path);
                }
            }
            catch (IOException ex)
            {
                //MessageBox.Show("在创建新目录时产生IO异常: " + ex.ToString());
                Console.WriteLine("在创建新目录时产生IO异常: " + ex.ToString());
            }
        }
    }
}
