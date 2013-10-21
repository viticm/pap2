using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace AutoQuery
{
    class QueryDir
    {
        public bool Init()
        {
            bool bResult = false;
            StringBuilder sbIni = new StringBuilder(1024);
            FileFolderHelper.ReadIniFile("config", "源码路经", "", sbIni, 1024, System.Environment.CurrentDirectory + "\\AutoQuery.ini");
            m_strFolderToQuery = sbIni.ToString();

            FileFolderHelper.ReadIniFile("config", "输出文件", "", sbIni, 1024, System.Environment.CurrentDirectory + "\\AutoQuery.ini");
            m_strOutputFile = sbIni.ToString();

            FileFolderHelper.ReadIniFile("config", "输出全部", "0", sbIni, 1024, System.Environment.CurrentDirectory + "\\AutoQuery.ini");
            m_bOutputAll = (sbIni.ToString() == "0")? false : true;

            if (m_strFolderToQuery == "" || m_strOutputFile == "")
                return bResult;

            m_sb = new StringBuilder();

            if (m_bOutputAll)
                m_strScriptCode = "";
            else
                m_strScriptCode = FileFolderHelper.FileToString(System.Environment.CurrentDirectory + "\\scriptcode.lua");

            bResult = true;
            return bResult;
        }
        public void StartQuery()
        { 
            DirectoryInfo di = new DirectoryInfo(m_strFolderToQuery);
            if (di == null)
            {
                return;
            }
            m_fi = di as FileSystemInfo;
            Query(m_fi);
            FileFolderHelper.StringToFile(m_sb.ToString(), m_strOutputFile);
        }

        public void Query(FileSystemInfo FInfo)
        { 
            if (!FInfo.Exists)
            {
                return;
            }
            DirectoryInfo di = FInfo as DirectoryInfo;
            if (di == null)
            {
                return;
            }
            FileSystemInfo[] fs = di.GetFileSystemInfos();
            foreach (FileSystemInfo f in fs)
            {
                FileInfo file = f as FileInfo;
                if (file != null)
                {
                    if (file.Extension.ToLower() == ".cs")
                    {
                        String strFileContent = FileFolderHelper.FileToString(file.DirectoryName + "\\" + file.Name);

                        int nS = 0;
                        int nE = 0;
                        int nIndex = 0;
                        bool bFound = false;
                        while(true)
                        {
                            nIndex = strFileContent.IndexOf("CallLuaFunction", nS);
                            if (nIndex == -1)
                                break;

                            nS = nIndex;
                            nIndex = strFileContent.IndexOf(";", nS);
                            nE = nIndex;

                            //截取 函数
                            CharEnumerator ContentEnum = strFileContent.Substring(nS, nE - nS).GetEnumerator();
                            int n = 0;
                            int nCountDot = 0;
                            for (int i = 0; i < nE - nS; i++)
                            {

                                if (!ContentEnum.MoveNext())
                                    break;

                                if (ContentEnum.Current == '(')
                                    n++;
                                if (ContentEnum.Current == ')')
                                {
                                    n--;
                                    if (n == 0)
                                    {
                                        nE = nS + i + 1;
                                        break;
                                    }
                                }
                                if (ContentEnum.Current == ',')
                                    nCountDot++;
                            }

                            //过滤声明
                            nIndex = strFileContent.IndexOf("\"", nS, nE - nS);
                            if (nIndex == -1)
                            {
                                nS = nE;
                                continue;
                            }

                            if (!bFound)
                            {
                                m_sb.Append(file.DirectoryName + "\\" + file.Name + "\r\n");
                            }
                            bFound = true;

                            String strCall = strFileContent.Substring(nS, nE - nS);
                            nS = nE;

                            int nIndexFunNS = 0;
                            int nIndexFunNE = 0;
                            nIndexFunNS = strCall.IndexOf("\"") + 1;
                            nIndexFunNE = strCall.IndexOf("\"", nIndexFunNS);
                            String strFunName = strCall.Substring(nIndexFunNS, nIndexFunNE - nIndexFunNS);

                            //过滤已经存在的函数反查事件
                            if (m_strScriptCode.IndexOf(strFunName) != -1)
                            {
                                continue;
                            }

                            String strToAdd = "\t\tregEvent(e, '" + strFunName + "', '注释', ";
                            while (nCountDot > 0)
                            {
                                strToAdd = strToAdd + "\r\n\t\t\tleParm('Param', GetType('Type'), '" + strFunName + "'), ";
                                nCountDot--;
                            }

                            m_sb.Append("\t" + strCall + "\r\n");
                            m_sb.Append(strToAdd.Substring(0, strToAdd.Length - 2) + ")\r\n");
                        }
                    }    
                }
                else//是文件夹，递归
                {
                    Query(f);
                }
            }
        }

        String m_strFolderToQuery;
        String m_strOutputFile;
        FileSystemInfo m_fi;
        StringBuilder m_sb;
        String m_strScriptCode;
        bool m_bOutputAll;
    }
}
