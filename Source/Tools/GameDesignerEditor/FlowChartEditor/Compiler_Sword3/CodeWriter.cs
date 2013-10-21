using System;
using System.IO;
using System.Collections.Generic;
using System.Text;

namespace Compiler_Sword3
{
    class CodeWriter
    {
        private StreamWriter m_f = null;
        private Encoding m_gb2312Encoding = Encoding.GetEncoding("gb2312");
        public bool OpenFile(string strfilename)
        {
            DirectoryInfo dinfo = Directory.GetParent(strfilename);
            dinfo.Create(); 
            if (m_f != null)
               m_f.Close();
           m_f = new StreamWriter(strfilename, false, m_gb2312Encoding);//File.CreateText(strfilename);

            return true;
        }
        public bool CloseFile()
        {
            if (m_f != null)
            {
                m_f.Close();
                m_f = null;
            }
            return true;
        }

        public void print(string s)
        {
            m_f.Write(s);
        }

        public void println(string s)
        {

            m_f.WriteLine(s);
        }
    }

    class CodeWriterMemory
    {
        private StringBuilder m_strContentBuf = new StringBuilder();

        public void Init()
        {
            m_strContentBuf = new StringBuilder();
        }

        public void print(string s)
        {            
            m_strContentBuf.Append(s.Replace("\r\n", "\n").Replace("\n", "\r\n"));
        }

        public void println(string s)
        {
            m_strContentBuf.AppendLine(s);
        }

        public String ToString()
        {
            return m_strContentBuf.ToString();
        }
    };
};
